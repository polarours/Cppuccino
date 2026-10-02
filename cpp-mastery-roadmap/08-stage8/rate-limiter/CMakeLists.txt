cmake_minimum_required(VERSION 3.16)

project(rate_limiter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_library(rate_limiter_core src/rate_limiter.cpp)
target_include_directories(rate_limiter_core PUBLIC include)
target_link_libraries(rate_limiter_core PUBLIC Threads::Threads)

add_executable(rate_limiter_example src/main.cpp)
target_link_libraries(rate_limiter_example PRIVATE rate_limiter_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(rate_limiter_tests tests/rate_limiter_tests.cpp)
    target_link_libraries(rate_limiter_tests PRIVATE rate_limiter_core)
    add_test(NAME rate_limiter_tests COMMAND rate_limiter_tests)
endif()
