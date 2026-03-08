cmake_minimum_required(VERSION 3.16)

project(tiny_http_server)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCES
    src/*.cpp
)

add_executable(tiny_http_server ${SOURCES})