cmake_minimum_required(VERSION 3.16)

project(event_loop LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_library(event_loop_core src/event_loop.cpp)
target_include_directories(event_loop_core PUBLIC include)
target_link_libraries(event_loop_core PUBLIC Threads::Threads)

add_executable(event_loop_example src/main.cpp)
target_link_libraries(event_loop_example PRIVATE event_loop_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(event_loop_tests tests/event_loop_tests.cpp)
    target_link_libraries(event_loop_tests PRIVATE event_loop_core)
    add_test(NAME event_loop_tests COMMAND event_loop_tests)
endif()
