#include <thread>
#include <iostream>

#include "CPUEncryptor.hpp"

namespace encryption {
    byte *CPUEncryptor::encrypt(Key *key, size_t size, byte *input) {
        size_t additional_bytes;
        int states_count;
        State *states = get_states(input, size, &additional_bytes, &states_count);
        auto counter = Counter();
        auto counters = new byte *[states_count];
        for (int i = 0; i < states_count; i++) {
            counters[i] = counter.get_value();
        }

        for (int i = 0; i < states_count; i++) {
            byte *encrypted_counter = encrypt_section(key, counters[i]);
            math::XOR(encrypted_counter, reinterpret_cast<byte *>(&states[i]), SECTION_SIZE);
            delete[] encrypted_counter;
        }


        auto output = new byte[size + additional_bytes + sizeof(size_t)];

        // Копирует по правилу символы из зашифрованных секций в выходной массив
        for (int i = 0, state_offset = 0; i < states_count; i++, state_offset += R * Nb) {
            State *state = &states[i];
            for (int r = 0; r < R; r++) {
                for (int c = 0; c < Nb; c++) {
                    output[sizeof(size_t) + state_offset + r + 4 * c] = (*state)[r][c];
                }
            }
        }

        *output = size + additional_bytes;

        delete[] states;

        return output;
    }

    byte *CPUEncryptor::decrypt(Key *key, byte *input) {
        size_t size = *input;
        input += sizeof(size_t);
        return encrypt(key, size, input);
    }
}
