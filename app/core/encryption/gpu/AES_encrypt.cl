__kernel void doubleMatrix(__global unsigned char* matrix) {
    int id = get_global_id(0);
    matrix[id] *= 2;
}
