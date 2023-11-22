#ifndef RSA_GPU_AES_BACKEND_HPP
#define RSA_GPU_AES_BACKEND_HPP


#include <vector>
#include "../math_utils.hpp"
#include "../key.hpp"

namespace encryption::aes {
    class AESBackend {
    public:
        virtual byte *encrypt(Key *key,
                              const byte *input,
                              size_t size) = 0;

        virtual void XOR(byte *a, const byte *b, size_t size) = 0;
    };
}


#endif //RSA_GPU_AES_BACKEND_HPP
