#ifndef RSA_GPU_AESGPU_BACKEND_HPP
#define RSA_GPU_AESGPU_BACKEND_HPP

#define CL_HPP_ENABLE_EXCEPTIONS

#include <CL/opencl.hpp>
#include <map>

#include "../aes_backend.hpp"
#include "../../state.hpp"
#include "opencl_loader.hpp"

class AESGPUBackend : public encryption::aes::AESBackend {
    enum KFunc {
        KF_LOAD_STATES,
        KF_UNLOAD_STATES,
        KF_XOR,
        KF_KEY_XOR,
        KF_SUB_BYTES,
        KF_SHIFT_ROWS,
        KF_MIX_COLUMNS,
        KF_COUNT,
    };

    const char *KFuncName[KF_COUNT] = {
            "loadStates",
            "unloadStates",
            "XOR",
            "KeyXOR",
            "subBytes",
            "shiftRows",
            "mixColumns",
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

    void load_states(const byte *input, size_t size);

    void sub_bytes(const cl::Buffer &bytes, size_t size);

    void shift_rows(const cl::Buffer &bytes, size_t size, int len);

    byte *unload_states(size_t size);

    void mix_columns(const cl::Buffer &bytes, size_t size);

    void XOR(const cl::Buffer &a, const cl::Buffer &b, size_t size);

    void KeyXOR(const cl::Buffer &bytes,
                const cl::Buffer &key,
                int offset,
                size_t size,
                size_t key_size = SECTION_SIZE);

public:
    AESGPUBackend();

    byte *encrypt(encryption::Key *key,
                  const byte *input,
                  size_t size) override;

    void XOR(byte *a, const byte *b, size_t size) override;
};

#endif //RSA_GPU_AESGPU_BACKEND_HPP
