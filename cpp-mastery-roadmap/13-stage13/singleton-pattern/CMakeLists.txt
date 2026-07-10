cmake_minimum_required(VERSION 3.16)

project(singleton_pattern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_library(singleton_pattern_core INTERFACE)
target_include_directories(singleton_pattern_core INTERFACE include)
target_link_libraries(singleton_pattern_core INTERFACE Threads::Threads)

add_executable(singleton_pattern_example src/main.cpp)
target_link_libraries(singleton_pattern_example PRIVATE singleton_pattern_core Threads::Threads)

enable_testing()

if(BUILD_TESTING)
    add_executable(singleton_pattern_tests tests/singleton_pattern_tests.cpp)
    target_link_libraries(singleton_pattern_tests PRIVATE singleton_pattern_core Threads::Threads)
    add_test(NAME singleton_pattern_tests COMMAND singleton_pattern_tests)
endif()
