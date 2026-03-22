# ─── Audit target (items 38.1 – 38.5) ────────────────────────────────────────
# Usage:
#   cmake --build build --target audit          # run all tools
#   cmake --build build --target audit-cppcheck
#   cmake --build build --target audit-clang-tidy
#   cmake --build build --target audit-valgrind
#   cmake --build build --target audit-asan
#
# ASan build (38.1): configure a separate build with sanitizers, then run tests.
#   cmake -B build_asan -DCMAKE_BUILD_TYPE=Debug -DASTROFIND_ASAN=ON
#   cmake --build build_asan --parallel
#   build_asan/bin/astrofind_tests --reporter console

set(AUDIT_REPORT_DIR "${CMAKE_BINARY_DIR}/audit")
set(AUDIT_REPORT     "${AUDIT_REPORT_DIR}/audit_report.txt")

# ── Export compile_commands.json (needed by clang-tidy) ──────────────────────
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "" FORCE)

# ── 38.3 cppcheck ─────────────────────────────────────────────────────────────
find_program(CPPCHECK cppcheck)
if(CPPCHECK)
    add_custom_target(audit-cppcheck
        COMMAND ${CMAKE_COMMAND} -E make_directory "${AUDIT_REPORT_DIR}"
        COMMAND ${CPPCHECK}
            --enable=all
            --std=c++23
            --language=c++
            --library=qt
            --suppress=missingIncludeSystem
            --suppress=unmatchedSuppression
            --suppress=checkersReport
            --suppress=unknownMacro
            --error-exitcode=0
            --xml --xml-version=2
            -I "${CMAKE_SOURCE_DIR}/src"
            "${CMAKE_SOURCE_DIR}/src/core"
            "${CMAKE_SOURCE_DIR}/src/ui"
            2> "${AUDIT_REPORT_DIR}/cppcheck.xml"
        COMMAND ${CMAKE_COMMAND} -E echo "cppcheck done → ${AUDIT_REPORT_DIR}/cppcheck.xml"
        COMMENT "38.3 cppcheck static analysis"
        VERBATIM
    )
    message(STATUS "Audit: cppcheck found — audit-cppcheck target enabled")
else()
    add_custom_target(audit-cppcheck
        COMMAND ${CMAKE_COMMAND} -E echo "cppcheck not found — install: sudo dnf install cppcheck"
    )
    message(STATUS "Audit: cppcheck NOT found — install: sudo dnf install cppcheck")
endif()

# ── 38.4 clang-tidy ───────────────────────────────────────────────────────────
find_program(CLANG_TIDY clang-tidy)
find_program(RUN_CLANG_TIDY run-clang-tidy NAMES run-clang-tidy run-clang-tidy-18 run-clang-tidy-17 run-clang-tidy-16)
if(CLANG_TIDY)
    if(RUN_CLANG_TIDY)
        set(_tidy_cmd ${RUN_CLANG_TIDY}
            -clang-tidy-binary ${CLANG_TIDY}
            -p "${CMAKE_BINARY_DIR}"
            -header-filter "^${CMAKE_SOURCE_DIR}/src/.*"
            -checks="-*,bugprone-*,cert-*,cppcoreguidelines-*,misc-*,performance-*,readability-narrowing-conversions"
            -export-fixes "${AUDIT_REPORT_DIR}/clang_tidy_fixes.yaml"
        )
    else()
        # Fallback: run clang-tidy on each source file manually
        file(GLOB_RECURSE _tidy_srcs
            "${CMAKE_SOURCE_DIR}/src/core/*.cpp"
            "${CMAKE_SOURCE_DIR}/src/ui/*.cpp"
            "${CMAKE_SOURCE_DIR}/src/main.cpp"
        )
        set(_tidy_cmd ${CLANG_TIDY}
            -p "${CMAKE_BINARY_DIR}"
            --checks="-*,bugprone-*,cert-*,cppcoreguidelines-*,misc-*,performance-*,readability-narrowing-conversions"
            ${_tidy_srcs}
        )
    endif()
    add_custom_target(audit-clang-tidy
        COMMAND ${CMAKE_COMMAND} -E make_directory "${AUDIT_REPORT_DIR}"
        COMMAND ${_tidy_cmd} > "${AUDIT_REPORT_DIR}/clang_tidy.txt" 2>&1 || true
        COMMAND ${CMAKE_COMMAND} -E echo "clang-tidy done → ${AUDIT_REPORT_DIR}/clang_tidy.txt"
        COMMENT "38.4 clang-tidy static analysis"
        VERBATIM
    )
    message(STATUS "Audit: clang-tidy found — audit-clang-tidy target enabled")
else()
    add_custom_target(audit-clang-tidy
        COMMAND ${CMAKE_COMMAND} -E echo "clang-tidy not found — install: sudo dnf install clang-tools-extra"
    )
    message(STATUS "Audit: clang-tidy NOT found — install: sudo dnf install clang-tools-extra")
endif()

# ── 38.2 Valgrind memcheck ───────────────────────────────────────────────────
find_program(VALGRIND valgrind)
if(VALGRIND)
    add_custom_target(audit-valgrind
        COMMAND ${CMAKE_COMMAND} -E make_directory ${AUDIT_REPORT_DIR}
        COMMAND ${VALGRIND}
            --tool=memcheck
            --leak-check=full
            --show-leak-kinds=definite,indirect
            --error-exitcode=0
            --xml=yes
            --xml-file=${AUDIT_REPORT_DIR}/valgrind.xml
            --suppressions=${CMAKE_SOURCE_DIR}/cmake/valgrind.supp
            $<TARGET_FILE:astrofind_tests>
            --reporter compact
        DEPENDS astrofind_tests
        COMMENT "38.2 Valgrind memcheck"
        VERBATIM
    )
    message(STATUS "Audit: valgrind found — audit-valgrind target enabled")
else()
    add_custom_target(audit-valgrind
        COMMAND ${CMAKE_COMMAND} -E echo "valgrind not found — install: sudo dnf install valgrind"
    )
    message(STATUS "Audit: valgrind NOT found — install: sudo dnf install valgrind")
endif()

# ── 38.1 ASan + UBSan (separate build) ───────────────────────────────────────
# This target configures + builds + runs tests in build_asan/ automatically.
add_custom_target(audit-asan
    COMMAND ${CMAKE_COMMAND} -E make_directory "${AUDIT_REPORT_DIR}"
    COMMAND ${CMAKE_COMMAND} -E echo "Configuring ASan build in ${CMAKE_SOURCE_DIR}/build_asan ..."
    COMMAND ${CMAKE_COMMAND}
        -B "${CMAKE_SOURCE_DIR}/build_asan"
        -S "${CMAKE_SOURCE_DIR}"
        -DCMAKE_BUILD_TYPE=Debug
        -DASTROFIND_ASAN=ON
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        -G "${CMAKE_GENERATOR}"
        --no-warn-unused-cli
        -DCMAKE_MESSAGE_LOG_LEVEL=WARNING
    COMMAND ${CMAKE_COMMAND} --build "${CMAKE_SOURCE_DIR}/build_asan" --parallel
        --target astrofind_tests
    COMMAND ${CMAKE_COMMAND} -E echo "Running ASan tests ..."
    COMMAND "${CMAKE_SOURCE_DIR}/build_asan/bin/astrofind_tests"
        --reporter console
        > "${AUDIT_REPORT_DIR}/asan_tests.txt" 2>&1
        || true
    COMMAND ${CMAKE_COMMAND} -E echo "ASan done → ${AUDIT_REPORT_DIR}/asan_tests.txt"
    COMMENT "38.1 ASan + UBSan: configure build_asan/, build, run tests"
    VERBATIM
)

# ── 38.5 audit meta-target ────────────────────────────────────────────────────
add_custom_target(audit
    COMMAND ${CMAKE_COMMAND} -E make_directory ${AUDIT_REPORT_DIR}
    COMMAND ${CMAKE_COMMAND} -E echo "=== AstroFind Numerical Audit ==="
    COMMAND ${CMAKE_COMMAND} -E echo "Reports written to: ${AUDIT_REPORT_DIR}/"
    COMMAND ${CMAKE_COMMAND} -E echo "  cppcheck.xml       -- 38.3 cppcheck static analysis"
    COMMAND ${CMAKE_COMMAND} -E echo "  clang_tidy.txt     -- 38.4 clang-tidy checks"
    COMMAND ${CMAKE_COMMAND} -E echo "  valgrind.xml       -- 38.2 Valgrind memcheck"
    COMMAND ${CMAKE_COMMAND} -E echo "  asan_tests.txt     -- 38.1 ASan+UBSan test run"
    DEPENDS audit-cppcheck audit-clang-tidy audit-valgrind audit-asan
    COMMENT "38.5 Full numerical audit (cppcheck + clang-tidy + valgrind + asan)"
)
