cmake_minimum_required(VERSION 3.16)

project(std_print LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(std_print_core INTERFACE)
target_include_directories(std_print_core INTERFACE include)

add_executable(std_print_example src/main.cpp)
target_link_libraries(std_print_example PRIVATE std_print_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(std_print_tests tests/std_print_tests.cpp)
    target_link_libraries(std_print_tests PRIVATE std_print_core)
    add_test(NAME std_print_tests COMMAND std_print_tests)
endif()
