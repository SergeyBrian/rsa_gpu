#ifndef RSA_GPU_MATH_UTILS_HPP
#define RSA_GPU_MATH_UTILS_HPP

#ifdef WIN32

#include <cstdint>

#endif

#define DIV_UP(x, y) ((x + y - 1) / y)
#define MIN(x, y) ((x < y) ? x : y)

using uint64 = uint64_t;
using uint32 = uint32_t;
using uint16 = uint16_t;
using uint8 = uint8_t;
using byte = unsigned char;
using word = unsigned int;

inline auto operator ""_GB(size_t const x) { return 1024 * 1024 * 1024 * x; }

namespace encryption::math {
    void XOR(const byte *a, byte *b, size_t size);
}

#endif //RSA_GPU_MATH_UTILS_HPP
