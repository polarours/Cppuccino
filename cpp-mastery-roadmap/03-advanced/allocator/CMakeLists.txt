cmake_minimum_required(VERSION 3.16)

project(simple_allocator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(simple_allocator_core src/simple_allocator.cpp)
target_include_directories(simple_allocator_core PUBLIC include)

add_executable(allocator_example src/main.cpp)
target_link_libraries(allocator_example PRIVATE simple_allocator_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(allocator_tests tests/allocator_tests.cpp)
    target_link_libraries(allocator_tests PRIVATE simple_allocator_core)
    add_test(NAME allocator_tests COMMAND allocator_tests)
endif()
