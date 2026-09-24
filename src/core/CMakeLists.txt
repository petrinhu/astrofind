add_library(astrofind_core STATIC
    Astronomy.cpp
    Astronomy.h
    FitsImage.cpp
    ImageSession.cpp
    StarDetector.cpp
    AstrometryClient.cpp
    CatalogClient.cpp
    KooEngine.cpp
    MpcOrb.cpp
    MpcOrb.h
    Ephemeris.cpp
    Ephemeris.h
    Centroid.cpp
    Centroid.h
    Photometry.cpp
    Photometry.h
    Measurement.h
    AdesReport.cpp
    AdesReport.h
    ExportImage.cpp
    ExportImage.h
    Calibration.cpp
    Calibration.h
    ImageStacker.cpp
    ImageStacker.h
    MovingObjectDetector.cpp
    MovingObjectDetector.h
    MpcSubmit.cpp
    MpcSubmit.h
    GusProject.cpp
    GusProject.h
    AstapSolver.cpp
    AstapSolver.h
    HorizonsClient.cpp
    HorizonsClient.h
    NetworkSafety.cpp
    NetworkSafety.h
    ApiKeyStore.cpp
    ApiKeyStore.h
    SessionComparator.cpp
    SessionComparator.h
    ImageLoader.cpp
    ImageLoader.h
    FitsTableReader.cpp
    FitsTableReader.h
    Spectrum1D.cpp
    Spectrum1D.h
    PdsLoader.cpp
    PdsLoader.h
    ArchiveExtractor.cpp
    ArchiveExtractor.h
)

target_include_directories(astrofind_core
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/..
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(astrofind_core
    PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::Network
        Qt6::Concurrent
        Qt6::Sql
        Qt6::Xml
        cfitsio::cfitsio
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        sep::sep
        fftw3::fftw3
        CCfits::CCfits
)

if(ASTROFIND_HAS_LIBRAW)
    target_link_libraries(astrofind_core PUBLIC libraw::libraw)
    target_compile_definitions(astrofind_core PUBLIC ASTROFIND_HAS_LIBRAW)
endif()

# libarchive (TAR/7Z/RAR) lives in core since AUD-TEST-6 so the extraction
# code is covered by astrofind_tests (and its ASan/valgrind runs).
if(ASTROFIND_HAS_LIBARCHIVE)
    target_link_libraries(astrofind_core PUBLIC libarchive::libarchive)
    target_compile_definitions(astrofind_core PUBLIC ASTROFIND_HAS_LIBARCHIVE)
endif()

if(ASTROFIND_HAS_KEYCHAIN)
    target_link_libraries(astrofind_core PUBLIC Qt6Keychain::Qt6Keychain)
    target_compile_definitions(astrofind_core PUBLIC ASTROFIND_HAS_KEYCHAIN)
endif()

target_compile_features(astrofind_core PUBLIC cxx_std_23)

target_compile_options(astrofind_core PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
)
