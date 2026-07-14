cmake_minimum_required(VERSION 3.16)

project(strategy_decorator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(strategy_decorator_core INTERFACE)
target_include_directories(strategy_decorator_core INTERFACE include)

add_executable(strategy_decorator_example src/main.cpp)
target_link_libraries(strategy_decorator_example PRIVATE strategy_decorator_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(strategy_decorator_tests tests/strategy_decorator_tests.cpp)
    target_link_libraries(strategy_decorator_tests PRIVATE strategy_decorator_core)
    add_test(NAME strategy_decorator_tests COMMAND strategy_decorator_tests)
endif()
