cmake_minimum_required(VERSION 3.22)

project(AstroFind
    VERSION 1.2.0
    DESCRIPTION "Modern asteroid detection software for citizen science"
    LANGUAGES CXX C
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# compile_commands.json for clang-tidy (item 38.4 / AUD-CI-7). Must be set
# BEFORE any target is created: since CMake 3.20 each target copies it into
# its EXPORT_COMPILE_COMMANDS property at creation time, so setting it later
# (it used to live only in cmake/audit.cmake, included after add_subdirectory)
# produced no database and clang-tidy never analysed a single file in CI.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ── Binary hardening ──────────────────────────────────────────────────────────
# CMAKE_POSITION_INDEPENDENT_CODE: adds -fPIC to shared libs, -fPIE to static
# libs/executables — the CMake-idiomatic way to enable PIE across all targets.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
# Stack protector on all targets; FORTIFY_SOURCE only with optimisation enabled
add_compile_options(-fstack-protector-strong)
# Distribution package builds pass their own level (e.g. Fedora/Arch/Debian
# use -D_FORTIFY_SOURCE=3); only add ours when none is set, to avoid a
# "redefined" warning on every file.
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug"
   AND NOT "${CMAKE_CXX_FLAGS} $ENV{CXXFLAGS} $ENV{CPPFLAGS}" MATCHES "_FORTIFY_SOURCE")
    add_compile_options(-D_FORTIFY_SOURCE=2)
endif()
# Linker hardening for the executable — stored for use in src/CMakeLists.txt
set(ASTROFIND_HARDEN_LINK -pie -Wl,-z,relro,-z,now,-z,noexecstack)

# AUD-CI-9: the local pre-commit gate (scripts/pre-commit, see CONTRIBUTING.md)
# only runs once it is armed in .git/hooks of each clone. Remind at configure
# time when it is not; skipped on CI runners (env CI) and silenced by -Wno-dev.
if(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/.git/hooks"
   AND NOT EXISTS "${CMAKE_SOURCE_DIR}/.git/hooks/pre-commit"
   AND NOT DEFINED ENV{CI})
    message(AUTHOR_WARNING
        "pre-commit hook not armed in this clone. Arm it once with:\n"
        "  ln -sf ../../scripts/pre-commit .git/hooks/pre-commit")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Qt6 auto tools
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Qt6 — LinguistTools is optional (install qt6-linguist to enable translations build)
find_package(Qt6 6.4 REQUIRED COMPONENTS
    Core
    Widgets
    Network
    OpenGL
    OpenGLWidgets
    Concurrent
    Sql
    Xml
    Charts
)
find_package(Qt6 OPTIONAL_COMPONENTS LinguistTools)
qt_standard_project_setup()

# Dependencies
include(cmake/dependencies.cmake)

# ── ASan / UBSan option (item 38.1) ──────────────────────────────────────────
option(ASTROFIND_ASAN "Enable AddressSanitizer + UndefinedBehaviorSanitizer" OFF)
if(ASTROFIND_ASAN)
    message(STATUS "ASan + UBSan enabled")
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()

add_subdirectory(src)
enable_testing()
add_subdirectory(tests)

# ── Audit target (items 38.1–38.5) — after subdirectories so targets exist ──
include(cmake/audit.cmake)
