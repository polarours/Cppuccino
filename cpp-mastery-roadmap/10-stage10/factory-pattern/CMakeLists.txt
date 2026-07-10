cmake_minimum_required(VERSION 3.16)

project(factory_pattern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(factory_pattern_core INTERFACE)
target_include_directories(factory_pattern_core INTERFACE include)

add_executable(factory_pattern_example src/main.cpp)
target_link_libraries(factory_pattern_example PRIVATE factory_pattern_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(factory_pattern_tests tests/factory_pattern_tests.cpp)
    target_link_libraries(factory_pattern_tests PRIVATE factory_pattern_core)
    add_test(NAME factory_pattern_tests COMMAND factory_pattern_tests)
endif()
