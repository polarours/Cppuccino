cmake_minimum_required(VERSION 3.16)

project(mediator_pattern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(mediator_pattern_core INTERFACE)
target_include_directories(mediator_pattern_core INTERFACE include)

add_executable(mediator_pattern_example src/main.cpp)
target_link_libraries(mediator_pattern_example PRIVATE mediator_pattern_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(mediator_pattern_tests tests/mediator_pattern_tests.cpp)
    target_link_libraries(mediator_pattern_tests PRIVATE mediator_pattern_core)
    add_test(NAME mediator_pattern_tests COMMAND mediator_pattern_tests)
endif()
