cmake_minimum_required(VERSION 3.16)

project(tiny_http_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES
    src/*.cpp
)

add_executable(tiny_http_server ${SOURCES})

target_include_directories(tiny_http_server PRIVATE src)