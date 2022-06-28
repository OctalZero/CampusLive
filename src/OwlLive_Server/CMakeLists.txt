cmake_minimum_required(VERSION 3.21)

project(OwlLive_Server LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)


add_executable(OwlLive_Server
  main.cpp
  User.cpp
  User.h
  Server.cpp
  Server.h
  Database.cpp
  Database.h
  Types.h
)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)
target_link_libraries(OwlLive_Server Qt${QT_VERSION_MAJOR}::Core)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Network REQUIRED)
target_link_libraries(OwlLive_Server Qt${QT_VERSION_MAJOR}::Network)


find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Sql REQUIRED)
target_link_libraries(OwlLive_Server Qt${QT_VERSION_MAJOR}::Sql)

