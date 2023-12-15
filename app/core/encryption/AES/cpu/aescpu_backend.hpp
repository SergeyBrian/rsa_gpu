#ifndef RSA_GPU_AESCPU_BACKEND_HPP
#define RSA_GPU_AESCPU_BACKEND_HPP

#include "../aes_backend.hpp"

class AESCPUBackend : public encryption::aes::AESBackend {

    static void addRoundKey(byte state[ROWS][COLS], const byte *round_key);

    static void subBytes(byte state[ROWS][COLS]);

    static void shiftRows(byte state[ROWS][COLS]);

    static void mixColumns(byte state[ROWS][COLS]);

    static void InvSubBytes(byte state[ROWS][COLS]);

    static void InvShiftRows(byte state[ROWS][COLS]);

    static void InvMixColumns(byte state[ROWS][COLS]);

public:

    AESCPUBackend();

    byte *encrypt(encryption::Key *key,const byte *input,size_t size) override;

    byte *decrypt(encryption::Key *key,const byte *input,size_t size) override;

    void XOR(byte *a, const byte *b, size_t size) override;
};

#endif //RSA_GPU_AESCPU_BACKEND_HPP
