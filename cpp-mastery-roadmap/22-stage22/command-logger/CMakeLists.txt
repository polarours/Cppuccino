cmake_minimum_required(VERSION 3.16)

project(command_logger LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(command_logger_core INTERFACE)
target_include_directories(command_logger_core INTERFACE include)

add_executable(command_logger_example src/main.cpp)
target_link_libraries(command_logger_example PRIVATE command_logger_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(command_logger_tests tests/command_logger_tests.cpp)
    target_link_libraries(command_logger_tests PRIVATE command_logger_core)
    add_test(NAME command_logger_tests COMMAND command_logger_tests)
endif()
