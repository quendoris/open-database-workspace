cmake_minimum_required(VERSION 3.24)

project(OpenDatabaseWorkspace
    VERSION 0.0.0
    DESCRIPTION "Open Database Workspace"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

add_executable(odw
    src/app/main.cpp
    src/ui/MainWindow.cpp
    src/ui/MainWindow.hpp
)

target_link_libraries(odw PRIVATE
    Qt6::Core
    Qt6::Widgets
)

target_compile_features(odw PRIVATE cxx_std_20)

if(MSVC)
    target_compile_options(odw PRIVATE /W4 /permissive-)
else()
    target_compile_options(odw PRIVATE -Wall -Wextra -Wpedantic)
endif()

include(CTest)
