cmake_minimum_required(VERSION 3.16)

project(publish_subscribe LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_library(pubsub_core INTERFACE)
target_include_directories(pubsub_core INTERFACE include)
target_link_libraries(pubsub_core INTERFACE Threads::Threads)

add_executable(pubsub_example src/main.cpp)
target_link_libraries(pubsub_example PRIVATE pubsub_core Threads::Threads)

enable_testing()

if(BUILD_TESTING)
    add_executable(pubsub_tests tests/pubsub_tests.cpp)
    target_link_libraries(pubsub_tests PRIVATE pubsub_core Threads::Threads)
    add_test(NAME pubsub_tests COMMAND pubsub_tests)
endif()
