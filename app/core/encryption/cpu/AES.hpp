#ifndef RSA_GPU_AES_HPP
#define RSA_GPU_AES_HPP

#include "../key.hpp"
#include "../state.hpp"

namespace encryption {
    byte *encrypt_section(Key *key, byte *input);

    byte *decrypt_section(Key *key, byte *input);
}

#endif //RSA_GPU_AES_HPP
