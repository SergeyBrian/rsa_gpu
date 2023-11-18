#ifndef RSA_GPU_ENCRYPTOR_HPP
#define RSA_GPU_ENCRYPTOR_HPP

#define BLOCK_SIZE 16
#define SECTION_SIZE 16

#define R 4
#define Nb 4
#define Nk 8
#define Nr 14

#include "key.hpp"

namespace encryption {
    class IEncryptor {
    public:
        virtual byte *encrypt(Key *key, size_t size, byte *input) = 0;

        virtual byte *decrypt(Key *key, byte *input) = 0;
    };
}

#endif //RSA_GPU_ENCRYPTOR_HPP
