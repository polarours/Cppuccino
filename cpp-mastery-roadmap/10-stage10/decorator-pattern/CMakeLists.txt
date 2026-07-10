cmake_minimum_required(VERSION 3.16)

project(decorator_pattern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(decorator_pattern_core INTERFACE)
target_include_directories(decorator_pattern_core INTERFACE include)

add_executable(decorator_pattern_example src/main.cpp)
target_link_libraries(decorator_pattern_example PRIVATE decorator_pattern_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(decorator_pattern_tests tests/decorator_pattern_tests.cpp)
    target_link_libraries(decorator_pattern_tests PRIVATE decorator_pattern_core)
    add_test(NAME decorator_pattern_tests COMMAND decorator_pattern_tests)
endif()
