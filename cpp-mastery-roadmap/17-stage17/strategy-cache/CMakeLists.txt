cmake_minimum_required(VERSION 3.16)

project(strategy_cache LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_library(strategy_cache_core INTERFACE)
target_include_directories(strategy_cache_core INTERFACE include)
target_link_libraries(strategy_cache_core INTERFACE Threads::Threads)

add_executable(strategy_cache_example src/main.cpp)
target_link_libraries(strategy_cache_example PRIVATE strategy_cache_core Threads::Threads)

enable_testing()

if(BUILD_TESTING)
    add_executable(strategy_cache_tests tests/strategy_cache_tests.cpp)
    target_link_libraries(strategy_cache_tests PRIVATE strategy_cache_core Threads::Threads)
    add_test(NAME strategy_cache_tests COMMAND strategy_cache_tests)
endif()
