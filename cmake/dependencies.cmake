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

# ─── SEP (Source Extractor C library) — Phase 2 ───────────────────────────
FetchContent_Declare(
    sep
    GIT_REPOSITORY https://github.com/kbarbary/sep.git
    GIT_TAG        v1.2.1
    GIT_SHALLOW    TRUE
)
FetchContent_GetProperties(sep)
if(NOT sep_POPULATED)
    FetchContent_Populate(sep)
    file(GLOB SEP_SRCS "${sep_SOURCE_DIR}/src/*.c")
    add_library(sep_lib STATIC ${SEP_SRCS})
    target_include_directories(sep_lib PUBLIC ${sep_SOURCE_DIR}/src)
    set_property(TARGET sep_lib PROPERTY C_STANDARD 99)
    # Suppress warnings from third-party C code
    target_compile_options(sep_lib PRIVATE -w)
    add_library(sep::sep ALIAS sep_lib)
    message(STATUS "SEP (Source Extractor) built from source")
endif()

# ─── Catch2 (unit tests) ───────────────────────────────────────────────────
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.6.0
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(Catch2)

# ─── FFTW3 (FFT for image alignment / stacking) — Sprint E ────────────────
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(FFTW3 REQUIRED fftw3)
    add_library(fftw3_iface INTERFACE)
    target_include_directories(fftw3_iface INTERFACE ${FFTW3_INCLUDE_DIRS})
    target_link_libraries(fftw3_iface INTERFACE ${FFTW3_LIBRARIES})
    add_library(fftw3::fftw3 ALIAS fftw3_iface)
    message(STATUS "Found fftw3: ${FFTW3_VERSION}")
endif()

# ─── CCfits 2.7 (bundled C++ wrapper around cfitsio) ──────────────────────
# Source: originals/CCfits.tar.gz (CCfits-2.7)
# We build inline, bypassing CCfits's own CMakeLists.txt, to reuse the
# cfitsio target already set up above (either system pkg-config or FetchContent).
if(NOT TARGET CCfits::CCfits)
    set(_ccfits_srcdir "${CMAKE_BINARY_DIR}/_deps/ccfits-src")
    if(NOT EXISTS "${_ccfits_srcdir}/CCfits-2.7/CCfits.h")
        message(STATUS "Extracting bundled CCfits 2.7 ...")
        file(ARCHIVE_EXTRACT
            INPUT       "${CMAKE_SOURCE_DIR}/originals/CCfits.tar.gz"
            DESTINATION "${_ccfits_srcdir}"
        )
    endif()
    set(_ccfits_dir "${_ccfits_srcdir}/CCfits-2.7")
    file(GLOB _ccfits_srcs "${_ccfits_dir}/*.cxx")
    # Exclude the cookbook demo program
    list(FILTER _ccfits_srcs EXCLUDE REGEX "cookbook\\.cxx$")
    add_library(ccfits_lib STATIC ${_ccfits_srcs})
    # Include dir = CCfits-2.7/ so that "CCfits.h", "FITS.h", etc. resolve correctly.
    # Consumer code uses: #include "CCfits.h"  (or through core/FitsTableReader.h)
    target_include_directories(ccfits_lib PUBLIC "${_ccfits_dir}")
    target_link_libraries(ccfits_lib PUBLIC cfitsio::cfitsio)
    target_compile_features(ccfits_lib PRIVATE cxx_std_11)
    # Suppress all warnings from third-party code
    target_compile_options(ccfits_lib PRIVATE -w)
    add_library(CCfits::CCfits ALIAS ccfits_lib)
    message(STATUS "CCfits 2.7 built from bundled source (${_ccfits_dir})")
endif()

# ─── Qt6Keychain (secure API key storage) — Task #13 ──────────────────────
# Requires: sudo dnf install qtkeychain-qt6-devel libsecret-devel
# Falls back to QSettings plain-text with a UI warning when not found.
set(ASTROFIND_HAS_KEYCHAIN FALSE)

find_package(Qt6Keychain QUIET)
if(Qt6Keychain_FOUND)
    set(ASTROFIND_HAS_KEYCHAIN TRUE)
    message(STATUS "Qt6Keychain found — secure API key storage enabled")
else()
    # Try pkg-config name used by some distros
    if(PkgConfig_FOUND)
        pkg_check_modules(QT6KEYCHAIN QUIET qt6keychain)
        if(QT6KEYCHAIN_FOUND)
            add_library(qt6keychain_iface INTERFACE)
            target_include_directories(qt6keychain_iface INTERFACE ${QT6KEYCHAIN_INCLUDE_DIRS})
            target_link_libraries(qt6keychain_iface INTERFACE ${QT6KEYCHAIN_LIBRARIES})
            add_library(Qt6Keychain::Qt6Keychain ALIAS qt6keychain_iface)
            set(ASTROFIND_HAS_KEYCHAIN TRUE)
            message(STATUS "Qt6Keychain (pkg-config) found — secure API key storage enabled")
        endif()
    endif()
endif()

if(NOT ASTROFIND_HAS_KEYCHAIN)
    message(STATUS
        "Qt6Keychain NOT found — API key stored in QSettings (plain-text).\n"
        "   To enable secure storage: sudo dnf install qtkeychain-qt6-devel libsecret-devel")
endif()

# ─── libarchive (TAR.GZ/BZ2/XZ, 7Z, RAR extraction) ──────────────────────────
# Requires: sudo dnf install libarchive-devel
set(ASTROFIND_HAS_LIBARCHIVE FALSE)

if(PkgConfig_FOUND)
    pkg_check_modules(LIBARCHIVE QUIET libarchive)
    if(LIBARCHIVE_FOUND)
        add_library(libarchive_iface INTERFACE)
        target_include_directories(libarchive_iface INTERFACE ${LIBARCHIVE_INCLUDE_DIRS})
        target_link_libraries(libarchive_iface INTERFACE ${LIBARCHIVE_LIBRARIES})
        add_library(libarchive::libarchive ALIAS libarchive_iface)
        set(ASTROFIND_HAS_LIBARCHIVE TRUE)
        message(STATUS "Found libarchive ${LIBARCHIVE_VERSION} — TAR/7Z/RAR extraction enabled")
    endif()
endif()

if(NOT ASTROFIND_HAS_LIBARCHIVE)
    find_library(LIBARCHIVE_LIB NAMES archive)
    find_path(LIBARCHIVE_INCLUDE NAMES archive.h)
    if(LIBARCHIVE_LIB AND LIBARCHIVE_INCLUDE)
        add_library(libarchive_iface INTERFACE)
        target_include_directories(libarchive_iface INTERFACE ${LIBARCHIVE_INCLUDE})
        target_link_libraries(libarchive_iface INTERFACE ${LIBARCHIVE_LIB})
        add_library(libarchive::libarchive ALIAS libarchive_iface)
        set(ASTROFIND_HAS_LIBARCHIVE TRUE)
        message(STATUS "Found libarchive (find_library) — TAR/7Z/RAR extraction enabled")
    endif()
endif()

if(NOT ASTROFIND_HAS_LIBARCHIVE)
    message(STATUS
        "libarchive NOT found — TAR.GZ/BZ2/XZ/7Z/RAR extraction disabled.\n"
        "   To enable: sudo dnf install libarchive-devel")
endif()
