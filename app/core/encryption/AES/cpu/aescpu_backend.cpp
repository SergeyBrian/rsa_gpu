#include "aescpu_backend.hpp"
#define NUM_ROUNDS 14
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
byte *AESCPUBackend::decrypt(encryption::Key *key,
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

        addRoundKey(state, round_keys + ROUNDS_COUNT * SECTION_SIZE);

        for (int round_idx = ROUNDS_COUNT - 1; round_idx >= 1; round_idx--) {
            InvSubBytes(state);
            InvShiftRows(state);
            InvMixColumns(state);
            addRoundKey(state, round_keys + round_idx * SECTION_SIZE);
        }

        InvSubBytes(state);
        InvShiftRows(state);
        addRoundKey(state, round_keys);


        for (int col = 0; col < COLS; col++) {
            for (int row = 0; row < ROWS; row++) {
                result[block_idx + row + COLS * col] = state[row][col];
            }
        }
    }

    delete[] round_keys;
    return result;
}

void AESCPUBackend::XOR(byte *a, const byte *b, const size_t size) {
    for (int i = 0; i < size; i++) {
        a[i] ^= b[i];
    }
}

void AESCPUBackend::addRoundKey(byte (*state)[ROWS], const byte *round_key) {
    /*for (int col = 0; col < COLS; col++) {
        for (int row = 0; row < ROWS; row++) {
            state[row][col] ^= *(round_key + col * COLS + row);
        }
    }*/
    unsigned int i, j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            state[i][j] = state[i][j] ^ round_key[i + 4 * j];
        }
    }
}

void AESCPUBackend::subBytes(byte (*state)[ROWS]) {
    unsigned int i, j;
    unsigned char t;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            t = state[i][j];
            state[i][j] = encryption::aes::SBox[t / 16][t % 16];
        }
    }
}

void AESCPUBackend::shiftRows(byte (*state)[ROWS]) {
    /*byte tmp;

    for (int row = 1; row < 4; row++) {
        for (int shifts = 0; shifts < row; shifts++) {
            tmp = state[row][0];
            for (int col = 1; col < 4; col++) {
                state[row][col - 1] = state[row][col];
            }
            state[row][COLS - 1] = tmp;
        }
    }*/
    for (int i = 1, n = 1; i < 4, n < 4; i++, n++){
        unsigned char tmp[4];
        for (unsigned int j = 0; j < 4; j++) {
            tmp[j] = state[i][(j + n) % 4];
        }
        memcpy(state[i], tmp, 4 * sizeof(unsigned char));
    }
}

void AESCPUBackend::mixColumns(byte (*state)[ROWS]) {
    unsigned char temp_state[4][4];

    for (size_t i = 0; i < 4; ++i) {
        memset(temp_state[i], 0, 4);
    }

    for (size_t i = 0; i < 4; ++i) {
        for (size_t k = 0; k < 4; ++k) {
            for (size_t j = 0; j < 4; ++j) {
                if (encryption::aes::CMDS[i][k] == 1)
                    temp_state[i][j] ^= state[k][j];
                else
                    temp_state[i][j] ^= encryption::aes::GF_MUL_TABLE[encryption::aes::CMDS[i][k]][state[k][j]];
            }
        }
    }

    for (size_t i = 0; i < 4; ++i) {
        memcpy(state[i], temp_state[i], 4);
    }
}

    /*byte tmpColumn[ROWS];

    for (int col = 0; col < COLS; col++) {
        for (int row = 0; row < ROWS; row++) {
            tmpColumn[row] = 0;
            for (int col_GF = 0; col_GF < COLS; col_GF++) {
                byte a = state[col_GF][col];
                byte b = encryption::aes::GF28[row][col_GF];
                byte p = 0;

                // Умножение в поле Галуа
                for (int i = 0; i < 8; i++) {
                    if (b & 1) p ^= a;
                    bool high_bit_set = (a & 0x80) != 0;
                    a <<= 1;
                    if (high_bit_set) a ^= 0x1b; // 0x1b - неприводимый полином для GF(2^8)
                    b >>= 1;
                }

                tmpColumn[row] ^= p;
            }
        }

        for (int row = 0; row < ROWS; row++) {
            state[row][col] = tmpColumn[row];
        }
    }*/


void AESCPUBackend::InvSubBytes(byte (*state)[ROWS]) {
    unsigned int i, j;
    unsigned char t;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            t = state[i][j];
            state[i][j] = encryption::aes::InvSBox[t / 16][t % 16];
        }
    }
}

void AESCPUBackend::InvShiftRows(byte (*state)[ROWS]) {
    for (int i = 1; i < 4; i++){
        int n = 4 - i;
        unsigned char tmp[4];
        for (unsigned int j = 0; j < 4; j++) {
            tmp[j] = state[i][(j + n) % 4];
        }
        memcpy(state[i], tmp, 4 * sizeof(unsigned char));
    }
}

void AESCPUBackend::InvMixColumns(byte (*state)[ROWS]) {
    unsigned char temp_state[4][4];

    for (size_t i = 0; i < 4; ++i) {
        memset(temp_state[i], 0, 4);
    }

    for (size_t i = 0; i < 4; ++i) {
        for (size_t k = 0; k < 4; ++k) {
            for (size_t j = 0; j < 4; ++j) {
                temp_state[i][j] ^= encryption::aes::GF_MUL_TABLE[encryption::aes::INV_CMDS[i][k]][state[k][j]];
            }
        }
    }

    for (size_t i = 0; i < 4; ++i) {
        memcpy(state[i], temp_state[i], 4);
    }
}

