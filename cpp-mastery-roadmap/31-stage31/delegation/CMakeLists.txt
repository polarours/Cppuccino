cmake_minimum_required(VERSION 3.16)

project(delegation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(delegation_core INTERFACE)
target_include_directories(delegation_core INTERFACE include)

add_executable(delegation_example src/main.cpp)
target_link_libraries(delegation_example PRIVATE delegation_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(delegation_tests tests/delegation_tests.cpp)
    target_link_libraries(delegation_tests PRIVATE delegation_core)
    add_test(NAME delegation_tests COMMAND delegation_tests)
endif()
