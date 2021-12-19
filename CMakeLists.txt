cmake_minimum_required(VERSION 3.0)
project(Lab2)

set(CMAKE_CXX_STANDARD 20)

include_directories(coding)

add_executable(max_coder main.cpp)

add_subdirectory(coding/BlockHuffman)

add_library(encode STATIC coding/coding.h)

target_link_libraries(encode huffman_coder)

target_link_libraries(max_coder encode)

configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download)
if (result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif ()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download)
if (result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif ()

add_subdirectory(
        ${CMAKE_CURRENT_BINARY_DIR}/googletest-src
        ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
        EXCLUDE_FROM_ALL
)

if (NOT MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-sign-compare -pedantic")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=undefined,address,leak -fno-sanitize-recover=all -D_GLIBCXX_DEBUG")
endif()

set(BASE_TESTS_SOURCES tests.cpp)
add_executable(base-tests ${BASE_TESTS_SOURCES})
target_link_libraries(base-tests gtest_main)
