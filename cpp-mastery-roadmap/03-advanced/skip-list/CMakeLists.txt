cmake_minimum_required(VERSION 3.16)

project(skip_list LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(skip_list_core INTERFACE)
target_include_directories(skip_list_core INTERFACE include)

add_executable(skip_list_example src/main.cpp)
target_link_libraries(skip_list_example PRIVATE skip_list_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(skip_list_tests tests/skip_list_tests.cpp)
    target_link_libraries(skip_list_tests PRIVATE skip_list_core)
    add_test(NAME skip_list_tests COMMAND skip_list_tests)
endif()
