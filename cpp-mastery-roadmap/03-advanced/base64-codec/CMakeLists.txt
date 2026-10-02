cmake_minimum_required(VERSION 3.16)

project(base64_codec LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(base64_codec_core src/base64_codec.cpp)
target_include_directories(base64_codec_core PUBLIC include)

add_executable(base64_codec_example src/main.cpp)
target_link_libraries(base64_codec_example PRIVATE base64_codec_core)

enable_testing()

if(BUILD_TESTING)
    add_executable(base64_codec_tests tests/base64_codec_tests.cpp)
    target_link_libraries(base64_codec_tests PRIVATE base64_codec_core)
    add_test(NAME base64_codec_tests COMMAND base64_codec_tests)
endif()
