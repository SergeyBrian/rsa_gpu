#ifndef RSA_GPU_ENCRYPTOR_HPP
#define RSA_GPU_ENCRYPTOR_HPP

#include "key.hpp"
#include "state.hpp"

namespace encryption {
    class IEncryptor {
    public:
        virtual byte *encrypt(Key *key, size_t size, byte *input) = 0;

        virtual byte *decrypt(Key *key, byte *input) = 0;

    protected:
        static byte *with_additional_bytes(byte *data, size_t size, size_t *new_size);

        static State *get_states(byte *input, size_t size, size_t *additional_bytes, int *states_count);
    };
}

#endif //RSA_GPU_ENCRYPTOR_HPP
