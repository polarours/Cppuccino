cmake_minimum_required(VERSION 3.16)

project(markdown_parser LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(markdown_parser_core src/markdown_parser.cpp)
target_include_directories(markdown_parser_core PUBLIC include)

add_executable(markdown_parser_example src/main.cpp)
target_link_libraries(markdown_parser_example PRIVATE markdown_parser_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(markdown_parser_tests tests/markdown_parser_tests.cpp)
    target_link_libraries(markdown_parser_tests PRIVATE markdown_parser_core)
    add_test(NAME markdown_parser_tests COMMAND markdown_parser_tests)
endif()
