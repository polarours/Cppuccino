cmake_minimum_required(VERSION 3.16)

project(parameterized_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(parameterized_tests_core INTERFACE)
target_include_directories(parameterized_tests_core INTERFACE include)

add_executable(parameterized_tests_example src/main.cpp)
target_link_libraries(parameterized_tests_example PRIVATE parameterized_tests_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(parameterized_tests_tests tests/parameterized_tests_tests.cpp)
    target_link_libraries(parameterized_tests_tests PRIVATE parameterized_tests_core)
    add_test(NAME parameterized_tests_tests COMMAND parameterized_tests_tests)
endif()
