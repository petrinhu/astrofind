add_executable(astrofind_tests
    test_fits_loader.cpp
    test_image_session.cpp
    test_star_detector.cpp
    test_calibration.cpp
    test_centroid.cpp
    test_image_stacker.cpp
    test_moving_object_detector.cpp
    test_mpc_orb.cpp
    test_ades_report.cpp
    test_fits_functional.cpp
    test_astronomy.cpp
    test_photometry.cpp
    test_robustness.cpp
)

target_link_libraries(astrofind_tests PRIVATE
    astrofind_core
    Catch2::Catch2WithMain
)

target_include_directories(astrofind_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

include(CTest)
add_test(NAME astrofind_tests COMMAND astrofind_tests)

# ─── UI tests (require QApplication) ─────────────────────────────────────────
add_executable(astrofind_ui_tests
    test_ui_main.cpp
    test_settings_dialog.cpp
    test_calibration_panel.cpp
    test_session_signals.cpp
)

target_link_libraries(astrofind_ui_tests PRIVATE
    astrofind_ui
    astrofind_core
    Catch2::Catch2
    Qt6::Widgets
)

target_include_directories(astrofind_ui_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

add_test(NAME astrofind_ui_tests
    COMMAND astrofind_ui_tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
