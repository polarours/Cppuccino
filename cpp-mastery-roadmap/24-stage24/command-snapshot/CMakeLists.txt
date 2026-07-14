cmake_minimum_required(VERSION 3.16)

project(command_snapshot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(command_snapshot_core INTERFACE)
target_include_directories(command_snapshot_core INTERFACE include)

add_executable(command_snapshot_example src/main.cpp)
target_link_libraries(command_snapshot_example PRIVATE command_snapshot_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(command_snapshot_tests tests/command_snapshot_tests.cpp)
    target_link_libraries(command_snapshot_tests PRIVATE command_snapshot_core)
    add_test(NAME command_snapshot_tests COMMAND command_snapshot_tests)
endif()
