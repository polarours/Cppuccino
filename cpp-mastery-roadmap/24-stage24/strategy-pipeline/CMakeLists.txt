cmake_minimum_required(VERSION 3.16)

project(strategy_pipeline LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(strategy_pipeline_core INTERFACE)
target_include_directories(strategy_pipeline_core INTERFACE include)

add_executable(strategy_pipeline_example src/main.cpp)
target_link_libraries(strategy_pipeline_example PRIVATE strategy_pipeline_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(strategy_pipeline_tests tests/strategy_pipeline_tests.cpp)
    target_link_libraries(strategy_pipeline_tests PRIVATE strategy_pipeline_core)
    add_test(NAME strategy_pipeline_tests COMMAND strategy_pipeline_tests)
endif()
