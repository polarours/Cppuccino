cmake_minimum_required(VERSION 3.16)

project(repository_pattern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(repository_pattern_core INTERFACE)
target_include_directories(repository_pattern_core INTERFACE include)

add_executable(repository_pattern_example src/main.cpp)
target_link_libraries(repository_pattern_example PRIVATE repository_pattern_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(repository_pattern_tests tests/repository_pattern_tests.cpp)
    target_link_libraries(repository_pattern_tests PRIVATE repository_pattern_core)
    add_test(NAME repository_pattern_tests COMMAND repository_pattern_tests)
endif()
