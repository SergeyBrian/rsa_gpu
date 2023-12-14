void mixColumns(__global unsigned char *state, __global unsigned char *GF28) {
    unsigned char tmpColumn[4];

        for (int col = 0; col < 4; col++) {
            for (int row_GF = 0; row_GF < 4; row_GF++) {
                tmpColumn[row_GF] = 0;
                for (int col_GF = 0; col_GF < 4; col_GF++) {
                    unsigned char a = state[col + col_GF * 4];
                    unsigned char b = GF28[row_GF * 4 + col_GF];
                    unsigned char p = 0;


                    for (int i = 0; i < 8; i++) {
                        if (b & 1) {
                            p ^= a;
                        }
                        bool hi_bit = (a & 0x80) != 0;
                        a <<= 1;
                        if (hi_bit) {
                            a ^= 0x1b;
                        }
                        b >>= 1;
                    }

                    tmpColumn[row_GF] ^= p;
                }
            }

            for (int row = 0; row < 4; row++) {
                state[col + row * 4] = tmpColumn[row];
            }
        }
}

void addRoundKey(__global unsigned char *state, __global unsigned char *round_key) {
    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            state[row * 4 + col] ^= round_key[col * 4 + row];
        }
    }
}

void subBytes(__global unsigned char *state, __global unsigned char *SBox) {
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            state[row * 4 + col] = SBox[state[row * 4 + col]];
        }
    }
}

void shiftRows(__global unsigned char *state) {
    unsigned char tmp;

    for (int row = 1; row < 4; row++) {
        for (int shifts = 0; shifts < row; shifts++) {
            tmp = state[row*4];
            for (int col = 1; col < 4; col++) {
                state[row * 4 + col - 1] = state[row * 4 + col];
            }
            state[row * 4 + 4 - 1] = tmp;
        }
    }
}

__kernel void encrypt(__global unsigned char *input,
                      __global unsigned char *round_keys,
                      __global unsigned char *SBox,
                      __global unsigned char *GF28,
                      __global unsigned char *states) {
    int block_idx = get_global_id(0);
    __global unsigned char *state = states + block_idx * 16;
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            state[row * 4 + col] = input[block_idx * 16 + row + 4 * col];
        }
    }

    addRoundKey(state, round_keys);

    for (int round_idx = 1; round_idx < 14; round_idx++) {
        subBytes(state, SBox);
        shiftRows(state);
        mixColumns(state, GF28);
        addRoundKey(state, round_keys + round_idx * 16);
    }

    subBytes(state, SBox);
    shiftRows(state);
    addRoundKey(state, round_keys + 14 * 16);

    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            input[block_idx * 16 + row + 4 * col] = state[row * 4 + col];
        }
    }
}

__kernel void XOR(__global unsigned char *a, __global unsigned char *b) {
    int i = get_global_id(0);
    a[i] ^= b[i];
}
