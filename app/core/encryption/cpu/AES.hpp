#ifndef RSA_GPU_AES_HPP
#define RSA_GPU_AES_HPP

#include "../key.hpp"
#include "../state.hpp"

namespace encryption {
    void apply_xor(Key *key, State *state);
}

#endif //RSA_GPU_AES_HPP
