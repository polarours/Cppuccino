cmake_minimum_required(VERSION 3.16)

project(result_type LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(result_type_core INTERFACE)
target_include_directories(result_type_core INTERFACE include)

add_executable(result_type_example src/main.cpp)
target_link_libraries(result_type_example PRIVATE result_type_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(result_type_tests tests/result_type_tests.cpp)
    target_link_libraries(result_type_tests PRIVATE result_type_core)
    add_test(NAME result_type_tests COMMAND result_type_tests)
endif()
