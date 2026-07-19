cmake_minimum_required(VERSION 3.16)

project(cmake_pattern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(cmake_pattern_core INTERFACE)
target_include_directories(cmake_pattern_core INTERFACE include)

add_executable(cmake_pattern_example src/main.cpp)
target_link_libraries(cmake_pattern_example PRIVATE cmake_pattern_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(cmake_pattern_tests tests/cmake_pattern_tests.cpp)
    target_link_libraries(cmake_pattern_tests PRIVATE cmake_pattern_core)
    add_test(NAME cmake_pattern_tests COMMAND cmake_pattern_tests)
endif()
