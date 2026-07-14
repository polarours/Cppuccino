cmake_minimum_required(VERSION 3.16)

project(command_memento LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(command_memento_core INTERFACE)
target_include_directories(command_memento_core INTERFACE include)

add_executable(command_memento_example src/main.cpp)
target_link_libraries(command_memento_example PRIVATE command_memento_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(command_memento_tests tests/command_memento_tests.cpp)
    target_link_libraries(command_memento_tests PRIVATE command_memento_core)
    add_test(NAME command_memento_tests COMMAND command_memento_tests)
endif()
