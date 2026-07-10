cmake_minimum_required(VERSION 3.16)

project(interpreter_pattern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(interpreter_pattern_core INTERFACE)
target_include_directories(interpreter_pattern_core INTERFACE include)

add_executable(interpreter_pattern_example src/main.cpp)
target_link_libraries(interpreter_pattern_example PRIVATE interpreter_pattern_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(interpreter_pattern_tests tests/interpreter_pattern_tests.cpp)
    target_link_libraries(interpreter_pattern_tests PRIVATE interpreter_pattern_core)
    add_test(NAME interpreter_pattern_tests COMMAND interpreter_pattern_tests)
endif()
