#include "counter.hpp"

namespace encryption::counter {
    Counter::Counter(unsigned long long block_count,
                     unsigned long long buff_size,
                     const std::string &init_str) {
        unsigned long long blocks_per_buff = buff_size / SECTION_SIZE;

        if (buff_size % 16) {
            throw std::runtime_error("Buffer is not divisible by 16");
        }

        if (!blocks_per_buff) {
            throw std::runtime_error("Buffer too small");
        }

        this->block_count = block_count;
        this->buff_size = buff_size;
        this->iteration = 0;
        counters = new byte[blocks_per_buff * SECTION_SIZE];
        std::memcpy(counters, init_str.c_str(), NONCE_SIZE);
    }


    byte **Counter::getCounters() {
        if (iteration >= block_count) return nullptr;

        if (iteration) {
            std::memcpy(counters, &counters[buff_size - SECTION_SIZE], SECTION_SIZE);
        }

        for (int i = 0; iteration < block_count; i += SECTION_SIZE) {
            int j = 0;
            int carry = 1;
            while (carry && j < SECTION_SIZE) {
                carry = counters[i + j] == 255;
                counters[i + j] = (counters[i + j] + 1) % 256;
                j++;
            }

            iteration++;
            if (i >= buff_size - SECTION_SIZE) break;

            std::memcpy(&counters[i + SECTION_SIZE], &counters[i], SECTION_SIZE);
        }

        return reinterpret_cast<byte **>(counters);
    }
}