cmake_minimum_required(VERSION 3.16)

project(strategy_factory LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(strategy_factory_core INTERFACE)
target_include_directories(strategy_factory_core INTERFACE include)

add_executable(strategy_factory_example src/main.cpp)
target_link_libraries(strategy_factory_example PRIVATE strategy_factory_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(strategy_factory_tests tests/strategy_factory_tests.cpp)
    target_link_libraries(strategy_factory_tests PRIVATE strategy_factory_core)
    add_test(NAME strategy_factory_tests COMMAND strategy_factory_tests)
endif()
