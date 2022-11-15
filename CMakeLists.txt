cmake_minimum_required(VERSION 3.14)

project(untitled1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(QT NAMES Qt6 COMPONENTS Widgets Charts REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS StateMachine)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Charts REQUIRED)

add_executable(untitled1
  main.cpp
)
target_link_libraries(untitled1 Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(untitled1 Qt${QT_VERSION_MAJOR}::Charts)
target_link_libraries(untitled1 Qt${QT_VERSION_MAJOR}::Widgets)

