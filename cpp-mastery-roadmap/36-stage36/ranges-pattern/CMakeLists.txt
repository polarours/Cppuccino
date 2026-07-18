cmake_minimum_required(VERSION 3.16)

project(ranges_pattern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(ranges_pattern_core INTERFACE)
target_include_directories(ranges_pattern_core INTERFACE include)

add_executable(ranges_pattern_example src/main.cpp)
target_link_libraries(ranges_pattern_example PRIVATE ranges_pattern_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(ranges_pattern_tests tests/ranges_pattern_tests.cpp)
    target_link_libraries(ranges_pattern_tests PRIVATE ranges_pattern_core)
    add_test(NAME ranges_pattern_tests COMMAND ranges_pattern_tests)
endif()
