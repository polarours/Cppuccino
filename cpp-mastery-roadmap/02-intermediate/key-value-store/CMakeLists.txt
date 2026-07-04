cmake_minimum_required(VERSION 3.16)

project(key_value_store)

include(CTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(KEY_VALUE_STORE_CORE_SOURCES
    src/command/command_processor.cpp
    src/persistence/store_persistence.cpp
    src/store/key_value_store.cpp
    src/store/store_manager.cpp
    src/protocol/protocol.cpp
    src/server/server.cpp
    src/client/client.cpp
)

add_library(key_value_store_core ${KEY_VALUE_STORE_CORE_SOURCES})
target_include_directories(key_value_store_core PUBLIC src)

find_package(Threads REQUIRED)

add_executable(key_value_store src/main.cpp)
target_link_libraries(key_value_store PRIVATE key_value_store_core Threads::Threads)

add_executable(kv_client src/client_main.cpp)
target_link_libraries(kv_client PRIVATE key_value_store_core Threads::Threads)

if(BUILD_TESTING)
    add_executable(key_value_store_tests tests/key_value_store_tests.cpp)
    target_link_libraries(key_value_store_tests PRIVATE key_value_store_core Threads::Threads)
    add_test(NAME key_value_store_tests COMMAND key_value_store_tests)
endif()
