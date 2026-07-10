cmake_minimum_required(VERSION 3.16)

project(adapter_pattern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(adapter_pattern_core INTERFACE)
target_include_directories(adapter_pattern_core INTERFACE include)

add_executable(adapter_pattern_example src/main.cpp)
target_link_libraries(adapter_pattern_example PRIVATE adapter_pattern_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(adapter_pattern_tests tests/adapter_pattern_tests.cpp)
    target_link_libraries(adapter_pattern_tests PRIVATE adapter_pattern_core)
    add_test(NAME adapter_pattern_tests COMMAND adapter_pattern_tests)
endif()
