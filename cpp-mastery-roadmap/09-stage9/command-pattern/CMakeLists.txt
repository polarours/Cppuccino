cmake_minimum_required(VERSION 3.16)

project(command_pattern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(command_pattern_core INTERFACE)
target_include_directories(command_pattern_core INTERFACE include)

add_executable(command_pattern_example src/main.cpp)
target_link_libraries(command_pattern_example PRIVATE command_pattern_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(command_pattern_tests tests/command_pattern_tests.cpp)
    target_link_libraries(command_pattern_tests PRIVATE command_pattern_core)
    add_test(NAME command_pattern_tests COMMAND command_pattern_tests)
endif()
