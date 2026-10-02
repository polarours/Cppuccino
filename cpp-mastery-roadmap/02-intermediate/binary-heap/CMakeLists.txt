cmake_minimum_required(VERSION 3.16)

project(binary_heap LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(binary_heap_core INTERFACE)
target_include_directories(binary_heap_core INTERFACE include)

add_executable(binary_heap_example src/main.cpp)
target_link_libraries(binary_heap_example PRIVATE binary_heap_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(binary_heap_tests tests/binary_heap_tests.cpp)
    target_link_libraries(binary_heap_tests PRIVATE binary_heap_core)
    add_test(NAME binary_heap_tests COMMAND binary_heap_tests)
endif()
