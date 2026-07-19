cmake_minimum_required(VERSION 3.16)

project(deducing_this LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(deducing_this_core INTERFACE)
target_include_directories(deducing_this_core INTERFACE include)

add_executable(deducing_this_example src/main.cpp)
target_link_libraries(deducing_this_example PRIVATE deducing_this_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(deducing_this_tests tests/deducing_this_tests.cpp)
    target_link_libraries(deducing_this_tests PRIVATE deducing_this_core)
    add_test(NAME deducing_this_tests COMMAND deducing_this_tests)
endif()
