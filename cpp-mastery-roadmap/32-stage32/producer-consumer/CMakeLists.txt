cmake_minimum_required(VERSION 3.16)

project(producer_consumer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_library(producer_consumer_core INTERFACE)
target_include_directories(producer_consumer_core INTERFACE include)
target_link_libraries(producer_consumer_core INTERFACE Threads::Threads)

add_executable(producer_consumer_example src/main.cpp)
target_link_libraries(producer_consumer_example PRIVATE producer_consumer_core Threads::Threads)

enable_testing()

if(BUILD_TESTING)
    add_executable(producer_consumer_tests tests/producer_consumer_tests.cpp)
    target_link_libraries(producer_consumer_tests PRIVATE producer_consumer_core Threads::Threads)
    add_test(NAME producer_consumer_tests COMMAND producer_consumer_tests)
endif()
