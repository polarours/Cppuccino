cmake_minimum_required(VERSION 3.16)

project(lru_cache LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_library(lru_cache_core INTERFACE)
target_include_directories(lru_cache_core INTERFACE include)
target_link_libraries(lru_cache_core INTERFACE Threads::Threads)

add_executable(lru_cache_example src/main.cpp)
target_link_libraries(lru_cache_example PRIVATE lru_cache_core Threads::Threads)

enable_testing()

if(BUILD_TESTING)
    add_executable(lru_cache_tests tests/lru_cache_tests.cpp)
    target_link_libraries(lru_cache_tests PRIVATE lru_cache_core Threads::Threads)
    add_test(NAME lru_cache_tests COMMAND lru_cache_tests)
endif()
