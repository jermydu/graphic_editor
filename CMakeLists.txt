cmake_minimum_required(VERSION 3.16)
project(PhotoEditor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets PrintSupport)

qt_standard_project_setup()

set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/ImageCanvas.cpp
    src/AdjustmentPanel.cpp
    src/FilterPanel.cpp
    src/ToolOptionsPanel.cpp
    src/ImageProcessor.cpp
)

qt_add_executable(PhotoEditor ${SOURCES})

target_link_libraries(PhotoEditor PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::PrintSupport
)

if(WIN32)
    set_target_properties(PhotoEditor PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
