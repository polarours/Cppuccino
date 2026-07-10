cmake_minimum_required(VERSION 3.16)

project(chain_of_responsibility LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(chain_of_responsibility_core INTERFACE)
target_include_directories(chain_of_responsibility_core INTERFACE include)

add_executable(chain_of_responsibility_example src/main.cpp)
target_link_libraries(chain_of_responsibility_example PRIVATE chain_of_responsibility_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(chain_of_responsibility_tests tests/chain_of_responsibility_tests.cpp)
    target_link_libraries(chain_of_responsibility_tests PRIVATE chain_of_responsibility_core)
    add_test(NAME chain_of_responsibility_tests COMMAND chain_of_responsibility_tests)
endif()
