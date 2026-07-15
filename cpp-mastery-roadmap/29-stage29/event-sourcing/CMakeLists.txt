cmake_minimum_required(VERSION 3.16)

project(event_sourcing LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(event_sourcing_core INTERFACE)
target_include_directories(event_sourcing_core INTERFACE include)

add_executable(event_sourcing_example src/main.cpp)
target_link_libraries(event_sourcing_example PRIVATE event_sourcing_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(event_sourcing_tests tests/event_sourcing_tests.cpp)
    target_link_libraries(event_sourcing_tests PRIVATE event_sourcing_core)
    add_test(NAME event_sourcing_tests COMMAND event_sourcing_tests)
endif()
