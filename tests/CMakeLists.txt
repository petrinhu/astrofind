enable_testing()

add_executable(astrofind_tests
    test_fits_loader.cpp
    test_image_session.cpp
    test_star_detector.cpp
)

target_link_libraries(astrofind_tests PRIVATE
    astrofind_core
    Catch2::Catch2WithMain
)

target_include_directories(astrofind_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

include(CTest)
include(Catch)
catch_discover_tests(astrofind_tests)
