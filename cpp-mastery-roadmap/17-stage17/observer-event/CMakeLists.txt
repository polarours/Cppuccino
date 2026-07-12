cmake_minimum_required(VERSION 3.16)

project(observer_event LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(observer_event_core INTERFACE)
target_include_directories(observer_event_core INTERFACE include)

add_executable(observer_event_example src/main.cpp)
target_link_libraries(observer_event_example PRIVATE observer_event_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(observer_event_tests tests/observer_event_tests.cpp)
    target_link_libraries(observer_event_tests PRIVATE observer_event_core)
    add_test(NAME observer_event_tests COMMAND observer_event_tests)
endif()
