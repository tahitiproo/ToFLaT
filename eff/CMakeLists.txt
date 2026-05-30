cmake_minimum_required(VERSION 3.16)
project(app_eff VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

add_executable(app_eff
    main.cpp
    MainWindow.h
    MainWindow.cpp
    GrammarCore.h
)

target_link_libraries(app_eff PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
