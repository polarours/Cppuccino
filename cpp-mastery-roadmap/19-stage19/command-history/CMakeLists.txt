cmake_minimum_required(VERSION 3.16)

project(command_history LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(command_history_core INTERFACE)
target_include_directories(command_history_core INTERFACE include)

add_executable(command_history_example src/main.cpp)
target_link_libraries(command_history_example PRIVATE command_history_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(command_history_tests tests/command_history_tests.cpp)
    target_link_libraries(command_history_tests PRIVATE command_history_core)
    add_test(NAME command_history_tests COMMAND command_history_tests)
endif()
