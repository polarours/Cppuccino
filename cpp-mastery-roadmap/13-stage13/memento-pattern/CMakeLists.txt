cmake_minimum_required(VERSION 3.16)

project(memento_pattern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(memento_pattern_core INTERFACE)
target_include_directories(memento_pattern_core INTERFACE include)

add_executable(memento_pattern_example src/main.cpp)
target_link_libraries(memento_pattern_example PRIVATE memento_pattern_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(memento_pattern_tests tests/memento_pattern_tests.cpp)
    target_link_libraries(memento_pattern_tests PRIVATE memento_pattern_core)
    add_test(NAME memento_pattern_tests COMMAND memento_pattern_tests)
endif()
