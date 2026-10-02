cmake_minimum_required(VERSION 3.16)

project(actor_mailbox LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_library(actor_mailbox_core INTERFACE)
target_include_directories(actor_mailbox_core INTERFACE include)
target_link_libraries(actor_mailbox_core INTERFACE Threads::Threads)

add_executable(actor_mailbox_example src/main.cpp)
target_link_libraries(actor_mailbox_example PRIVATE actor_mailbox_core Threads::Threads)

enable_testing()

if(BUILD_TESTING)
    add_executable(actor_mailbox_tests tests/actor_mailbox_tests.cpp)
    target_link_libraries(actor_mailbox_tests PRIVATE actor_mailbox_core Threads::Threads)
    add_test(NAME actor_mailbox_tests COMMAND actor_mailbox_tests)
endif()
