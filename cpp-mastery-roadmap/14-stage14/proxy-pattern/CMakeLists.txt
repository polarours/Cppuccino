cmake_minimum_required(VERSION 3.16)

project(proxy_pattern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(proxy_pattern_core INTERFACE)
target_include_directories(proxy_pattern_core INTERFACE include)

add_executable(proxy_pattern_example src/main.cpp)
target_link_libraries(proxy_pattern_example PRIVATE proxy_pattern_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(proxy_pattern_tests tests/proxy_pattern_tests.cpp)
    target_link_libraries(proxy_pattern_tests PRIVATE proxy_pattern_core)
    add_test(NAME proxy_pattern_tests COMMAND proxy_pattern_tests)
endif()
