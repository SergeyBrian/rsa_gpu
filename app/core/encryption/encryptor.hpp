#ifndef RSA_GPU_ENCRYPTOR_HPP
#define RSA_GPU_ENCRYPTOR_HPP

#include "key.hpp"
#include "state.hpp"
#include "AES/aes.hpp"
#include "counter.hpp"

namespace encryption {
    class Encryptor {
        aes::AES *aes;
        counter::Counter *counter;
    public:
        Encryptor(bool use_gpu, unsigned long long block_count, size_t buff_size);

        void apply(Key *key, size_t size, byte *input);
    };
}

#endif //RSA_GPU_ENCRYPTOR_HPP
