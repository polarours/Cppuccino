cmake_minimum_required(VERSION 3.16)

project(error_category LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(error_category_core src/my_error.cpp)
target_include_directories(error_category_core PUBLIC include)

add_executable(error_category_example src/main.cpp)
target_link_libraries(error_category_example PRIVATE error_category_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(my_error_tests tests/my_error_tests.cpp)
    target_link_libraries(my_error_tests PRIVATE error_category_core)
    add_test(NAME my_error_tests COMMAND my_error_tests)
endif()
