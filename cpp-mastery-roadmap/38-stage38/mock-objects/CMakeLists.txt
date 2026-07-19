cmake_minimum_required(VERSION 3.16)

project(mock_objects LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(mock_objects_core INTERFACE)
target_include_directories(mock_objects_core INTERFACE include)

add_executable(mock_objects_example src/main.cpp)
target_link_libraries(mock_objects_example PRIVATE mock_objects_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(mock_objects_tests tests/mock_objects_tests.cpp)
    target_link_libraries(mock_objects_tests PRIVATE mock_objects_core)
    add_test(NAME mock_objects_tests COMMAND mock_objects_tests)
endif()
