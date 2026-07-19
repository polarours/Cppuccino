cmake_minimum_required(VERSION 3.16)

project(ci_cd_pattern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(ci_cd_pattern_core INTERFACE)
target_include_directories(ci_cd_pattern_core INTERFACE include)

add_executable(ci_cd_pattern_example src/main.cpp)
target_link_libraries(ci_cd_pattern_example PRIVATE ci_cd_pattern_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(ci_cd_pattern_tests tests/ci_cd_pattern_tests.cpp)
    target_link_libraries(ci_cd_pattern_tests PRIVATE ci_cd_pattern_core)
    add_test(NAME ci_cd_pattern_tests COMMAND ci_cd_pattern_tests)
endif()
