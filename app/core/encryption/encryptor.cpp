#include "encryptor.hpp"

namespace encryption {
    Encryptor::Encryptor(bool use_gpu, unsigned long long int block_count, size_t buff_size) {
        aes = new aes::AES(use_gpu);
        counter = new counter::Counter(block_count, buff_size);
    }

    /*
     * Запишет результат в input.
     * Функция используется для шифрования или дешифрования, в зависимости от того, какие данные подаются в input.
     * Функцию можно вызывать несколько раз для обработки последовательных наборов блоков данных.
     */
    void Encryptor::apply(Key *key, size_t size, byte *input) {
        byte **counters = counter->getCounters();
        byte *counters_encrypted = aes->encrypt(key,
                                                reinterpret_cast<const byte *>(counters),
                                                size);
        aes->XOR(input, counters_encrypted, size);
        delete[] counters_encrypted;
    }
}
