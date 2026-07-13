cmake_minimum_required(VERSION 3.16)

project(command_queue LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_library(command_queue_core INTERFACE)
target_include_directories(command_queue_core INTERFACE include)
target_link_libraries(command_queue_core INTERFACE Threads::Threads)

add_executable(command_queue_example src/main.cpp)
target_link_libraries(command_queue_example PRIVATE command_queue_core Threads::Threads)

enable_testing()

if(BUILD_TESTING)
    add_executable(command_queue_tests tests/command_queue_tests.cpp)
    target_link_libraries(command_queue_tests PRIVATE command_queue_core Threads::Threads)
    add_test(NAME command_queue_tests COMMAND command_queue_tests)
endif()
