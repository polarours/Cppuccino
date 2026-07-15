cmake_minimum_required(VERSION 3.16)

project(mvc_pattern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(mvc_pattern_core INTERFACE)
target_include_directories(mvc_pattern_core INTERFACE include)

add_executable(mvc_pattern_example src/main.cpp)
target_link_libraries(mvc_pattern_example PRIVATE mvc_pattern_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(mvc_pattern_tests tests/mvc_pattern_tests.cpp)
    target_link_libraries(mvc_pattern_tests PRIVATE mvc_pattern_core)
    add_test(NAME mvc_pattern_tests COMMAND mvc_pattern_tests)
endif()
