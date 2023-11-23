#include "aes.hpp"

namespace encryption::aes {
    AES::AES(bool use_gpu) {
        if (use_gpu) this->backend = new AESGPUBackend();
        else this->backend = new AESCPUBackend();
    }

    void AES::XOR(byte *a, const byte *b, size_t size) {
        backend->XOR(a, b, size);
    }

    byte *AES::encrypt(Key *key,
                       const byte *input,
                       size_t size) {
        return backend->encrypt(key, input, size);
    }
}
