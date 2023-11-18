#include "AES.hpp"

namespace encryption {
    byte *encrypt_section(Key *key, byte *input) {
        // input - гарантированно 128 бит
        // return должен быть тоже 128 бит
        // Здесь по очереди вызываются все этапы шифрования
    }

    byte *decrypt_section(Key *key, byte *input) {
        // input - гарантированно 128 бит
        // return должен быть тоже 128 бит
        // Здесь по очереди вызываются все этапы дешифрования
    }

}
