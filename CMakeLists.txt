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
        app/core/encryption/state.cpp
        app/core/encryption/state.hpp
        app/core/encryption/AES/gpu/opencl_loader.cpp
        app/core/encryption/AES/gpu/opencl_loader.hpp
        app/core/encryption/counter.hpp
        app/core/encryption/counter.cpp
        app/core/encryption/AES/aes.cpp
        app/core/encryption/AES/aes.hpp
        app/core/encryption/AES/gpu/aesgpu_backend.cpp
        app/core/encryption/AES/gpu/aesgpu_backend.hpp
        app/core/encryption/AES/cpu/aescpu_backend.cpp
        app/core/encryption/AES/cpu/aescpu_backend.hpp
        app/core/encryption/AES/aes_backend.cpp
        app/core/encryption/AES/aes_backend.hpp
)
target_link_libraries(rsa_gpu OpenCL)

file(GLOB_RECURSE CL_FILES "${CMAKE_SOURCE_DIR}/*.cl")

foreach (CL_FILE ${CL_FILES})
    get_filename_component(CL_FILENAME ${CL_FILE} NAME)

    add_custom_command(TARGET rsa_gpu POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${CL_FILE} ${CMAKE_CFG_INTDIR}/${CL_FILENAME})
endforeach ()
