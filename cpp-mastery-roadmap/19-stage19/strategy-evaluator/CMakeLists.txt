cmake_minimum_required(VERSION 3.16)

project(strategy_evaluator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(strategy_evaluator_core INTERFACE)
target_include_directories(strategy_evaluator_core INTERFACE include)

add_executable(strategy_evaluator_example src/main.cpp)
target_link_libraries(strategy_evaluator_example PRIVATE strategy_evaluator_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(strategy_evaluator_tests tests/strategy_evaluator_tests.cpp)
    target_link_libraries(strategy_evaluator_tests PRIVATE strategy_evaluator_core)
    add_test(NAME strategy_evaluator_tests COMMAND strategy_evaluator_tests)
endif()
