cmake_minimum_required(VERSION 3.16)

project(student_management_v2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(student_management_v2
    main.cpp
    student.cpp
    student_manager.cpp
)

target_include_directories(student_management_v2 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

enable_testing()

if(BUILD_TESTING)
    add_executable(student_management_tests
        tests/student_tests.cpp
        student.cpp
        student_manager.cpp
    )
    target_include_directories(student_management_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    add_test(NAME student_management_tests COMMAND student_management_tests)
endif()
