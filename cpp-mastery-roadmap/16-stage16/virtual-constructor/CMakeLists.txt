cmake_minimum_required(VERSION 3.16)

project(virtual_constructor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(virtual_constructor_core INTERFACE)
target_include_directories(virtual_constructor_core INTERFACE include)

add_executable(virtual_constructor_example src/main.cpp)
target_link_libraries(virtual_constructor_example PRIVATE virtual_constructor_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(virtual_constructor_tests tests/virtual_constructor_tests.cpp)
    target_link_libraries(virtual_constructor_tests PRIVATE virtual_constructor_core)
    add_test(NAME virtual_constructor_tests COMMAND virtual_constructor_tests)
endif()
