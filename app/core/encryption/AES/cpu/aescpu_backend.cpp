#include "aescpu_backend.hpp"

AESCPUBackend::AESCPUBackend() = default;

byte *AESCPUBackend::encrypt(encryption::Key *key,
                             const byte *input,
                             size_t size) {
    auto result = new byte[size];
    byte state[ROWS][COLS];
    byte *round_keys = key->expand();

    for (int block_idx = 0; block_idx < size; block_idx += SECTION_SIZE) {

        for (int row = 0; row < ROWS; row++) {
            for (int col = 0; col < COLS; col++) {
                state[row][col] = input[block_idx + row + COLS * col];
            }
        }

        addRoundKey(state, round_keys);

        for (int round_idx = 1; round_idx < ROUNDS_COUNT; round_idx++) {
            subBytes(state);
            shiftRows(state);
            mixColumns(state);
            addRoundKey(state, round_keys + round_idx * SECTION_SIZE);
        }

        subBytes(state);
        shiftRows(state);
        addRoundKey(state, round_keys + ROUNDS_COUNT * SECTION_SIZE);

        for (int col = 0; col < COLS; col++) {
            for (int row = 0; row < ROWS; row++) {
                result[block_idx + row + COLS * col] = state[row][col];
            }
        }
    }

    delete[] round_keys;
    return result;
}

void AESCPUBackend::XOR(byte *a, const byte *b, size_t size) {
    for (int i = 0; i < size; i++) {
        a[i] ^= b[i];
    }
}

void AESCPUBackend::addRoundKey(byte (*state)[ROWS], const byte *round_key) {
    for (int col = 0; col < COLS; col++) {
        for (int row = 0; row < ROWS; row++) {
            state[row][col] ^= *(round_key + col * COLS + row);
        }
    }
}

void AESCPUBackend::subBytes(byte (*state)[ROWS]) {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            state[row][col] = encryption::aes::SBox[state[row][col]];
        }
    }
}

void AESCPUBackend::shiftRows(byte (*state)[ROWS]) {
    byte tmp;

    for (int row = 1; row < 4; row++) {
        for (int shifts = 0; shifts < row; shifts++) {
            tmp = state[row][0];
            for (int col = 1; col < 4; col++) {
                state[row][col - 1] = state[row][col];
            }
            state[row][COLS - 1] = tmp;
        }
    }
}

void AESCPUBackend::mixColumns(byte (*state)[ROWS]) {
    byte tmpColumn[ROWS];

    for (int col = 0; col < COLS; col++) {
        for (int row_GF = 0; row_GF < ROWS; row_GF++) {
            tmpColumn[row_GF] = 0;
            for (int col_GF = 0; col_GF < COLS; col_GF++) {
                tmpColumn[row_GF] += state[col_GF][col] * encryption::aes::GF28[row_GF][col_GF];
            }
        }

        for (int row = 0; row < ROWS; row++) {
            state[row][col] = tmpColumn[row];
        }
    }

}
