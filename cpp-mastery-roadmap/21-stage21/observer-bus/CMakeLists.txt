cmake_minimum_required(VERSION 3.16)

project(observer_bus LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(observer_bus_core INTERFACE)
target_include_directories(observer_bus_core INTERFACE include)

add_executable(observer_bus_example src/main.cpp)
target_link_libraries(observer_bus_example PRIVATE observer_bus_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(observer_bus_tests tests/observer_bus_tests.cpp)
    target_link_libraries(observer_bus_tests PRIVATE observer_bus_core)
    add_test(NAME observer_bus_tests COMMAND observer_bus_tests)
endif()
