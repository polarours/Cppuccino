cmake_minimum_required(VERSION 3.16)

project(concepts_pattern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(concepts_pattern_core INTERFACE)
target_include_directories(concepts_pattern_core INTERFACE include)

add_executable(concepts_pattern_example src/main.cpp)
target_link_libraries(concepts_pattern_example PRIVATE concepts_pattern_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(concepts_pattern_tests tests/concepts_pattern_tests.cpp)
    target_link_libraries(concepts_pattern_tests PRIVATE concepts_pattern_core)
    add_test(NAME concepts_pattern_tests COMMAND concepts_pattern_tests)
endif()
