cmake_minimum_required(VERSION 3.16)

project(command_queue_v2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_library(command_queue_v2_core src/command_queue_v2.cpp)
target_include_directories(command_queue_v2_core PUBLIC include)
target_link_libraries(command_queue_v2_core PUBLIC Threads::Threads)

add_executable(command_queue_v2_example src/main.cpp)
target_link_libraries(command_queue_v2_example PRIVATE command_queue_v2_core Threads::Threads)

enable_testing()

if(BUILD_TESTING)
    add_executable(command_queue_v2_tests tests/command_queue_v2_tests.cpp)
    target_link_libraries(command_queue_v2_tests PRIVATE command_queue_v2_core Threads::Threads)
    add_test(NAME command_queue_v2_tests COMMAND command_queue_v2_tests)
endif()
