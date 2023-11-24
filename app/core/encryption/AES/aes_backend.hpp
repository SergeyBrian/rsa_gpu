#ifndef RSA_GPU_AES_BACKEND_HPP
#define RSA_GPU_AES_BACKEND_HPP


#include <vector>
#include "../math_utils.hpp"
#include "../key.hpp"

namespace encryption::aes {
    extern const byte SBox[256];
    extern const byte InvSBox[256];
    extern const byte RCon[11][4];
    extern const byte GF28[4][4];

    class AESBackend {
    public:
        virtual byte *encrypt(Key *key,
                              const byte *input,
                              size_t size) = 0;

        virtual void XOR(byte *a, const byte *b, size_t size) = 0;
    };
}


#endif //RSA_GPU_AES_BACKEND_HPP
