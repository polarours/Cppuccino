cmake_minimum_required(VERSION 3.16)

project(null_object_pattern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(null_object_pattern_core INTERFACE)
target_include_directories(null_object_pattern_core INTERFACE include)

add_executable(null_object_pattern_example src/main.cpp)
target_link_libraries(null_object_pattern_example PRIVATE null_object_pattern_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(null_object_pattern_tests tests/null_object_pattern_tests.cpp)
    target_link_libraries(null_object_pattern_tests PRIVATE null_object_pattern_core)
    add_test(NAME null_object_pattern_tests COMMAND null_object_pattern_tests)
endif()
