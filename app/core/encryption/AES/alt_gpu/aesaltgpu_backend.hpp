#ifndef RSA_GPU_AESALTGPU_BACKEND_HPP
#define RSA_GPU_AESALTGPU_BACKEND_HPP

#define CL_HPP_ENABLE_EXCEPTIONS
#ifdef _WIN32

#include <CL/opencl.hpp>

#elif __APPLE__
#include "../../../../libs/opencl/opencl.hpp"
#endif

#include <map>

#include "../aes_backend.hpp"
#include "../../state.hpp"
#include "../gpu/opencl_loader.hpp"

class AESALTGPUBackend : public encryption::aes::AESBackend {
    enum KFunc {
        KF_ENCRYPT,
        KF_XOR,
        KF_COUNT,
    };

    const char *KFuncName[KF_COUNT] = {
            "encrypt",
            "XOR",
    };

    cl::Platform platform;
    cl::Device device;
    cl::Context context;
    cl::CommandQueue command_queue;
    std::map<KFunc, cl::Kernel> kernel;
    cl::Program program;

    cl::Buffer SBox;
    cl::Buffer InvSBox;
    cl::Buffer RCon;
    cl::Buffer RoundKeys;
    cl::Buffer GF28;

    cl::Buffer states;

    cl::Buffer tmp_row;
    cl::Buffer tmp_col;

    byte *apply_aes(const byte *input, size_t size);

    void XOR(const cl::Buffer &a, const cl::Buffer &b, size_t size);

public:
    AESALTGPUBackend();

    byte *encrypt(encryption::Key *key,
                  const byte *input,
                  size_t size) override;

    void XOR(byte *a, const byte *b, size_t size) override;
};

#endif //RSA_GPU_AESALTGPU_BACKEND_HPP
