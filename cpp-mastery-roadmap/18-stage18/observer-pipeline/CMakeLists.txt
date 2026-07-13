cmake_minimum_required(VERSION 3.16)

project(observer_pipeline LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(observer_pipeline_core INTERFACE)
target_include_directories(observer_pipeline_core INTERFACE include)

add_executable(observer_pipeline_example src/main.cpp)
target_link_libraries(observer_pipeline_example PRIVATE observer_pipeline_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(observer_pipeline_tests tests/observer_pipeline_tests.cpp)
    target_link_libraries(observer_pipeline_tests PRIVATE observer_pipeline_core)
    add_test(NAME observer_pipeline_tests COMMAND observer_pipeline_tests)
endif()
