cmake_minimum_required(VERSION 3.16)

project(ring_buffer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_library(ring_buffer_core INTERFACE)
target_include_directories(ring_buffer_core INTERFACE include)
target_link_libraries(ring_buffer_core INTERFACE Threads::Threads)

add_executable(ring_buffer_example src/main.cpp)
target_link_libraries(ring_buffer_example PRIVATE ring_buffer_core Threads::Threads)

enable_testing()

if(BUILD_TESTING)
    add_executable(ring_buffer_tests tests/ring_buffer_tests.cpp)
    target_link_libraries(ring_buffer_tests PRIVATE ring_buffer_core Threads::Threads)
    add_test(NAME ring_buffer_tests COMMAND ring_buffer_tests)
endif()
