cmake_minimum_required(VERSION 3.23)
project(rsa_hpu CUDA)

set(CMAKE_CUDA_STANDARD 17)


add_executable(rsa_hpu main.cu main.cpp)

set_target_properties(rsa_hpu PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON)
