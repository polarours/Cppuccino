cmake_minimum_required(VERSION 3.16)

project(double_buffer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_library(double_buffer_core INTERFACE)
target_include_directories(double_buffer_core INTERFACE include)
target_link_libraries(double_buffer_core INTERFACE Threads::Threads)

add_executable(double_buffer_example src/main.cpp)
target_link_libraries(double_buffer_example PRIVATE double_buffer_core Threads::Threads)

enable_testing()

if(BUILD_TESTING)
    add_executable(double_buffer_tests tests/double_buffer_tests.cpp)
    target_link_libraries(double_buffer_tests PRIVATE double_buffer_core Threads::Threads)
    add_test(NAME double_buffer_tests COMMAND double_buffer_tests)
endif()
