cmake_minimum_required(VERSION 4.1)
project(SQM_Calculator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt@5")

find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(SQM_Calculator main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        SqmModel.cpp
        SqmModel.h
)
target_link_libraries(SQM_Calculator
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
)

