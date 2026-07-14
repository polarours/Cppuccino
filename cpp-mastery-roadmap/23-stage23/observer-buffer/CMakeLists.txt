cmake_minimum_required(VERSION 3.16)

project(observer_buffer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_library(observer_buffer_core INTERFACE)
target_include_directories(observer_buffer_core INTERFACE include)
target_link_libraries(observer_buffer_core INTERFACE Threads::Threads)

add_executable(observer_buffer_example src/main.cpp)
target_link_libraries(observer_buffer_example PRIVATE observer_buffer_core Threads::Threads)

enable_testing()

if(BUILD_TESTING)
    add_executable(observer_buffer_tests tests/observer_buffer_tests.cpp)
    target_link_libraries(observer_buffer_tests PRIVATE observer_buffer_core Threads::Threads)
    add_test(NAME observer_buffer_tests COMMAND observer_buffer_tests)
endif()
