#ifndef RSA_GPU_KEY_HPP
#define RSA_GPU_KEY_HPP

#define KEYSIZE 32

#include <fstream>

#include "math_utils.hpp"
#include "state.hpp"

namespace encryption {
    class Key {
    public:
        Key();

        explicit Key(std::ifstream &file);

        byte *expand();

        byte value[KEYSIZE] = {};
    };
}

#endif //RSA_GPU_KEY_HPP
