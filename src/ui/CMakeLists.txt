set(UI_SOURCES
    MainWindow.cpp
    FitsImageView.cpp
    FitsSubWindow.cpp
    WorkflowPanel.cpp
    AboutDialog.cpp
    LogPanel.cpp
)

set(UI_HEADERS
    MainWindow.h
    FitsImageView.h
    FitsSubWindow.h
    WorkflowPanel.h
    AboutDialog.h
    LogPanel.h
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
        Qt6::OpenGL
        Qt6::OpenGLWidgets
        spdlog::spdlog
)
