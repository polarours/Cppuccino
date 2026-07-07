cmake_minimum_required(VERSION 3.16)

project(simple_db LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(simple_db_core src/simple_db.cpp)
target_include_directories(simple_db_core PUBLIC include)

add_executable(simple_db_example src/main.cpp)
target_link_libraries(simple_db_example PRIVATE simple_db_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(simple_db_tests tests/simple_db_tests.cpp)
    target_link_libraries(simple_db_tests PRIVATE simple_db_core)
    add_test(NAME simple_db_tests COMMAND simple_db_tests)
endif()
