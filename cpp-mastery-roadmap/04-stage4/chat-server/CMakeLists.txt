cmake_minimum_required(VERSION 3.16)

project(chat_server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_library(chat_server_core src/chat_server.cpp)
target_include_directories(chat_server_core PUBLIC include)
target_link_libraries(chat_server_core PUBLIC Threads::Threads)

add_executable(chat_server_example src/main.cpp)
target_link_libraries(chat_server_example PRIVATE chat_server_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(chat_server_tests tests/chat_server_tests.cpp)
    target_link_libraries(chat_server_tests PRIVATE chat_server_core)
    add_test(NAME chat_server_tests COMMAND chat_server_tests)
endif()
