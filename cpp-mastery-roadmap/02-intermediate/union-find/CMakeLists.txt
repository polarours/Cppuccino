cmake_minimum_required(VERSION 3.16)

project(union_find LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(union_find_core INTERFACE)
target_include_directories(union_find_core INTERFACE include)

add_executable(union_find_example src/main.cpp)
target_link_libraries(union_find_example PRIVATE union_find_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(union_find_tests tests/union_find_tests.cpp)
    target_link_libraries(union_find_tests PRIVATE union_find_core)
    add_test(NAME union_find_tests COMMAND union_find_tests)
endif()
