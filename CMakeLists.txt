cmake_minimum_required(VERSION 3.16)
project(mymuduo)

set(CMAKE_CXX_STANDARD 17)

link_directories(lib)
include_directories(src)
add_subdirectory(src/muduo/base)
add_subdirectory(src/muduo/net)
set(CMAKE_BUILD_NO_EXAMPLES 1)
add_executable(mymuduo main.cpp src/chatserver.hpp)

target_link_libraries(mymuduo muduo_net)
target_link_libraries(mymuduo muduo_base)
#
target_link_libraries(mymuduo pthread)