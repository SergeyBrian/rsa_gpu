#include "key.hpp"
#include "AES/aes_backend.hpp"

#include <random>

namespace encryption {
    Key::Key() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);

        for (unsigned char &i: this->value) {
            i = static_cast<byte>(dis(gen));
        }
    }

    Key::Key(std::ifstream &file) {
        file.read(reinterpret_cast<char *>(this->value), KEYSIZE);
    }

    byte *Key::expand() {
        auto round_keys = new byte[SECTION_SIZE * (ROUNDS_COUNT + 1)];

        // Копирование исходного ключа (первые два раундовых ключа)
        std::memcpy(round_keys, value, KEYSIZE);

        // Генерация ещё 13 раундовых ключей поблочно (1 блок 4х4 байта на итерацию цикла)
        for (int i = KEYSIZE; i < SECTION_SIZE * (ROUNDS_COUNT + 1); i += sizeof(word)) {
            auto temp = new byte[sizeof(word)];
            std::copy(round_keys + i - sizeof(word), round_keys + i, temp);

            if (i % KEYSIZE == 0) {
                RotWord(temp);
                SubWord(temp);
                temp[0] ^= aes::RCon[i / KEYSIZE - 1];
            } else if (i % KEYSIZE == KEYSIZE / 2) {
                SubWord(temp);
            }

            for (int j = 0; j < sizeof(word); ++j) {
                round_keys[i + j] = round_keys[i + j - KEYSIZE] ^ temp[j];
            }
        }

        // В итоге массив из 15 раундовых ключей
        return round_keys;
    }

    void Key::RotWord(byte* word) {
        byte tmp = word[0];
        word[0] = word[1];
        word[1] = word[2];
        word[2] = word[3];
        word[3] = tmp;
    }

    void Key::SubWord(byte* a) {
        int i;
        for (i = 0; i < 4; i++) {
            a[i] = encryption::aes::SBox[a[i] / 16][a[i] % 16];
        }
    }
}

