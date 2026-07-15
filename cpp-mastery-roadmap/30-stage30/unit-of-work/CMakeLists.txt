cmake_minimum_required(VERSION 3.16)

project(unit_of_work LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(unit_of_work_core INTERFACE)
target_include_directories(unit_of_work_core INTERFACE include)

add_executable(unit_of_work_example src/main.cpp)
target_link_libraries(unit_of_work_example PRIVATE unit_of_work_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(unit_of_work_tests tests/unit_of_work_tests.cpp)
    target_link_libraries(unit_of_work_tests PRIVATE unit_of_work_core)
    add_test(NAME unit_of_work_tests COMMAND unit_of_work_tests)
endif()
