cmake_minimum_required(VERSION 3.16)

project(observer_filter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(observer_filter_core INTERFACE)
target_include_directories(observer_filter_core INTERFACE include)

add_executable(observer_filter_example src/main.cpp)
target_link_libraries(observer_filter_example PRIVATE observer_filter_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(observer_filter_tests tests/observer_filter_tests.cpp)
    target_link_libraries(observer_filter_tests PRIVATE observer_filter_core)
    add_test(NAME observer_filter_tests COMMAND observer_filter_tests)
endif()
