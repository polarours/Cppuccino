cmake_minimum_required(VERSION 3.16)

project(middleware_pipeline LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(middleware_pipeline_core INTERFACE)
target_include_directories(middleware_pipeline_core INTERFACE include)

add_executable(middleware_pipeline_example src/main.cpp)
target_link_libraries(middleware_pipeline_example PRIVATE middleware_pipeline_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(middleware_pipeline_tests tests/middleware_pipeline_tests.cpp)
    target_link_libraries(middleware_pipeline_tests PRIVATE middleware_pipeline_core)
    add_test(NAME middleware_pipeline_tests COMMAND middleware_pipeline_tests)
endif()
