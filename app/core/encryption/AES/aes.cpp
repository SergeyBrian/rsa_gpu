#include "aes.hpp"

namespace encryption::aes {
    AES::AES(gpu_mode mode) {
        switch (mode) {
            case CPU:
                this->backend = new AESCPUBackend();
                break;
            case DEFAULT:
                this->backend = new AESGPUBackend();
                break;
            case PARALLEL:
                this->backend = new AESALTGPUBackend();
                break;
        }
    }

    void AES::XOR(byte *a, const byte *b, const size_t size) {
        backend->XOR(a, b, size);
    }

    byte *AES::encrypt(Key *key,
                       const byte *input,
                       size_t size) {
        return backend->encrypt(key, input, size);
    }
    byte *AES::decrypt(Key *key,
                       const byte *input,
                       size_t size) {
        return backend->decrypt(key, input, size);
    }
}
