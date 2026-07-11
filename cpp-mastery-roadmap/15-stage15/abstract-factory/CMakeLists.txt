cmake_minimum_required(VERSION 3.16)

project(abstract_factory LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(abstract_factory_core INTERFACE)
target_include_directories(abstract_factory_core INTERFACE include)

add_executable(abstract_factory_example src/main.cpp)
target_link_libraries(abstract_factory_example PRIVATE abstract_factory_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(abstract_factory_tests tests/abstract_factory_tests.cpp)
    target_link_libraries(abstract_factory_tests PRIVATE abstract_factory_core)
    add_test(NAME abstract_factory_tests COMMAND abstract_factory_tests)
endif()
