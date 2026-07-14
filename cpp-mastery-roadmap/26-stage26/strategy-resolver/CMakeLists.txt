cmake_minimum_required(VERSION 3.16)

project(strategy_resolver LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(strategy_resolver_core INTERFACE)
target_include_directories(strategy_resolver_core INTERFACE include)

add_executable(strategy_resolver_example src/main.cpp)
target_link_libraries(strategy_resolver_example PRIVATE strategy_resolver_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(strategy_resolver_tests tests/strategy_resolver_tests.cpp)
    target_link_libraries(strategy_resolver_tests PRIVATE strategy_resolver_core)
    add_test(NAME strategy_resolver_tests COMMAND strategy_resolver_tests)
endif()
