cmake_minimum_required(VERSION 3.16)

project(error_code LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(error_code_core INTERFACE)
target_include_directories(error_code_core INTERFACE include)

add_executable(error_code_example src/main.cpp)
target_link_libraries(error_code_example PRIVATE error_code_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(error_code_tests tests/error_code_tests.cpp)
    target_link_libraries(error_code_tests PRIVATE error_code_core)
    add_test(NAME error_code_tests COMMAND error_code_tests)
endif()
