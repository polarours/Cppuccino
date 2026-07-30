cmake_minimum_required(VERSION 3.16)

project(reference_binding_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(reference_binding_demo
    main.cpp
)

enable_testing()

if(BUILD_TESTING)
    add_executable(ref_bind_tests
        tests/ref_bind_tests.cpp
    )
    add_test(NAME ref_bind_tests COMMAND ref_bind_tests)
endif()
