cmake_minimum_required(VERSION 3.16)

project(command_router LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(command_router_core INTERFACE)
target_include_directories(command_router_core INTERFACE include)

add_executable(command_router_example src/main.cpp)
target_link_libraries(command_router_example PRIVATE command_router_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(command_router_tests tests/command_router_tests.cpp)
    target_link_libraries(command_router_tests PRIVATE command_router_core)
    add_test(NAME command_router_tests COMMAND command_router_tests)
endif()
