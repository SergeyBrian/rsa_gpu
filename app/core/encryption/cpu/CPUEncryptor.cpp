#include <thread>
#include <iostream>

#include "CPUEncryptor.hpp"

using State = byte[R][Nb];

void encrypt_thread_func(State *state, int tid) {
    std::cout << "Thread " << tid << "\n";

    for (int i = 0; i < R; i++) {
        for (int j = 0; j < Nb; j++) {
            std::cout << (*state)[i][j];
        }
        std::cout << "\n";
    }
}

namespace encryption {
    void CPUEncryptor::encrypt(Key *key, size_t size, byte *input, byte *output) {
        size_t additional_bytes = SECTION_SIZE - (size % SECTION_SIZE);

        auto new_input = new byte[size + additional_bytes];
        std::memcpy(new_input, input, size);
        std::memset(new_input + size, '*', additional_bytes);

        new_input[size + additional_bytes - 1] = additional_bytes;


        auto states = new State[(size + additional_bytes) / SECTION_SIZE];
        for (int state_offset = 0, state_idx = 0; state_offset < size; state_offset += R * Nb, state_idx++) {
            State *state = &states[state_idx];
            for (int r = 0; r < R; r++) {
                for (int c = 0; c < Nb; c++) {
                    (*state)[r][c] = new_input[state_offset + r + 4 * c];
                }
            }
            std::thread t(encrypt_thread_func, state, state_idx);
            t.join();
        }

        delete[] new_input;
        delete[] states;
    }
}
