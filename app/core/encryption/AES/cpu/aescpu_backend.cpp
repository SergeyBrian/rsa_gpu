#include "aescpu_backend.hpp"

AESCPUBackend::AESCPUBackend() = default;

byte *AESCPUBackend::encrypt(encryption::Key *key,
                             const byte *input,
                             size_t size) {
    auto result = new byte[size];
    for (int i = 0; i < size; i++) {
        result[i] = input[i] * 2;
    }

    return result;
}

void AESCPUBackend::XOR(byte *a, const byte *b, size_t size) {
    for (int i = 0; i < size; i++) {
        a[i] ^= b[i];
    }
}
