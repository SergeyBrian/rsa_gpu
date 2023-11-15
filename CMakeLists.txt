cmake_minimum_required(VERSION 3.23)
project(rsa_gpu)

set(CMAKE_CXX_STANDARD 20)

include_directories("C:\\OpenCL-SDK\\install\\include")
link_directories("C:\\OpenCL-SDK\\install\\lib")

add_executable(rsa_gpu app/main.cpp app/libs/cargs/cargs.h app/libs/cargs/cargs.c)
target_link_libraries(rsa_gpu OpenCL)
