cmake_minimum_required(VERSION 3.16)
project(mymuduo)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)
add_subdirectory(src)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

