#ifndef RSA_AES_HPP
#define RSA_AES_HPP


#include <vector>

#include "cpu/aescpu_backend.hpp"
#include "gpu/aesgpu_backend.hpp"
#include "../math_utils.hpp"
#include "../key.hpp"
#include "../state.hpp"

namespace encryption::aes {
    class AES {
        AESBackend *backend;
    public:
        explicit AES(bool use_gpu);

        byte *encrypt(Key *key,
                      const byte *input,
                      size_t size);

        void XOR(byte *a, const byte *b, size_t size);
    };
}


#endif //RSA_AES_HPP
