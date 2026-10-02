cmake_minimum_required(VERSION 3.16)

project(allocation_tracker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(allocation_tracker_core src/allocation_tracker.cpp)
target_include_directories(allocation_tracker_core PUBLIC include)

add_executable(allocation_tracker_example src/main.cpp)
target_link_libraries(allocation_tracker_example PRIVATE allocation_tracker_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(allocation_tracker_tests tests/allocation_tracker_tests.cpp)
    target_link_libraries(allocation_tracker_tests PRIVATE allocation_tracker_core)
    add_test(NAME allocation_tracker_tests COMMAND allocation_tracker_tests)
endif()
