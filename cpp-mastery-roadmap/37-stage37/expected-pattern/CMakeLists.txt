cmake_minimum_required(VERSION 3.16)

project(expected_pattern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(expected_pattern_core INTERFACE)
target_include_directories(expected_pattern_core INTERFACE include)

add_executable(expected_pattern_example src/main.cpp)
target_link_libraries(expected_pattern_example PRIVATE expected_pattern_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(expected_pattern_tests tests/expected_pattern_tests.cpp)
    target_link_libraries(expected_pattern_tests PRIVATE expected_pattern_core)
    add_test(NAME expected_pattern_tests COMMAND expected_pattern_tests)
endif()
