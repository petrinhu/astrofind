set(UI_SOURCES
    MainWindow.cpp
    MainWindow_settings.cpp
    MainWindow_io.cpp
    MainWindow_reduction.cpp
    MainWindow_measurement.cpp
    FitsImageView.cpp
    FitsSubWindow.cpp
    WorkflowPanel.cpp
    AboutDialog.cpp
    LogPanel.cpp
    BlinkWidget.cpp
    ThumbnailBar.cpp
    BackgroundRangeDialog.cpp
    AppIcons.cpp
    Theme.cpp
    SettingsDialog.cpp
    MeasurementTable.cpp
    ReportPreviewDialog.cpp
    SetupWizard.cpp
    ImageCatalogTable.cpp
    CalibrationPanel.cpp
    VerificationDialog.cpp
    SelectMarkingsDialog.cpp
    EditImageParametersDialog.cpp
    HelpDialog.cpp
    CalibrationWizard.cpp
    SessionInfoBar.cpp
    HorizonsQueryDialog.cpp
    LightCurveDialog.cpp
    SessionCompareDialog.cpp
    MasterFrameDialog.cpp
    GrowthCurveDialog.cpp
    Toast.cpp
    DockTitleBar.cpp
    RegionStatsPanel.cpp
    HistogramDialog.cpp
    PowerSpectrumDialog.cpp
    SpectrumDialog.cpp
)

set(UI_HEADERS
    MainWindow.h
    MainWindow_p.h
    FitsImageView.h
    FitsSubWindow.h
    WorkflowPanel.h
    AboutDialog.h
    LogPanel.h
    BlinkWidget.h
    ThumbnailBar.h
    BackgroundRangeDialog.h
    Theme.h
    SettingsDialog.h
    MeasurementTable.h
    ReportPreviewDialog.h
    SetupWizard.h
    ImageCatalogTable.h
    CalibrationPanel.h
    VerificationDialog.h
    SelectMarkingsDialog.h
    EditImageParametersDialog.h
    HelpDialog.h
    CalibrationWizard.h
    SessionInfoBar.h
    HorizonsQueryDialog.h
    LightCurveDialog.h
    SessionCompareDialog.h
    MasterFrameDialog.h
    GrowthCurveDialog.h
    Toast.h
    DockTitleBar.h
    RegionStatsPanel.h
    HistogramDialog.h
    PowerSpectrumDialog.h
    SpectrumDialog.h
)

add_library(astrofind_ui STATIC
    ${UI_SOURCES}
    ${UI_HEADERS}
)

target_include_directories(astrofind_ui
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/..
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(astrofind_ui
    PUBLIC
        astrofind_core
        Qt6::Core
        Qt6::Widgets
        Qt6::Concurrent
        Qt6::OpenGL
        Qt6::OpenGLWidgets
        Qt6::Charts
        spdlog::spdlog
)

if(ASTROFIND_HAS_LIBARCHIVE)
    target_link_libraries(astrofind_ui PUBLIC libarchive::libarchive)
    target_compile_definitions(astrofind_ui PUBLIC ASTROFIND_HAS_LIBARCHIVE)
endif()

target_compile_options(astrofind_ui PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
)
