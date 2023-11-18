cmake_minimum_required(VERSION 3.23)
project(rsa_gpu)

set(CMAKE_CXX_STANDARD 20)

include_directories("C:\\OpenCL-SDK\\install\\include")
link_directories("C:\\OpenCL-SDK\\install\\lib")

add_executable(rsa_gpu
        app/main.cpp
        app/core/encryption/encryptor.cpp
        app/core/encryption/encryptor.hpp
        app/core/encryption/key.cpp
        app/core/encryption/key.hpp
        app/libs/cargs/cargs.h
        app/libs/cargs/cargs.c
        app/core/encryption/math_utils.cpp
        app/core/encryption/math_utils.hpp
        app/core/encryption/cpu/CPUEncryptor.cpp
        app/core/encryption/cpu/CPUEncryptor.hpp
        app/core/encryption/gpu/GPUEncryptor.cpp
        app/core/encryption/gpu/GPUEncryptor.hpp
        app/core/encryption/cpu/AES.cpp
        app/core/encryption/cpu/AES.hpp
        app/core/encryption/state.cpp
        app/core/encryption/state.hpp
        app/core/encryption/gpu/opencl_loader.cpp
        app/core/encryption/gpu/opencl_loader.hpp
        app/core/encryption/counter.hpp
        app/core/encryption/counter.cpp
)
target_link_libraries(rsa_gpu OpenCL)
