cmake_minimum_required(VERSION 3.16)

project(template_method LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(template_method_core INTERFACE)
target_include_directories(template_method_core INTERFACE include)

add_executable(template_method_example src/main.cpp)
target_link_libraries(template_method_example PRIVATE template_method_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(template_method_tests tests/template_method_tests.cpp)
    target_link_libraries(template_method_tests PRIVATE template_method_core)
    add_test(NAME template_method_tests COMMAND template_method_tests)
endif()
