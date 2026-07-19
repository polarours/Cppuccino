cmake_minimum_required(VERSION 3.16)

project(cache_friendly LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(cache_friendly_core INTERFACE)
target_include_directories(cache_friendly_core INTERFACE include)

add_executable(cache_friendly_example src/main.cpp)
target_link_libraries(cache_friendly_example PRIVATE cache_friendly_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(cache_friendly_tests tests/cache_friendly_tests.cpp)
    target_link_libraries(cache_friendly_tests PRIVATE cache_friendly_core)
    add_test(NAME cache_friendly_tests COMMAND cache_friendly_tests)
endif()
