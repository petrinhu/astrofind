add_library(astrofind_core STATIC
    FitsImage.cpp
    ImageSession.cpp
    StarDetector.cpp
    AstrometryClient.cpp
    CatalogClient.cpp
    KooEngine.cpp
)

target_include_directories(astrofind_core
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/..
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(astrofind_core
    PUBLIC
        Qt6::Core
        Qt6::Network
        Qt6::Sql
        cfitsio::cfitsio
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        sep::sep
)

target_compile_features(astrofind_core PUBLIC cxx_std_23)
