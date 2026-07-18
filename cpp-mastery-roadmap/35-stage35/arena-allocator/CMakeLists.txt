cmake_minimum_required(VERSION 3.16)

project(arena_allocator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(arena_allocator_core INTERFACE)
target_include_directories(arena_allocator_core INTERFACE include)

add_executable(arena_allocator_example src/main.cpp)
target_link_libraries(arena_allocator_example PRIVATE arena_allocator_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(arena_allocator_tests tests/arena_allocator_tests.cpp)
    target_link_libraries(arena_allocator_tests PRIVATE arena_allocator_core)
    add_test(NAME arena_allocator_tests COMMAND arena_allocator_tests)
endif()
