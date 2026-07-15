cmake_minimum_required(VERSION 3.16)

project(specification_pattern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(specification_pattern_core INTERFACE)
target_include_directories(specification_pattern_core INTERFACE include)

add_executable(specification_pattern_example src/main.cpp)
target_link_libraries(specification_pattern_example PRIVATE specification_pattern_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(specification_pattern_tests tests/specification_pattern_tests.cpp)
    target_link_libraries(specification_pattern_tests PRIVATE specification_pattern_core)
    add_test(NAME specification_pattern_tests COMMAND specification_pattern_tests)
endif()
