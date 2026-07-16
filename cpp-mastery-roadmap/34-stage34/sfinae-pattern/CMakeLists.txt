cmake_minimum_required(VERSION 3.16)

project(sfinae_pattern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(sfinae_pattern_core INTERFACE)
target_include_directories(sfinae_pattern_core INTERFACE include)

add_executable(sfinae_pattern_example src/main.cpp)
target_link_libraries(sfinae_pattern_example PRIVATE sfinae_pattern_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(sfinae_pattern_tests tests/sfinae_pattern_tests.cpp)
    target_link_libraries(sfinae_pattern_tests PRIVATE sfinae_pattern_core)
    add_test(NAME sfinae_pattern_tests COMMAND sfinae_pattern_tests)
endif()
