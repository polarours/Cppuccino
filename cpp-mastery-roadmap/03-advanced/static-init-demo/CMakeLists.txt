cmake_minimum_required(VERSION 3.16)

project(static_init_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(static_init_demo
    src/main.cpp
    src/bad_demo.cpp
    src/counter.cpp
    include/counter.hpp
)

target_include_directories(static_init_demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

enable_testing()

if(BUILD_TESTING)
add_executable(static_init_tests
    tests/static_init_tests.cpp
    src/bad_demo.cpp
    src/counter.cpp
    include/counter.hpp
)
    target_include_directories(static_init_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    add_test(NAME static_init_tests COMMAND static_init_tests)
endif()
