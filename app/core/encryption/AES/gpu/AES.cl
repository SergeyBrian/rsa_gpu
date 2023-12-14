__kernel void loadStates(__global unsigned char *states, __global unsigned char *input) {
    int id = get_global_id(0);
    states[id] = input[(id / 16) * 16 + (id % 4) * 4 + ((id % 16) / 4)];
}

__kernel void unloadStates(__global unsigned char *states, __global unsigned char *result) {
    int id = get_global_id(0);
    result[(id / 16) * 16 + (id % 4) * 4 + ((id % 16) / 4)] = states[id];
}

__kernel void doubleMatrix(__global unsigned char *matrix) {
    int id = get_global_id(0);
    matrix[id] *= 2;
}

__kernel void XOR(__global unsigned char *a, __global const unsigned char *b) {
    int id = get_global_id(0);
    a[id] ^= b[id];
}

__kernel void KeyXOR(__global unsigned char *bytes,
                     __global const unsigned char *key,
                     int offset,
                     unsigned int key_size) {
    int id = get_global_id(0);
    bytes[id] ^= key[offset * key_size + (id % 4) * 4 + ((id % key_size) / 4)];
}

__kernel void subBytes(__global unsigned char *bytes,
                       __global unsigned char *SBox,
                       __global unsigned char *InvSBox) {
    int id = get_global_id(0);
    bytes[id] = SBox[bytes[id]];
}

__kernel void shiftRows(__global unsigned char *row, int len) {
    unsigned char tmp;
    int id = get_global_id(0);
    int offset = id * len;
    for (int i = 0; i < id % len; i++) {
        tmp = row[0 + offset];
        for (int j = 1; j < len; j++) {
            row[j - 1 + offset] = row[j + offset];
        }
        row[len - 1 + offset] = tmp;
    }
}

__kernel void mixColumns(__global unsigned char *bytes, int len) {
    const unsigned char GF28[4][4] = {
            {0x02, 0x03, 0x01, 0x01},
            {0x01, 0x02, 0x03, 0x01},
            {0x01, 0x01, 0x02, 0x03},
            {0x03, 0x01, 0x01, 0x02}
        };
    int id = get_global_id(0);
    unsigned char tmpColumn[4];

    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            tmpColumn[row] = 0;
            for (int col_GF = 0; col_GF < 4; col_GF++) {
                unsigned char a = bytes[id * len + col_GF * 4 + col];
                unsigned char b = GF28[row][col_GF];
                unsigned char p = 0;


                for (int i = 0; i < 8; i++) {
                    if (b & 0x01) {
                        p ^= a;
                    }
                    bool hi_bit = (a & 0x80) != 0;
                    a <<= 1;
                    if (hi_bit) {
                        a ^= 0x1b;
                    }
                    b >>= 1;
                }

                tmpColumn[row] ^= p;
            }
        }

        for (int row = 0; row < 4; row++) {
            bytes[id * len + col + row * 4] = tmpColumn[row];
        }
    }
}
