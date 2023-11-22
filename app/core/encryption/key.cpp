#include "key.hpp"

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
        auto out = new byte[SECTION_SIZE * ROUNDS_COUNT];
        // TODO: Написать нормальный алгоритм
        for (int i = 0; i < SECTION_SIZE * ROUNDS_COUNT; i += KEYSIZE) {
            std::memcpy(out + i, value, KEYSIZE);
        }
        return out;
    }
}

