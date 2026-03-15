cmake_minimum_required(VERSION 3.25)

project(AstroFind
    VERSION 0.1.0
    DESCRIPTION "Modern asteroid detection software for citizen science"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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
)
find_package(Qt6 OPTIONAL_COMPONENTS LinguistTools)
qt_standard_project_setup()

# Dependencies
include(cmake/dependencies.cmake)

add_subdirectory(src)
add_subdirectory(tests)
