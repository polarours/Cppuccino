cmake_minimum_required(VERSION 3.16)

project(generator_pipeline LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(pipeline_core src/pipeline.cpp)
target_include_directories(pipeline_core PUBLIC include)

add_executable(pipeline_example src/main.cpp)
target_link_libraries(pipeline_example PRIVATE pipeline_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(pipeline_tests tests/pipeline_tests.cpp)
    target_link_libraries(pipeline_tests PRIVATE pipeline_core)
    add_test(NAME pipeline_tests COMMAND pipeline_tests)
endif()
