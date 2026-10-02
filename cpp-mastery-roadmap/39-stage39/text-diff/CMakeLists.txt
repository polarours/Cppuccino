cmake_minimum_required(VERSION 3.16)

project(text_diff LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(text_diff_core src/text_diff.cpp)
target_include_directories(text_diff_core PUBLIC include)

add_executable(text_diff_example src/main.cpp)
target_link_libraries(text_diff_example PRIVATE text_diff_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(text_diff_tests tests/text_diff_tests.cpp)
    target_link_libraries(text_diff_tests PRIVATE text_diff_core)
    add_test(NAME text_diff_tests COMMAND text_diff_tests)
endif()
