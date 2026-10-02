cmake_minimum_required(VERSION 3.16)

project(template_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(template_engine_core src/template_engine.cpp)
target_include_directories(template_engine_core PUBLIC include)

add_executable(template_engine_example src/main.cpp)
target_link_libraries(template_engine_example PRIVATE template_engine_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(template_engine_tests tests/template_engine_tests.cpp)
    target_link_libraries(template_engine_tests PRIVATE template_engine_core)
    add_test(NAME template_engine_tests COMMAND template_engine_tests)
endif()
