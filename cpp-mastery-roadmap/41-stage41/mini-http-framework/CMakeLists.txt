cmake_minimum_required(VERSION 3.16)

project(mini_http_framework LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(mini_http_core INTERFACE)
target_include_directories(mini_http_core INTERFACE include)

add_executable(mini_http_example examples/main.cpp)
target_link_libraries(mini_http_example PRIVATE mini_http_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(framework_tests tests/framework_tests.cpp)
    target_link_libraries(framework_tests PRIVATE mini_http_core)
    add_test(NAME framework_tests COMMAND framework_tests)
endif()
