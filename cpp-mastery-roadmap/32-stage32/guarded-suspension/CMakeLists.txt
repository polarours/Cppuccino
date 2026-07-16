cmake_minimum_required(VERSION 3.16)

project(guarded_suspension LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_library(guarded_suspension_core INTERFACE)
target_include_directories(guarded_suspension_core INTERFACE include)
target_link_libraries(guarded_suspension_core INTERFACE Threads::Threads)

add_executable(guarded_suspension_example src/main.cpp)
target_link_libraries(guarded_suspension_example PRIVATE guarded_suspension_core Threads::Threads)

enable_testing()

if(BUILD_TESTING)
    add_executable(guarded_suspension_tests tests/guarded_suspension_tests.cpp)
    target_link_libraries(guarded_suspension_tests PRIVATE guarded_suspension_core Threads::Threads)
    add_test(NAME guarded_suspension_tests COMMAND guarded_suspension_tests)
endif()
