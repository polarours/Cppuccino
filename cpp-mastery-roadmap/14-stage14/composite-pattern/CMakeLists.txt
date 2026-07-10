cmake_minimum_required(VERSION 3.16)

project(composite_pattern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(composite_pattern_core INTERFACE)
target_include_directories(composite_pattern_core INTERFACE include)

add_executable(composite_pattern_example src/main.cpp)
target_link_libraries(composite_pattern_example PRIVATE composite_pattern_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(composite_pattern_tests tests/composite_pattern_tests.cpp)
    target_link_libraries(composite_pattern_tests PRIVATE composite_pattern_core)
    add_test(NAME composite_pattern_tests COMMAND composite_pattern_tests)
endif()
