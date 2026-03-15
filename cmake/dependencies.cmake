include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

# ─── spdlog ────────────────────────────────────────────────────────────────
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.14.1
    GIT_SHALLOW    TRUE
)
set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(SPDLOG_FMT_EXTERNAL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(spdlog)

# ─── nlohmann/json ─────────────────────────────────────────────────────────
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
    GIT_SHALLOW    TRUE
)
set(JSON_BuildTests OFF CACHE BOOL "" FORCE)
set(JSON_Install OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(nlohmann_json)

# ─── cfitsio ───────────────────────────────────────────────────────────────
find_package(PkgConfig QUIET)
set(CFITSIO_FOUND_SYSTEM FALSE)

if(PkgConfig_FOUND)
    pkg_check_modules(CFITSIO_PC QUIET cfitsio)
    if(CFITSIO_PC_FOUND)
        add_library(cfitsio_iface INTERFACE)
        target_include_directories(cfitsio_iface INTERFACE ${CFITSIO_PC_INCLUDE_DIRS})
        target_link_libraries(cfitsio_iface INTERFACE ${CFITSIO_PC_LIBRARIES})
        target_link_directories(cfitsio_iface INTERFACE ${CFITSIO_PC_LIBRARY_DIRS})
        add_library(cfitsio::cfitsio ALIAS cfitsio_iface)
        set(CFITSIO_FOUND_SYSTEM TRUE)
        message(STATUS "Found system cfitsio: ${CFITSIO_PC_VERSION}")
    endif()
endif()

if(NOT CFITSIO_FOUND_SYSTEM)
    message(STATUS "cfitsio not found on system, building from source...")
    FetchContent_Declare(
        cfitsio_src
        GIT_REPOSITORY https://github.com/HEASARC/cfitsio.git
        GIT_TAG        cfitsio-4-4-1
        GIT_SHALLOW    TRUE
    )
    set(USE_PTHREADS  OFF CACHE BOOL "" FORCE)
    set(TESTS         OFF CACHE BOOL "" FORCE)
    set(UTILS         OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(cfitsio_src)
    if(NOT TARGET cfitsio::cfitsio)
        add_library(cfitsio::cfitsio ALIAS cfitsio)
    endif()
endif()

# ─── QuaZip (ZIP support for image sets) — Phase 1 ────────────────────────
# Requires qt6-qt5compat-devel: dnf install qt6-qt5compat-devel
# Enabled automatically when Core5Compat is found
find_package(Qt6 OPTIONAL_COMPONENTS Core5Compat)
if(Qt6Core5Compat_FOUND)
    FetchContent_Declare(
        quazip
        GIT_REPOSITORY https://github.com/stachenov/quazip.git
        GIT_TAG        v1.4
        GIT_SHALLOW    TRUE
    )
    set(QUAZIP_QT_MAJOR_VERSION 6 CACHE STRING "" FORCE)
    FetchContent_MakeAvailable(quazip)
    message(STATUS "QuaZip enabled (ZIP image set support)")
else()
    message(STATUS "QuaZip disabled — install qt6-qt5compat-devel to enable ZIP support")
endif()

# ─── Catch2 (unit tests) ───────────────────────────────────────────────────
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.6.0
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(Catch2)
