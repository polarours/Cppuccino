cmake_minimum_required(VERSION 3.16)

project(observer_pattern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(observer_pattern_core INTERFACE)
target_include_directories(observer_pattern_core INTERFACE include)

add_executable(observer_pattern_example src/main.cpp)
target_link_libraries(observer_pattern_example PRIVATE observer_pattern_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(observer_pattern_tests tests/observer_pattern_tests.cpp)
    target_link_libraries(observer_pattern_tests PRIVATE observer_pattern_core)
    add_test(NAME observer_pattern_tests COMMAND observer_pattern_tests)
endif()
