#include "encryptor.hpp"

namespace encryption {
    Encryptor::Encryptor(aes::gpu_mode mode, unsigned long long int block_count, size_t buff_size) {
        aes = new aes::AES(mode);
        counter = new counter::Counter(block_count, buff_size);
    }

    /*
     * Запишет результат в input.
     * Функция используется для шифрования или дешифрования, в зависимости от того, какие данные подаются в input.
     * Функцию можно вызывать несколько раз для обработки последовательных наборов блоков данных.
     */
    void Encryptor::apply(Key *key, size_t size, byte *input,  bool decrypt) {
        byte *result;
        if (!decrypt)
            result = aes->encrypt(key,reinterpret_cast<const byte *>(input),size);
        else
            result = aes->decrypt(key,reinterpret_cast<const byte *>(input),size);
        for (int i = 0; i < size; i++){
            input[i] = result[i];
        }
        delete[] result;
    }
}
