cmake_minimum_required(VERSION 3.16)

project(state_machine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(state_machine_core INTERFACE)
target_include_directories(state_machine_core INTERFACE include)

add_executable(state_machine_example src/main.cpp)
target_link_libraries(state_machine_example PRIVATE state_machine_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(state_machine_tests tests/state_machine_tests.cpp)
    target_link_libraries(state_machine_tests PRIVATE state_machine_core)
    add_test(NAME state_machine_tests COMMAND state_machine_tests)
endif()
