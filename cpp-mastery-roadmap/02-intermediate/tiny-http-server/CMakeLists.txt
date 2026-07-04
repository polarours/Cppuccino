cmake_minimum_required(VERSION 3.16)

project(tiny_http_server)

include(CTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TINY_HTTP_SERVER_CORE_SOURCES
    src/http/request.cpp
    src/http/response.cpp
    src/router/router.cpp
    src/server/server.cpp
    src/thread-pool/thread_pool.cpp
)

add_library(tiny_http_server_core ${TINY_HTTP_SERVER_CORE_SOURCES})
target_include_directories(tiny_http_server_core PUBLIC src)

add_executable(tiny_http_server src/main.cpp)
target_link_libraries(tiny_http_server PRIVATE tiny_http_server_core)

if(BUILD_TESTING)
    add_executable(tiny_http_server_tests tests/http_components_test.cpp)
    target_link_libraries(tiny_http_server_tests PRIVATE tiny_http_server_core)
    add_test(NAME tiny_http_server_tests COMMAND tiny_http_server_tests)
endif()
