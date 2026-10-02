cmake_minimum_required(VERSION 3.16)

project(link_checker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(link_checker_core src/link_checker.cpp)
target_include_directories(link_checker_core PUBLIC include)

add_executable(link_checker_example src/main.cpp)
target_link_libraries(link_checker_example PRIVATE link_checker_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(link_checker_tests tests/link_checker_tests.cpp)
    target_link_libraries(link_checker_tests PRIVATE link_checker_core)
    add_test(NAME link_checker_tests COMMAND link_checker_tests)
endif()
