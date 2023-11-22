#ifndef RSA_GPU_AESGPU_BACKEND_HPP
#define RSA_GPU_AESGPU_BACKEND_HPP

#define CL_HPP_ENABLE_EXCEPTIONS

#include <CL/opencl.hpp>
#include <map>

#include "../aes_backend.hpp"
#include "opencl_loader.hpp"

class AESGPUBackend : public encryption::aes::AESBackend {
    enum KFunc {
        KF_DOUBLE,
        KF_XOR,
        KF_COUNT,
    };

    const char *KFuncName[KF_COUNT] = {
            "doubleMatrix",
            "XOR",
    };

    cl::Platform platform;
    cl::Device device;
    cl::Context context;
    cl::CommandQueue command_queue;
    std::map<KFunc, cl::Kernel> kernel;
    cl::Program program;
public:
    AESGPUBackend();

    byte *encrypt(encryption::Key *key,
                  const byte *input,
                  size_t size) override;

    void XOR(byte *a, const byte *b, size_t size) override;
};

#endif //RSA_GPU_AESGPU_BACKEND_HPP
