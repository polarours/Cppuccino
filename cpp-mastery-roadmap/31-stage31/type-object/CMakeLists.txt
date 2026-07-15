cmake_minimum_required(VERSION 3.16)

project(type_object LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(type_object_core INTERFACE)
target_include_directories(type_object_core INTERFACE include)

add_executable(type_object_example src/main.cpp)
target_link_libraries(type_object_example PRIVATE type_object_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(type_object_tests tests/type_object_tests.cpp)
    target_link_libraries(type_object_tests PRIVATE type_object_core)
    add_test(NAME type_object_tests COMMAND type_object_tests)
endif()
