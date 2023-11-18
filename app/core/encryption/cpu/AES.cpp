#include "AES.hpp"

namespace encryption {
    void apply_xor(Key *key, State *state) {
        for (int state_offset = 0; state_offset < KEYSIZE; state_offset += R * Nb) {
            for (int r = 0; r < R; r++) {
                for (int c = 0; c < Nb; c++) {
                    (*state)[r][c] ^= key->value[state_offset + r + 4 * c];
                }
            }
        }
    }
}
