cmake_minimum_required(VERSION 3.16)

project(active_record LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(active_record_core INTERFACE)
target_include_directories(active_record_core INTERFACE include)

add_executable(active_record_example src/main.cpp)
target_link_libraries(active_record_example PRIVATE active_record_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(active_record_tests tests/active_record_tests.cpp)
    target_link_libraries(active_record_tests PRIVATE active_record_core)
    add_test(NAME active_record_tests COMMAND active_record_tests)
endif()
