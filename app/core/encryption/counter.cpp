#include "counter.hpp"

namespace encryption {
    Counter::Counter() {
        std::string init_str = "I SUCK DICKS"; // Не менять, нужно именно такое
        strcpy_s(reinterpret_cast<char *>(this->value), SECTION_SIZE, init_str.c_str());
    }

    byte *Counter::get_value() {
        auto value_copy = new byte[SECTION_SIZE];
        memcpy(value_copy, value, SECTION_SIZE);

        int i = 0;
        int carry = 1;
        while (carry && i < SECTION_SIZE) {
            carry = this->value[i] == 255;
            this->value[i] = (this->value[i] + 1) % 256;
            i++;
        }

        return value_copy;
    }
}