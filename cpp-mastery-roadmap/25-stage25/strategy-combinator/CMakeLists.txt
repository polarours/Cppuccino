cmake_minimum_required(VERSION 3.16)

project(strategy_combinator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(strategy_combinator_core INTERFACE)
target_include_directories(strategy_combinator_core INTERFACE include)

add_executable(strategy_combinator_example src/main.cpp)
target_link_libraries(strategy_combinator_example PRIVATE strategy_combinator_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(strategy_combinator_tests tests/strategy_combinator_tests.cpp)
    target_link_libraries(strategy_combinator_tests PRIVATE strategy_combinator_core)
    add_test(NAME strategy_combinator_tests COMMAND strategy_combinator_tests)
endif()
