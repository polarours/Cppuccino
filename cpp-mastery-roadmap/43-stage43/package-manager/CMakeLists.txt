cmake_minimum_required(VERSION 3.16)

project(package_manager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(package_manager_core INTERFACE)
target_include_directories(package_manager_core INTERFACE include)

add_executable(package_manager_example src/main.cpp)
target_link_libraries(package_manager_example PRIVATE package_manager_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(package_manager_tests tests/package_manager_tests.cpp)
    target_link_libraries(package_manager_tests PRIVATE package_manager_core)
    add_test(NAME package_manager_tests COMMAND package_manager_tests)
endif()
