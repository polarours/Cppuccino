cmake_minimum_required(VERSION 3.16)

project(csv_parser LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(csv_parser_core src/csv_parser.cpp)
target_include_directories(csv_parser_core PUBLIC include)

add_executable(csv_parser_example src/main.cpp)
target_link_libraries(csv_parser_example PRIVATE csv_parser_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(csv_parser_tests tests/csv_parser_tests.cpp)
    target_link_libraries(csv_parser_tests PRIVATE csv_parser_core)
    add_test(NAME csv_parser_tests COMMAND csv_parser_tests)
endif()
