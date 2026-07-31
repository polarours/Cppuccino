cmake_minimum_required(VERSION 3.16)

project(consumer_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(producer_consumer_demo
    main.cpp
)

target_include_directories(producer_consumer_demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../ring-buffer/include)

enable_testing()

if(BUILD_TESTING)
    add_executable(pcon_tests
        tests/pcon_tests.cpp
    )
    target_include_directories(pcon_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../ring-buffer/include)
    add_test(NAME pcon_tests COMMAND pcon_tests)
endif()
