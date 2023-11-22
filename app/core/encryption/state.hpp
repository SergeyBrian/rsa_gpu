#ifndef RSA_GPU_STATE_HPP
#define RSA_GPU_STATE_HPP

#define SECTION_SIZE 16

#define ROWS 4
#define COLS 4
#define ROUNDS_COUNT 14

#include "math_utils.hpp"

namespace encryption {
    using State = byte[ROWS][COLS];
}

#endif //RSA_GPU_STATE_HPP
