cmake_minimum_required(VERSION 3.16)

project(flyweight_pattern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(flyweight_pattern_core INTERFACE)
target_include_directories(flyweight_pattern_core INTERFACE include)

add_executable(flyweight_pattern_example src/main.cpp)
target_link_libraries(flyweight_pattern_example PRIVATE flyweight_pattern_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(flyweight_pattern_tests tests/flyweight_pattern_tests.cpp)
    target_link_libraries(flyweight_pattern_tests PRIVATE flyweight_pattern_core)
    add_test(NAME flyweight_pattern_tests COMMAND flyweight_pattern_tests)
endif()
