cmake_minimum_required(VERSION 3.16)

project(json_parser LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(json_parser_core src/json.cpp)
target_include_directories(json_parser_core PUBLIC include)

add_executable(json_example src/main.cpp)
target_link_libraries(json_example PRIVATE json_parser_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(json_tests tests/json_tests.cpp)
    target_link_libraries(json_tests PRIVATE json_parser_core)
    add_test(NAME json_tests COMMAND json_tests)
endif()
