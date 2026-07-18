cmake_minimum_required(VERSION 3.16)

project(coroutine_pattern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(coroutine_pattern_core INTERFACE)
target_include_directories(coroutine_pattern_core INTERFACE include)

add_executable(coroutine_pattern_example src/main.cpp)
target_link_libraries(coroutine_pattern_example PRIVATE coroutine_pattern_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(coroutine_pattern_tests tests/coroutine_pattern_tests.cpp)
    target_link_libraries(coroutine_pattern_tests PRIVATE coroutine_pattern_core)
    add_test(NAME coroutine_pattern_tests COMMAND coroutine_pattern_tests)
endif()
