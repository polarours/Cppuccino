cmake_minimum_required(VERSION 3.16)

project(observer_mediator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(observer_mediator_core INTERFACE)
target_include_directories(observer_mediator_core INTERFACE include)

add_executable(observer_mediator_example src/main.cpp)
target_link_libraries(observer_mediator_example PRIVATE observer_mediator_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(observer_mediator_tests tests/observer_mediator_tests.cpp)
    target_link_libraries(observer_mediator_tests PRIVATE observer_mediator_core)
    add_test(NAME observer_mediator_tests COMMAND observer_mediator_tests)
endif()
