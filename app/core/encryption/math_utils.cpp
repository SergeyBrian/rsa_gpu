#include "math_utils.hpp"

void encryption::math::XOR(const byte *a, byte *b, size_t size) {
    for (int i = 0; i < size; i++) {
        b[i] ^= a[i];
    }
}
