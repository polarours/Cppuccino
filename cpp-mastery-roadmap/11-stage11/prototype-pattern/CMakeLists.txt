cmake_minimum_required(VERSION 3.16)

project(prototype_pattern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(prototype_pattern_core INTERFACE)
target_include_directories(prototype_pattern_core INTERFACE include)

add_executable(prototype_pattern_example src/main.cpp)
target_link_libraries(prototype_pattern_example PRIVATE prototype_pattern_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(prototype_pattern_tests tests/prototype_pattern_tests.cpp)
    target_link_libraries(prototype_pattern_tests PRIVATE prototype_pattern_core)
    add_test(NAME prototype_pattern_tests COMMAND prototype_pattern_tests)
endif()
