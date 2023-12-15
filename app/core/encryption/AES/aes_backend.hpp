#ifndef RSA_GPU_AES_BACKEND_HPP
#define RSA_GPU_AES_BACKEND_HPP


#include <vector>
#include "../math_utils.hpp"
#include "../key.hpp"

namespace encryption::aes {
    enum gpu_mode {
        CPU,
        DEFAULT,
        PARALLEL,
    };

    extern const byte SBox[16][16];
    extern const byte InvSBox[16][16];
    extern const byte RCon[15];
    extern const byte GF28[4][4];
    extern const byte InvGF28[4][4];
    extern const byte GF_MUL_TABLE[15][256];
    extern const byte CMDS[4][4];
    extern const byte INV_CMDS[4][4];


    class AESBackend {
    public:
        virtual byte *encrypt(Key *key,const byte *input,size_t size) = 0;

        virtual byte *decrypt(Key *key,const byte *input,size_t size) = 0;

        virtual void XOR(byte *a, const byte *b, size_t size) = 0;

    };
}


#endif //RSA_GPU_AES_BACKEND_HPP
