#include <thread>
#include <iostream>

#include "CPUEncryptor.hpp"

namespace encryption {
    void encrypt_section(Key *key, State *state) {
        // Здесь по очереди вызываются все этапы шифрования
        // В качестве примера добавлю шифрование через XOR
        apply_xor(key, state);
    }

    void decrypt_section(Key *key, State *state) {
        // Здесь по очереди вызываются все этапы дешифрования
        // В качестве примера добавлю дешифрование через XOR
        apply_xor(key, state);
    }

    byte *CPUEncryptor::encrypt(Key *key, size_t size, byte *input) {
        size_t additional_bytes;
        int states_count;
        State *states = get_states(input, size, &additional_bytes, &states_count);

        for (int i = 0; i < states_count; i++) {
            encrypt_section(key, &states[i]);
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
        auto result = new byte[size];

        if (size % SECTION_SIZE) {
            std::cerr << "ERROR: Incorrect data size\n";
            return nullptr;
        }

        auto states = new State[size / SECTION_SIZE];
        int state_idx = 0;
        for (int state_offset = 0; state_offset < size; state_offset += R * Nb, state_idx++) {
            State *state = &states[state_idx];
            for (int r = 0; r < R; r++) {
                for (int c = 0; c < Nb; c++) {
                    (*state)[r][c] = input[state_offset + r + 4 * c];
                }
            }
            // Все секции дешифруются
            decrypt_section(key, state);
        }

        // Копирует по правилу символы из дешифрованных секций в выходной массив
        for (int i = 0, state_offset = 0; i < state_idx; i++, state_offset += R * Nb) {
            State *state = &states[i];
            for (int r = 0; r < R; r++) {
                for (int c = 0; c < Nb; c++) {
                    result[state_offset + r + 4 * c] = (*state)[r][c];
                }
            }
        }

        delete[] states;

        return result;
    }
}
