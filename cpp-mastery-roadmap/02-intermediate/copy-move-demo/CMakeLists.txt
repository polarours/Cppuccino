cmake_minimum_required(VERSION 3.16)

project(copy_move_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(copy_move_demo
    main.cpp
)

target_include_directories(copy_move_demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

enable_testing()

if(BUILD_TESTING)
add_executable(copy_move_tests
    tests/demo_tests.cpp
)
    target_include_directories(copy_move_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    add_test(NAME copy_move_tests COMMAND copy_move_tests)
endif()
