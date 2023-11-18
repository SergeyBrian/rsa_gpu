#ifndef RSA_GPU_KEY_HPP
#define RSA_GPU_KEY_HPP

#define KEYSIZE 32

#include "math_utils.hpp"

namespace encryption {
    class Key {
    public:
        Key();

        byte value[KEYSIZE] = {};
    };
}

#endif //RSA_GPU_KEY_HPP
