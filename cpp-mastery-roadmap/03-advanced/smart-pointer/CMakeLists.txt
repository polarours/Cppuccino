cmake_minimum_required(VERSION 3.16)

project(smart_pointer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(smart_ptr_core INTERFACE)
target_include_directories(smart_ptr_core INTERFACE include)

add_executable(smart_ptr_example src/main.cpp)
target_link_libraries(smart_ptr_example PRIVATE smart_ptr_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(smart_ptr_tests tests/smart_ptr_tests.cpp)
    target_link_libraries(smart_ptr_tests PRIVATE smart_ptr_core)
    add_test(NAME smart_ptr_tests COMMAND smart_ptr_tests)
endif()
