cmake_minimum_required(VERSION 3.16)

project(memory_alignment LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(memory_alignment_core INTERFACE)
target_include_directories(memory_alignment_core INTERFACE include)

add_executable(memory_alignment_example src/main.cpp)
target_link_libraries(memory_alignment_example PRIVATE memory_alignment_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(memory_alignment_tests tests/memory_alignment_tests.cpp)
    target_link_libraries(memory_alignment_tests PRIVATE memory_alignment_core)
    add_test(NAME memory_alignment_tests COMMAND memory_alignment_tests)
endif()
