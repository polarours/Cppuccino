cmake_minimum_required(VERSION 3.16)

project(bilingual_doc_checker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(bilingual_doc_checker_core src/bilingual_doc_checker.cpp)
target_include_directories(bilingual_doc_checker_core PUBLIC include)

add_executable(bilingual_doc_checker_example src/main.cpp)
target_link_libraries(bilingual_doc_checker_example PRIVATE bilingual_doc_checker_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(bilingual_doc_checker_tests tests/bilingual_doc_checker_tests.cpp)
    target_link_libraries(bilingual_doc_checker_tests PRIVATE bilingual_doc_checker_core)
    add_test(NAME bilingual_doc_checker_tests COMMAND bilingual_doc_checker_tests)
endif()
