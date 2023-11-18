#ifndef RSA_GPU_MATH_UTILS_HPP
#define RSA_GPU_MATH_UTILS_HPP

#ifdef WIN32

#include <cstdint>

#endif

using uint64 = uint64_t;
using uint32 = uint32_t;
using uint16 = uint16_t;
using uint8 = uint8_t;
using byte = unsigned char;
using word = unsigned long long;

namespace encryption::math {
    void XOR(const byte *a, byte *b, size_t size);
}

#endif //RSA_GPU_MATH_UTILS_HPP
