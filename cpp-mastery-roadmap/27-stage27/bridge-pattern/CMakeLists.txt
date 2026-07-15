cmake_minimum_required(VERSION 3.16)

project(bridge_pattern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(bridge_pattern_core INTERFACE)
target_include_directories(bridge_pattern_core INTERFACE include)

add_executable(bridge_pattern_example src/main.cpp)
target_link_libraries(bridge_pattern_example PRIVATE bridge_pattern_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(bridge_pattern_tests tests/bridge_pattern_tests.cpp)
    target_link_libraries(bridge_pattern_tests PRIVATE bridge_pattern_core)
    add_test(NAME bridge_pattern_tests COMMAND bridge_pattern_tests)
endif()
