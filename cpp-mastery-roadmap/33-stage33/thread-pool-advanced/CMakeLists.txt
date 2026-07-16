cmake_minimum_required(VERSION 3.16)

project(thread_pool_advanced LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_library(thread_pool_advanced_core INTERFACE)
target_include_directories(thread_pool_advanced_core INTERFACE include)
target_link_libraries(thread_pool_advanced_core INTERFACE Threads::Threads)

add_executable(thread_pool_advanced_example src/main.cpp)
target_link_libraries(thread_pool_advanced_example PRIVATE thread_pool_advanced_core Threads::Threads)

enable_testing()

if(BUILD_TESTING)
    add_executable(thread_pool_advanced_tests tests/thread_pool_advanced_tests.cpp)
    target_link_libraries(thread_pool_advanced_tests PRIVATE thread_pool_advanced_core Threads::Threads)
    add_test(NAME thread_pool_advanced_tests COMMAND thread_pool_advanced_tests)
endif()
