cmake_minimum_required(VERSION 3.16)

project(timer_wheel LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(timer_wheel_core src/timer_wheel.cpp)
target_include_directories(timer_wheel_core PUBLIC include)

add_executable(timer_wheel_example src/main.cpp)
target_link_libraries(timer_wheel_example PRIVATE timer_wheel_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(timer_wheel_tests tests/timer_wheel_tests.cpp)
    target_link_libraries(timer_wheel_tests PRIVATE timer_wheel_core)
    add_test(NAME timer_wheel_tests COMMAND timer_wheel_tests)
endif()
