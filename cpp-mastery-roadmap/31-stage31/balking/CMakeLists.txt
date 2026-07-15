cmake_minimum_required(VERSION 3.16)

project(balking LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_library(balking_core INTERFACE)
target_include_directories(balking_core INTERFACE include)
target_link_libraries(balking_core INTERFACE Threads::Threads)

add_executable(balking_example src/main.cpp)
target_link_libraries(balking_example PRIVATE balking_core Threads::Threads)

enable_testing()

if(BUILD_TESTING)
    add_executable(balking_tests tests/balking_tests.cpp)
    target_link_libraries(balking_tests PRIVATE balking_core Threads::Threads)
    add_test(NAME balking_tests COMMAND balking_tests)
endif()
