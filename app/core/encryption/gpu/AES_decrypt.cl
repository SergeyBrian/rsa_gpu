__kernel void unDoubleMatrix(__global unsigned char* matrix, const unsigned int size) {
    int id = get_global_id(0);
    if (id < size) {
        matrix[id] /= 2;
    }
}
