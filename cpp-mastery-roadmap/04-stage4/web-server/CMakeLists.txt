cmake_minimum_required(VERSION 3.16)

project(web_server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_library(web_server_core src/web_server.cpp)
target_include_directories(web_server_core PUBLIC include)
target_link_libraries(web_server_core PUBLIC Threads::Threads)

add_executable(web_server_example src/main.cpp)
target_link_libraries(web_server_example PRIVATE web_server_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(web_server_tests tests/web_server_tests.cpp)
    target_link_libraries(web_server_tests PRIVATE web_server_core)
    add_test(NAME web_server_tests COMMAND web_server_tests)
endif()
