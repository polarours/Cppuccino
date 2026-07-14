cmake_minimum_required(VERSION 3.16)

project(observer_subscription LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(observer_subscription_core INTERFACE)
target_include_directories(observer_subscription_core INTERFACE include)

add_executable(observer_subscription_example src/main.cpp)
target_link_libraries(observer_subscription_example PRIVATE observer_subscription_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(observer_subscription_tests tests/observer_subscription_tests.cpp)
    target_link_libraries(observer_subscription_tests PRIVATE observer_subscription_core)
    add_test(NAME observer_subscription_tests COMMAND observer_subscription_tests)
endif()
