cmake_minimum_required(VERSION 3.16)

project(thread_pool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_library(thread_pool_core src/thread_pool.cpp)
target_include_directories(thread_pool_core PUBLIC include)
target_link_libraries(thread_pool_core PUBLIC Threads::Threads)

add_executable(thread_pool_example src/main.cpp)
target_link_libraries(thread_pool_example PRIVATE thread_pool_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(thread_pool_tests tests/thread_pool_tests.cpp)
    target_link_libraries(thread_pool_tests PRIVATE thread_pool_core)
    add_test(NAME thread_pool_tests COMMAND thread_pool_tests)
endif()
