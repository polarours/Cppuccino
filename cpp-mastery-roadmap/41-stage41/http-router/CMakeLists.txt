cmake_minimum_required(VERSION 3.16)

project(http_router LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(http_router_core src/http_router.cpp)
target_include_directories(http_router_core PUBLIC include)

add_executable(http_router_example src/main.cpp)
target_link_libraries(http_router_example PRIVATE http_router_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(http_router_tests tests/http_router_tests.cpp)
    target_link_libraries(http_router_tests PRIVATE http_router_core)
    add_test(NAME http_router_tests COMMAND http_router_tests)
endif()
