cmake_minimum_required(VERSION 3.16)
project(ka_graph) # Название вашего проекта

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "C:/Qt/6.5.3/mingw_64") 

# Находим нужные модули Qt (в вашем случае нужен Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Настройки для Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Создаем программу из ka_graph.cpp
add_executable(ka_graph ka_graph.cpp)

# Подключаем библиотеки Qt к нашей программе
target_link_libraries(ka_graph PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)