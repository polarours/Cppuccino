cmake_minimum_required(VERSION 3.16)

project(cqrs_pattern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(cqrs_pattern_core INTERFACE)
target_include_directories(cqrs_pattern_core INTERFACE include)

add_executable(cqrs_pattern_example src/main.cpp)
target_link_libraries(cqrs_pattern_example PRIVATE cqrs_pattern_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(cqrs_pattern_tests tests/cqrs_pattern_tests.cpp)
    target_link_libraries(cqrs_pattern_tests PRIVATE cqrs_pattern_core)
    add_test(NAME cqrs_pattern_tests COMMAND cqrs_pattern_tests)
endif()
