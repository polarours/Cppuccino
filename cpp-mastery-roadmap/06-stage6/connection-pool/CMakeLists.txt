cmake_minimum_required(VERSION 3.16)

project(connection_pool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_library(connection_pool_core INTERFACE)
target_include_directories(connection_pool_core INTERFACE include)
target_link_libraries(connection_pool_core INTERFACE Threads::Threads)

add_executable(connection_pool_example src/main.cpp)
target_link_libraries(connection_pool_example PRIVATE connection_pool_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(connection_pool_tests tests/connection_pool_tests.cpp)
    target_link_libraries(connection_pool_tests PRIVATE connection_pool_core Threads::Threads)
    add_test(NAME connection_pool_tests COMMAND connection_pool_tests)
endif()
