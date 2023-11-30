__kernel void loadStates(__global unsigned char *states, __global unsigned char *input) {
    int r = get_global_id(0);
    int c = get_global_id(1);
    states[r * 4 + c] = input[r + c * 4];
}

__kernel void unloadStates(__global unsigned char *states, __global unsigned char *result) {
    int c = get_global_id(0);
    int r = get_global_id(1);
    result[r + c * 4] = states[r * 4 + c];
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
    bytes[id] ^= key[offset * key_size + id % key_size];
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
    int col = get_global_id(0);
    for (int row = 0; row < 4; row++) {
        unsigned char sum = 0;
        for (int i = 0; i < 4; i++) {
            sum += bytes[col + i * 4] * GF28[(col % 4) * 4 + i];
        }
        bytes[row * 4 + col] = sum;
    }
}
