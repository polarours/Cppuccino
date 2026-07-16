cmake_minimum_required(VERSION 3.16)

project(crtp_pattern LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(crtp_pattern_core INTERFACE)
target_include_directories(crtp_pattern_core INTERFACE include)

add_executable(crtp_pattern_example src/main.cpp)
target_link_libraries(crtp_pattern_example PRIVATE crtp_pattern_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(crtp_pattern_tests tests/crtp_pattern_tests.cpp)
    target_link_libraries(crtp_pattern_tests PRIVATE crtp_pattern_core)
    add_test(NAME crtp_pattern_tests COMMAND crtp_pattern_tests)
endif()
