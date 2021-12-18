cmake_minimum_required(VERSION 3.0)
project(coder)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -mtune=native")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address,undefined -D_GLIBCXX_DEBUG")

add_executable(coder main.cpp)

add_executable(entropy main1_entropy.cpp)