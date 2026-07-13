cmake_minimum_required(VERSION 3.16)

project(strategy_selector LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(strategy_selector_core INTERFACE)
target_include_directories(strategy_selector_core INTERFACE include)

add_executable(strategy_selector_example src/main.cpp)
target_link_libraries(strategy_selector_example PRIVATE strategy_selector_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(strategy_selector_tests tests/strategy_selector_tests.cpp)
    target_link_libraries(strategy_selector_tests PRIVATE strategy_selector_core)
    add_test(NAME strategy_selector_tests COMMAND strategy_selector_tests)
endif()
