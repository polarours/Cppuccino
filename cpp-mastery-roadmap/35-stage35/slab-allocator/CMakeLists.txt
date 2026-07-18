cmake_minimum_required(VERSION 3.16)

project(slab_allocator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(slab_allocator_core INTERFACE)
target_include_directories(slab_allocator_core INTERFACE include)

add_executable(slab_allocator_example src/main.cpp)
target_link_libraries(slab_allocator_example PRIVATE slab_allocator_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(slab_allocator_tests tests/slab_allocator_tests.cpp)
    target_link_libraries(slab_allocator_tests PRIVATE slab_allocator_core)
    add_test(NAME slab_allocator_tests COMMAND slab_allocator_tests)
endif()
