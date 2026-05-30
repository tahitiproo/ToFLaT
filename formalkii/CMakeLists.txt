cmake_minimum_required(VERSION 3.16)
project(ka_graph VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Включаем автоматическую обработку MOC-макросов Qt (обязательно!)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Ищем Qt6 или Qt5 (работает с обеими версиями)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

add_executable(ka_graph
    ka_graph.cpp
    mainwindow.h
    mainwindow.cpp
    visualwidget.h
    visualwidget.cpp
    types.h
)

target_link_libraries(ka_graph PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)