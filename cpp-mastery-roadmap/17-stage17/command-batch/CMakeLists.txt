cmake_minimum_required(VERSION 3.16)

project(command_batch LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(command_batch_core INTERFACE)
target_include_directories(command_batch_core INTERFACE include)

add_executable(command_batch_example src/main.cpp)
target_link_libraries(command_batch_example PRIVATE command_batch_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(command_batch_tests tests/command_batch_tests.cpp)
    target_link_libraries(command_batch_tests PRIVATE command_batch_core)
    add_test(NAME command_batch_tests COMMAND command_batch_tests)
endif()
