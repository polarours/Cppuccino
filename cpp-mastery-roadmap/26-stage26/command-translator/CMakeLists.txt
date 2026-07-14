cmake_minimum_required(VERSION 3.16)

project(command_translator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(command_translator_core INTERFACE)
target_include_directories(command_translator_core INTERFACE include)

add_executable(command_translator_example src/main.cpp)
target_link_libraries(command_translator_example PRIVATE command_translator_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(command_translator_tests tests/command_translator_tests.cpp)
    target_link_libraries(command_translator_tests PRIVATE command_translator_core)
    add_test(NAME command_translator_tests COMMAND command_translator_tests)
endif()
