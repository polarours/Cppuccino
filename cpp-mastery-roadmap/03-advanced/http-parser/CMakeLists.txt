cmake_minimum_required(VERSION 3.16)

project(http_parser LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(http_core src/http.cpp)
target_include_directories(http_core PUBLIC include)

add_executable(http_example src/main.cpp)
target_link_libraries(http_example PRIVATE http_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(http_tests tests/http_tests.cpp)
    target_link_libraries(http_tests PRIVATE http_core)
    add_test(NAME http_tests COMMAND http_tests)
endif()
