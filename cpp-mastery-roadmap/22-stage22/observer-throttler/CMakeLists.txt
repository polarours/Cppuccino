cmake_minimum_required(VERSION 3.16)

project(observer_throttler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_library(observer_throttler_core INTERFACE)
target_include_directories(observer_throttler_core INTERFACE include)
target_link_libraries(observer_throttler_core INTERFACE Threads::Threads)

add_executable(observer_throttler_example src/main.cpp)
target_link_libraries(observer_throttler_example PRIVATE observer_throttler_core Threads::Threads)

enable_testing()

if(BUILD_TESTING)
    add_executable(observer_throttler_tests tests/observer_throttler_tests.cpp)
    target_link_libraries(observer_throttler_tests PRIVATE observer_throttler_core Threads::Threads)
    add_test(NAME observer_throttler_tests COMMAND observer_throttler_tests)
endif()
