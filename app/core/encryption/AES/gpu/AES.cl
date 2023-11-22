__kernel void doubleMatrix(__global unsigned char* matrix) {
    int id = get_global_id(0);
    matrix[id] *= 2;
}

__kernel void XOR(__global unsigned char* a, __global const unsigned char* b) {
    int id = get_global_id(0);
    a[id] ^= b[id];
}
