cmake_minimum_required(VERSION 3.16)

project(builder_pattern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(builder_pattern_core INTERFACE)
target_include_directories(builder_pattern_core INTERFACE include)

add_executable(builder_pattern_example src/main.cpp)
target_link_libraries(builder_pattern_example PRIVATE builder_pattern_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(builder_pattern_tests tests/builder_pattern_tests.cpp)
    target_link_libraries(builder_pattern_tests PRIVATE builder_pattern_core)
    add_test(NAME builder_pattern_tests COMMAND builder_pattern_tests)
endif()
