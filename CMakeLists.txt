cmake_minimum_required(VERSION 3.0)
project(Lab2)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -mtune=native")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address,undefined -D_GLIBCXX_DEBUG")

include_directories(${CMAKE_SOURCE_DIR}/coding)

add_executable(max_coder main.cpp coding/RANS/rans.h coding/RANS/rans_adaptive.h)

add_subdirectory(coding/BlockHuffman)

target_link_libraries(max_coder huffman_coder)
