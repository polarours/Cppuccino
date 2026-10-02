cmake_minimum_required(VERSION 3.16)

project(bloom_filter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(bloom_filter_core src/bloom_filter.cpp)
target_include_directories(bloom_filter_core PUBLIC include)

add_executable(bloom_filter_example src/main.cpp)
target_link_libraries(bloom_filter_example PRIVATE bloom_filter_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(bloom_filter_tests tests/bloom_filter_tests.cpp)
    target_link_libraries(bloom_filter_tests PRIVATE bloom_filter_core)
    add_test(NAME bloom_filter_tests COMMAND bloom_filter_tests)
endif()
