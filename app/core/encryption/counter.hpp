#ifndef RSA_GPU_COUNTER_HPP
#define RSA_GPU_COUNTER_HPP

#define NONCE_SIZE 12

#include <stdexcept>
#include <string>
#include <vector>

#include "math_utils.hpp"
#include "state.hpp"

namespace encryption::counter {
    class Counter {
        unsigned long long iteration;
        unsigned long long buff_size;
        unsigned long long block_count;
        byte *counters;
    public:
        Counter(unsigned long long block_count,
                unsigned long long buff_size,
                const std::string &init_str = "I SUCK DICKS");

        byte **getCounters();
    };
}

#endif //RSA_GPU_COUNTER_HPP
