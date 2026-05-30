cmake_minimum_required(VERSION 3.16)
project(cpp_postman VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

add_executable(cpp_postman
    main.cpp
    MainWindow.h
    MainWindow.cpp
    GraphWidget.h
    GraphWidget.cpp
    types.h
)

target_link_libraries(cpp_postman PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
