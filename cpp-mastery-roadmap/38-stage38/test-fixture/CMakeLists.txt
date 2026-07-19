cmake_minimum_required(VERSION 3.16)

project(test_fixture LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(test_fixture_core INTERFACE)
target_include_directories(test_fixture_core INTERFACE include)

add_executable(test_fixture_example src/main.cpp)
target_link_libraries(test_fixture_example PRIVATE test_fixture_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(test_fixture_tests tests/test_fixture_tests.cpp)
    target_link_libraries(test_fixture_tests PRIVATE test_fixture_core)
    add_test(NAME test_fixture_tests COMMAND test_fixture_tests)
endif()
