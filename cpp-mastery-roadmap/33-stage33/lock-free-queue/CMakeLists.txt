cmake_minimum_required(VERSION 3.16)

project(lock_free_queue LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_library(lock_free_queue_core INTERFACE)
target_include_directories(lock_free_queue_core INTERFACE include)
target_link_libraries(lock_free_queue_core INTERFACE Threads::Threads)

add_executable(lock_free_queue_example src/main.cpp)
target_link_libraries(lock_free_queue_example PRIVATE lock_free_queue_core Threads::Threads)

enable_testing()

if(BUILD_TESTING)
    add_executable(lock_free_queue_tests tests/lock_free_queue_tests.cpp)
    target_link_libraries(lock_free_queue_tests PRIVATE lock_free_queue_core Threads::Threads)
    add_test(NAME lock_free_queue_tests COMMAND lock_free_queue_tests)
endif()
