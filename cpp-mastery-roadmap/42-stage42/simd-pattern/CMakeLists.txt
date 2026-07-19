cmake_minimum_required(VERSION 3.16)

project(simd_pattern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(simd_pattern_core INTERFACE)
target_include_directories(simd_pattern_core INTERFACE include)

add_executable(simd_pattern_example src/main.cpp)
target_link_libraries(simd_pattern_example PRIVATE simd_pattern_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(simd_pattern_tests tests/simd_pattern_tests.cpp)
    target_link_libraries(simd_pattern_tests PRIVATE simd_pattern_core)
    add_test(NAME simd_pattern_tests COMMAND simd_pattern_tests)
endif()
