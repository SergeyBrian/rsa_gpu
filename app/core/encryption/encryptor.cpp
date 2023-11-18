#include "encryptor.hpp"

namespace encryption {
    byte *IEncryptor::with_additional_bytes(byte *input, size_t size, size_t *additional_bytes) {
        *additional_bytes = SECTION_SIZE - (size % SECTION_SIZE);

        auto new_input = new byte[size + *additional_bytes];

        std::memcpy(new_input, input, size);
        std::memset(new_input + size, '*', *additional_bytes);

        new_input[size + *additional_bytes - 1] = *additional_bytes;

        return new_input;
    }

    State *IEncryptor::get_states(byte *input, size_t size, size_t *additional_bytes, int *states_count) {
        byte *new_input = with_additional_bytes(input, size, additional_bytes);

        auto *states = new State[(size + *additional_bytes) / SECTION_SIZE];
        int state_idx = 0;
        for (int state_offset = 0; state_offset < size; state_offset += R * Nb, state_idx++) {
            State *state = &states[state_idx];
            for (int r = 0; r < R; r++) {
                for (int c = 0; c < Nb; c++) {
                    (*state)[r][c] = new_input[state_offset + r + 4 * c];
                }
            }
        }

        *states_count = state_idx;

        delete[] new_input;

        return states;
    }
}
