cmake_minimum_required(VERSION 3.16)

project(circuit_breaker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_library(circuit_breaker_core INTERFACE)
target_include_directories(circuit_breaker_core INTERFACE include)
target_link_libraries(circuit_breaker_core INTERFACE Threads::Threads)

add_executable(circuit_breaker_example src/main.cpp)
target_link_libraries(circuit_breaker_example PRIVATE circuit_breaker_core Threads::Threads)

enable_testing()

if(BUILD_TESTING)
    add_executable(circuit_breaker_tests tests/circuit_breaker_tests.cpp)
    target_link_libraries(circuit_breaker_tests PRIVATE circuit_breaker_core Threads::Threads)
    add_test(NAME circuit_breaker_tests COMMAND circuit_breaker_tests)
endif()
