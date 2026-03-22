cmake_minimum_required(VERSION 3.22)

project(AstroFind
    VERSION 0.8.0
    DESCRIPTION "Modern asteroid detection software for citizen science"
    LANGUAGES CXX C
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── Binary hardening ──────────────────────────────────────────────────────────
# CMAKE_POSITION_INDEPENDENT_CODE: adds -fPIC to shared libs, -fPIE to static
# libs/executables — the CMake-idiomatic way to enable PIE across all targets.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
# Stack protector on all targets; FORTIFY_SOURCE only with optimisation enabled
add_compile_options(-fstack-protector-strong)
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-D_FORTIFY_SOURCE=2)
endif()
# Linker hardening for the executable — stored for use in src/CMakeLists.txt
set(ASTROFIND_HARDEN_LINK -pie -Wl,-z,relro,-z,now,-z,noexecstack)

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
