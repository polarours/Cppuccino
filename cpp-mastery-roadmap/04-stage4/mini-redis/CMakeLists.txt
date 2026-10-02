cmake_minimum_required(VERSION 3.16)

project(mini_redis LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(mini_redis_core src/mini_redis.cpp)
target_include_directories(mini_redis_core PUBLIC include)

add_executable(mini_redis_example src/main.cpp)
target_link_libraries(mini_redis_example PRIVATE mini_redis_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(mini_redis_tests tests/mini_redis_tests.cpp)
    target_link_libraries(mini_redis_tests PRIVATE mini_redis_core)
    add_test(NAME mini_redis_tests COMMAND mini_redis_tests)
endif()
