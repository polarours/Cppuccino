cmake_minimum_required(VERSION 3.16)

project(strategy_adapter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(strategy_adapter_core INTERFACE)
target_include_directories(strategy_adapter_core INTERFACE include)

add_executable(strategy_adapter_example src/main.cpp)
target_link_libraries(strategy_adapter_example PRIVATE strategy_adapter_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(strategy_adapter_tests tests/strategy_adapter_tests.cpp)
    target_link_libraries(strategy_adapter_tests PRIVATE strategy_adapter_core)
    add_test(NAME strategy_adapter_tests COMMAND strategy_adapter_tests)
endif()
