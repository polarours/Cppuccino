cmake_minimum_required(VERSION 3.16)

project(strategy_pattern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(strategy_pattern_core INTERFACE)
target_include_directories(strategy_pattern_core INTERFACE include)

add_executable(strategy_pattern_example src/main.cpp)
target_link_libraries(strategy_pattern_example PRIVATE strategy_pattern_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(strategy_pattern_tests tests/strategy_pattern_tests.cpp)
    target_link_libraries(strategy_pattern_tests PRIVATE strategy_pattern_core)
    add_test(NAME strategy_pattern_tests COMMAND strategy_pattern_tests)
endif()
