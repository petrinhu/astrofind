add_library(astrofind_core STATIC
    FitsImage.cpp
    ImageSession.cpp
)

target_include_directories(astrofind_core
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/..
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(astrofind_core
    PUBLIC
        Qt6::Core
        cfitsio::cfitsio
        spdlog::spdlog
        nlohmann_json::nlohmann_json
)

target_compile_features(astrofind_core PUBLIC cxx_std_23)
