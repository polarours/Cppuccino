cmake_minimum_required(VERSION 3.16)

project(strategy_chain LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(strategy_chain_core INTERFACE)
target_include_directories(strategy_chain_core INTERFACE include)

add_executable(strategy_chain_example src/main.cpp)
target_link_libraries(strategy_chain_example PRIVATE strategy_chain_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(strategy_chain_tests tests/strategy_chain_tests.cpp)
    target_link_libraries(strategy_chain_tests PRIVATE strategy_chain_core)
    add_test(NAME strategy_chain_tests COMMAND strategy_chain_tests)
endif()
