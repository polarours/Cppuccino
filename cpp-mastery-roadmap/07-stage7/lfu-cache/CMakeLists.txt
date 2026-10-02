cmake_minimum_required(VERSION 3.16)

project(lfu_cache LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(lfu_cache_core INTERFACE)
target_include_directories(lfu_cache_core INTERFACE include)

add_executable(lfu_cache_example src/main.cpp)
target_link_libraries(lfu_cache_example PRIVATE lfu_cache_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(lfu_cache_tests tests/lfu_cache_tests.cpp)
    target_link_libraries(lfu_cache_tests PRIVATE lfu_cache_core)
    add_test(NAME lfu_cache_tests COMMAND lfu_cache_tests)
endif()
