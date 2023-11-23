#ifndef RSA_GPU_AESCPU_BACKEND_HPP
#define RSA_GPU_AESCPU_BACKEND_HPP

#include "../aes_backend.hpp"

class AESCPUBackend : public encryption::aes::AESBackend {
public:
    AESCPUBackend();

    byte *encrypt(encryption::Key *key,
                  const byte *input,
                  size_t size) override;

    void XOR(byte *a, const byte *b, size_t size) override;
};

#endif //RSA_GPU_AESCPU_BACKEND_HPP
