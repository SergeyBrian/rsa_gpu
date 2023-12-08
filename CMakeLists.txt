cmake_minimum_required(VERSION 3.23)
project(rsa_gpu)

set(CMAKE_CXX_STANDARD 20)

if(WIN32)
    include_directories("C:\\OpenCL-SDK\\install\\include")
    link_directories("C:\\OpenCL-SDK\\install\\lib")
else()
    find_package(OpenCL REQUIRED)
    include_directories( ${OPENCL_INCLUDE_DIR})
    set(FILE_URL "https://raw.githubusercontent.com/KhronosGroup/OpenCL-CLHPP/main/include/CL/opencl.hpp")

    set(DOWNLOAD_PATH "${CMAKE_SOURCE_DIR}/app/libs/opencl/opencl.hpp")

    message("Downloading opencl hpp headers to ${DOWNLOAD_PATH}")
    file(DOWNLOAD ${FILE_URL} ${DOWNLOAD_PATH}
            STATUS download_status
            SHOW_PROGRESS)

    list(GET download_status 0 status_code)
    list(GET download_status 1 status_string)
    if(NOT status_code EQUAL 0)
        message(FATAL_ERROR "Error downloading file: ${status_string}")
    endif()
endif()

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
        app/libs/opencl/opencl.hpp
        app/core/encryption/AES/alt_gpu/aesaltgpu_backend.cpp
        app/core/encryption/AES/alt_gpu/aesaltgpu_backend.hpp
)
if(WIN32)
    target_link_libraries(rsa_gpu OpenCL)
else()
    target_link_libraries(rsa_gpu "-framework OpenCL")
endif()

if(APPLE)
    add_compile_definitions(CL_HPP_MINIMUM_OPENCL_VERSION=100)
    add_compile_definitions(CL_HPP_TARGET_OPENCL_VERSION=100)
endif()

file(GLOB_RECURSE CL_FILES "${CMAKE_SOURCE_DIR}/*.cl")

foreach (CL_FILE ${CL_FILES})
    get_filename_component(CL_FILENAME ${CL_FILE} NAME)

    add_custom_command(TARGET rsa_gpu POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${CL_FILE} ${CMAKE_CFG_INTDIR}/${CL_FILENAME})
endforeach ()
