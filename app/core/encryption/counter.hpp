#ifndef RSA_GPU_COUNTER_HPP
#define RSA_GPU_COUNTER_HPP

#include <string>

#include "math_utils.hpp"
#include "state.hpp"

namespace encryption {
    class Counter {
        byte value[SECTION_SIZE]{};
    public:
        Counter();

        byte *get_value();
    };
}

#endif //RSA_GPU_COUNTER_HPP
