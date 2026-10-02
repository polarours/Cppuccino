cmake_minimum_required(VERSION 3.16)

project(type_erasure LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(type_erasure_core INTERFACE)
target_include_directories(type_erasure_core INTERFACE include)

add_executable(type_erasure_example src/main.cpp)
target_link_libraries(type_erasure_example PRIVATE type_erasure_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(type_erasure_tests tests/type_erasure_tests.cpp)
    target_link_libraries(type_erasure_tests PRIVATE type_erasure_core)
    add_test(NAME type_erasure_tests COMMAND type_erasure_tests)
endif()
