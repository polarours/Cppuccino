cmake_minimum_required(VERSION 3.16)

project(trie LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(trie_core src/trie.cpp)
target_include_directories(trie_core PUBLIC include)

add_executable(trie_example src/main.cpp)
target_link_libraries(trie_example PRIVATE trie_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(trie_tests tests/trie_tests.cpp)
    target_link_libraries(trie_tests PRIVATE trie_core)
    add_test(NAME trie_tests COMMAND trie_tests)
endif()
