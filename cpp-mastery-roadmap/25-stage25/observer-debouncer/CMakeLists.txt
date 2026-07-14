cmake_minimum_required(VERSION 3.16)

project(observer_debouncer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_library(observer_debouncer_core INTERFACE)
target_include_directories(observer_debouncer_core INTERFACE include)
target_link_libraries(observer_debouncer_core INTERFACE Threads::Threads)

add_executable(observer_debouncer_example src/main.cpp)
target_link_libraries(observer_debouncer_example PRIVATE observer_debouncer_core Threads::Threads)

enable_testing()

if(BUILD_TESTING)
    add_executable(observer_debouncer_tests tests/observer_debouncer_tests.cpp)
    target_link_libraries(observer_debouncer_tests PRIVATE observer_debouncer_core Threads::Threads)
    add_test(NAME observer_debouncer_tests COMMAND observer_debouncer_tests)
endif()
