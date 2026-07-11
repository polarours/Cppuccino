cmake_minimum_required(VERSION 3.16)

project(iterator_pattern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(iterator_pattern_core INTERFACE)
target_include_directories(iterator_pattern_core INTERFACE include)

add_executable(iterator_pattern_example src/main.cpp)
target_link_libraries(iterator_pattern_example PRIVATE iterator_pattern_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(iterator_pattern_tests tests/iterator_pattern_tests.cpp)
    target_link_libraries(iterator_pattern_tests PRIVATE iterator_pattern_core)
    add_test(NAME iterator_pattern_tests COMMAND iterator_pattern_tests)
endif()
