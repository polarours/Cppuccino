cmake_minimum_required(VERSION 3.16)

project(command_validator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(command_validator_core INTERFACE)
target_include_directories(command_validator_core INTERFACE include)

add_executable(command_validator_example src/main.cpp)
target_link_libraries(command_validator_example PRIVATE command_validator_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(command_validator_tests tests/command_validator_tests.cpp)
    target_link_libraries(command_validator_tests PRIVATE command_validator_core)
    add_test(NAME command_validator_tests COMMAND command_validator_tests)
endif()
