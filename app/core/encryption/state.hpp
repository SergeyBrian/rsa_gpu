#ifndef RSA_GPU_STATE_HPP
#define RSA_GPU_STATE_HPP

#define BLOCK_SIZE 16
#define SECTION_SIZE 16

#define R 4
#define Nb 4
#define Nk 8
#define Nr 14

#include "math_utils.hpp"

namespace encryption {
    using State = byte[R][Nb];
}

#endif //RSA_GPU_STATE_HPP
