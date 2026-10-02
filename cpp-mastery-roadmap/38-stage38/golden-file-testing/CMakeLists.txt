cmake_minimum_required(VERSION 3.16)

project(golden_test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(golden_test_core INTERFACE)
target_include_directories(golden_test_core INTERFACE include)

add_executable(golden_test_example src/main.cpp)
target_link_libraries(golden_test_example PRIVATE golden_test_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(golden_test_tests tests/golden_test_tests.cpp)
    target_link_libraries(golden_test_tests PRIVATE golden_test_core)
    add_test(NAME golden_test_tests COMMAND golden_test_tests)
endif()
