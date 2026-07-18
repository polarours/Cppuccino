cmake_minimum_required(VERSION 3.16)

project(memory_pool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(memory_pool_core INTERFACE)
target_include_directories(memory_pool_core INTERFACE include)

add_executable(memory_pool_example src/main.cpp)
target_link_libraries(memory_pool_example PRIVATE memory_pool_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(memory_pool_tests tests/memory_pool_tests.cpp)
    target_link_libraries(memory_pool_tests PRIVATE memory_pool_core)
    add_test(NAME memory_pool_tests COMMAND memory_pool_tests)
endif()
