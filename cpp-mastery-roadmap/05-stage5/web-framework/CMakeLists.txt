cmake_minimum_required(VERSION 3.16)

project(web_framework LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_library(web_framework_core src/web_framework.cpp)
target_include_directories(web_framework_core PUBLIC include)
target_link_libraries(web_framework_core PUBLIC Threads::Threads)

add_executable(web_framework_example src/main.cpp)
target_link_libraries(web_framework_example PRIVATE web_framework_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(web_framework_tests tests/web_framework_tests.cpp)
    target_link_libraries(web_framework_tests PRIVATE web_framework_core)
    add_test(NAME web_framework_tests COMMAND web_framework_tests)
endif()
