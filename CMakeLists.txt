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

find_package(Qt6 6.4 REQUIRED COMPONENTS Core Widgets)

function(odw_enable_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive-)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

add_library(odw_core STATIC
    src/core/workspace/Layout.cpp
    src/core/workspace/Layout.hpp
)

target_include_directories(odw_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(odw_core PUBLIC cxx_std_20)
odw_enable_warnings(odw_core)

add_executable(odw
    src/app/main.cpp
    src/ui/MainWindow.cpp
    src/ui/MainWindow.hpp
    src/ui/theme/VisualTheme.cpp
    src/ui/theme/VisualTheme.hpp
)

target_link_libraries(odw PRIVATE
    odw_core
    Qt6::Core
    Qt6::Widgets
)

target_compile_features(odw PRIVATE cxx_std_20)
odw_enable_warnings(odw)

include(CTest)

if(BUILD_TESTING)
    add_executable(odw_core_tests
        tests/workspace_layout_tests.cpp
    )
    target_link_libraries(odw_core_tests PRIVATE odw_core)
    odw_enable_warnings(odw_core_tests)

    add_test(NAME workspace_layout COMMAND odw_core_tests)
endif()
