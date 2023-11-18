#ifndef RSA_GPU_GPUENCRYPTOR_HPP
#define RSA_GPU_GPUENCRYPTOR_HPP

#define CL_HPP_ENABLE_EXCEPTIONS

#include <CL/opencl.hpp>

#include "../encryptor.hpp"
#include "opencl_loader.hpp"

namespace encryption {
    class GPUEncryptor : public IEncryptor {
        cl::Platform platform;
        cl::Device device;
        cl::Context context;
        cl::CommandQueue command_queue;

        cl::Program AES_encrypt_program;
        cl::Program AES_decrypt_program;

        std::string AES_encrypt_source;
        std::string AES_decrypt_source;
    public:
        GPUEncryptor();

        byte *encrypt(Key *key, size_t size, byte *input) override;

        byte *decrypt(Key *key, byte *input) override;
    };
}

#endif //RSA_GPU_GPUENCRYPTOR_HPP
