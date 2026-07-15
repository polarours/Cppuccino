cmake_minimum_required(VERSION 3.16)

project(facade_pattern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(facade_pattern_core INTERFACE)
target_include_directories(facade_pattern_core INTERFACE include)

add_executable(facade_pattern_example src/main.cpp)
target_link_libraries(facade_pattern_example PRIVATE facade_pattern_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(facade_pattern_tests tests/facade_pattern_tests.cpp)
    target_link_libraries(facade_pattern_tests PRIVATE facade_pattern_core)
    add_test(NAME facade_pattern_tests COMMAND facade_pattern_tests)
endif()
