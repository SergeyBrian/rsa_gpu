cmake_minimum_required(VERSION 3.23)
project(rsa_gpu)

set(CMAKE_CXX_STANDARD 20)

include_directories("C:\\OpenCL-SDK\\install\\include")
link_directories("C:\\OpenCL-SDK\\install\\lib")

add_executable(rsa_gpu
        app/main.cpp
        app/core/encryption/encryptor.hpp
        app/core/encryption/key.cpp
        app/core/encryption/key.hpp
        app/libs/cargs/cargs.h
        app/libs/cargs/cargs.c
        app/libs/biginteger/integer.h
        app/core/encryption/math_utils.cpp
        app/core/encryption/math_utils.hpp
        app/core/encryption/cpu/CPUEncryptor.cpp
        app/core/encryption/cpu/CPUEncryptor.hpp)
target_link_libraries(rsa_gpu OpenCL)
