cmake_minimum_required(VERSION 3.16)

project(service_locator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(service_locator_core INTERFACE)
target_include_directories(service_locator_core INTERFACE include)

add_executable(service_locator_example src/main.cpp)
target_link_libraries(service_locator_example PRIVATE service_locator_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(service_locator_tests tests/service_locator_tests.cpp)
    target_link_libraries(service_locator_tests PRIVATE service_locator_core)
    add_test(NAME service_locator_tests COMMAND service_locator_tests)
endif()
