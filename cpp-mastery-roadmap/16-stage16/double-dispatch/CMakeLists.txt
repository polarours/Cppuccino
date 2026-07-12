cmake_minimum_required(VERSION 3.16)

project(double_dispatch LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(double_dispatch_core INTERFACE)
target_include_directories(double_dispatch_core INTERFACE include)

add_executable(double_dispatch_example src/main.cpp)
target_link_libraries(double_dispatch_example PRIVATE double_dispatch_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(double_dispatch_tests tests/double_dispatch_tests.cpp)
    target_link_libraries(double_dispatch_tests PRIVATE double_dispatch_core)
    add_test(NAME double_dispatch_tests COMMAND double_dispatch_tests)
endif()
