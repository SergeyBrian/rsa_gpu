#define _CRT_SECURE_NO_WARNINGS
#include "encryptor.hpp"
#include <string.h>

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
    void Encryptor::apply(Key *key, size_t size, byte *input, bool ecb, bool decrypted) {
        if (ecb){
            byte *result;
            if (decrypted)
                result = aes->decrypt(key, reinterpret_cast<const byte *>(input), size);
            else
                result = aes->encrypt(key, reinterpret_cast<const byte *>(input),size);

            /*for (int i = 0; i < size; i++){
                input[i] = result[i];
            }*/
            memcpy(reinterpret_cast<byte *>(input), reinterpret_cast<const byte *>(result), size);
            delete[] result;
        }
        else{
            byte **counters = counter->getCounters();
            byte *counters_encrypted = aes->encrypt(key,
                                                    reinterpret_cast<const byte *>(counters),
                                                    size);
            aes->XOR(input, counters_encrypted, size);
            delete[] counters_encrypted;
        }

    }
}
