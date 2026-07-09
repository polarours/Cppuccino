cmake_minimum_required(VERSION 3.16)

project(task_scheduler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_library(task_scheduler_core src/task_scheduler.cpp)
target_include_directories(task_scheduler_core PUBLIC include)
target_link_libraries(task_scheduler_core PUBLIC Threads::Threads)

add_executable(task_scheduler_example src/main.cpp)
target_link_libraries(task_scheduler_example PRIVATE task_scheduler_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(task_scheduler_tests tests/task_scheduler_tests.cpp)
    target_link_libraries(task_scheduler_tests PRIVATE task_scheduler_core)
    add_test(NAME task_scheduler_tests COMMAND task_scheduler_tests)
endif()
