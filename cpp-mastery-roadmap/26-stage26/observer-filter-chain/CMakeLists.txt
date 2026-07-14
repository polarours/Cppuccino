cmake_minimum_required(VERSION 3.16)

project(observer_filter_chain LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(observer_filter_chain_core INTERFACE)
target_include_directories(observer_filter_chain_core INTERFACE include)

add_executable(observer_filter_chain_example src/main.cpp)
target_link_libraries(observer_filter_chain_example PRIVATE observer_filter_chain_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(observer_filter_chain_tests tests/observer_filter_chain_tests.cpp)
    target_link_libraries(observer_filter_chain_tests PRIVATE observer_filter_chain_core)
    add_test(NAME observer_filter_chain_tests COMMAND observer_filter_chain_tests)
endif()
