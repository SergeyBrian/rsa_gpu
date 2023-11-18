#include <thread>
#include <iostream>

#include "CPUEncryptor.hpp"

using State = byte[R][Nb];


namespace encryption {
    void encrypt_section(Key *key, State *state) {
        // Здесь по очереди вызываются все этапы шифрования
    }

    byte *CPUEncryptor::encrypt(Key *key, size_t size, byte *input) {
        size_t additional_bytes = SECTION_SIZE - (size % SECTION_SIZE);

        auto new_input = new byte[size + additional_bytes];
        std::memcpy(new_input, input, size);
        std::memset(new_input + size, '*', additional_bytes);

        new_input[size + additional_bytes - 1] = additional_bytes;


        auto states = new State[(size + additional_bytes) / SECTION_SIZE];
        int state_idx = 0;
        for (int state_offset = 0; state_offset < size; state_offset += R * Nb, state_idx++) {
            State *state = &states[state_idx];
            for (int r = 0; r < R; r++) {
                for (int c = 0; c < Nb; c++) {
                    (*state)[r][c] = new_input[state_offset + r + 4 * c];
                }
            }
            // Все секции шифруются
            encrypt_section(key, state);
        }

        auto output = new byte[size + additional_bytes];

        // Копирует по правилу символы из зашифрованных секций в выходной массив
        for (int i = 0, state_offset = 0; i < state_idx; i++, state_offset += R * Nb) {
            State *state = &states[i];
            for (int r = 0; r < R; r++) {
                for (int c = 0; c < Nb; c++) {
                    output[state_offset + r + 4 * c] = (*state)[r][c];
                }
            }
        }

        delete[] new_input;
        delete[] states;

        return output;
    }
}
