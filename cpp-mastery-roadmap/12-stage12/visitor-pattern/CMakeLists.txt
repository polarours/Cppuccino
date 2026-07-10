cmake_minimum_required(VERSION 3.16)

project(visitor_pattern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(visitor_pattern_core INTERFACE)
target_include_directories(visitor_pattern_core INTERFACE include)

add_executable(visitor_pattern_example src/main.cpp)
target_link_libraries(visitor_pattern_example PRIVATE visitor_pattern_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(visitor_pattern_tests tests/visitor_pattern_tests.cpp)
    target_link_libraries(visitor_pattern_tests PRIVATE visitor_pattern_core)
    add_test(NAME visitor_pattern_tests COMMAND visitor_pattern_tests)
endif()
