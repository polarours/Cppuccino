cmake_minimum_required(VERSION 3.16)

project(varargs_pattern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(varargs_pattern_core INTERFACE)
target_include_directories(varargs_pattern_core INTERFACE include)

add_executable(varargs_pattern_example src/main.cpp)
target_link_libraries(varargs_pattern_example PRIVATE varargs_pattern_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(varargs_pattern_tests tests/varargs_pattern_tests.cpp)
    target_link_libraries(varargs_pattern_tests PRIVATE varargs_pattern_core)
    add_test(NAME varargs_pattern_tests COMMAND varargs_pattern_tests)
endif()
