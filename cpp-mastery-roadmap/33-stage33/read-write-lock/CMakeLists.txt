cmake_minimum_required(VERSION 3.16)

project(read_write_lock LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_library(read_write_lock_core INTERFACE)
target_include_directories(read_write_lock_core INTERFACE include)
target_link_libraries(read_write_lock_core INTERFACE Threads::Threads)

add_executable(read_write_lock_example src/main.cpp)
target_link_libraries(read_write_lock_example PRIVATE read_write_lock_core Threads::Threads)

enable_testing()

if(BUILD_TESTING)
    add_executable(read_write_lock_tests tests/read_write_lock_tests.cpp)
    target_link_libraries(read_write_lock_tests PRIVATE read_write_lock_core Threads::Threads)
    add_test(NAME read_write_lock_tests COMMAND read_write_lock_tests)
endif()
