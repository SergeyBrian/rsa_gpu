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

__kernel void mixColumns(__global unsigned char *bytes, __global unsigned char *GF28, int len) {
    unsigned char tmpColumn[4];
    int id = get_global_id(0);
    for (int col = 0; col < 4; col++) {
        for (int row_GF = 0; row_GF < 4; row_GF++) {
            tmpColumn[row_GF] = 0;
            for (int col_GF = 0; col_GF < 4; col_GF++) {
                tmpColumn[row_GF] += bytes[id * 16 + col + col_GF * 4] * GF28[row_GF * 4 + col_GF];
            }
        }

        for (int row = 0; row < 4; row++) {
            bytes[id * 16 + col + row * 4] = tmpColumn[row];
        }
    }
}
