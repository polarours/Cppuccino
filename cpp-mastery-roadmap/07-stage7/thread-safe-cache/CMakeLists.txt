cmake_minimum_required(VERSION 3.16)

project(thread_safe_cache LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_library(thread_safe_cache_core INTERFACE)
target_include_directories(thread_safe_cache_core INTERFACE include)
target_link_libraries(thread_safe_cache_core INTERFACE Threads::Threads)

add_executable(thread_safe_cache_example src/main.cpp)
target_link_libraries(thread_safe_cache_example PRIVATE thread_safe_cache_core Threads::Threads)

enable_testing()

if(BUILD_TESTING)
    add_executable(thread_safe_cache_tests tests/thread_safe_cache_tests.cpp)
    target_link_libraries(thread_safe_cache_tests PRIVATE thread_safe_cache_core Threads::Threads)
    add_test(NAME thread_safe_cache_tests COMMAND thread_safe_cache_tests)
endif()
