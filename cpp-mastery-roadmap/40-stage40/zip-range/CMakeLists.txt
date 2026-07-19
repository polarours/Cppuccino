cmake_minimum_required(VERSION 3.16)

project(zip_range LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(zip_range_core INTERFACE)
target_include_directories(zip_range_core INTERFACE include)

add_executable(zip_range_example src/main.cpp)
target_link_libraries(zip_range_example PRIVATE zip_range_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(zip_range_tests tests/zip_range_tests.cpp)
    target_link_libraries(zip_range_tests PRIVATE zip_range_core)
    add_test(NAME zip_range_tests COMMAND zip_range_tests)
endif()
