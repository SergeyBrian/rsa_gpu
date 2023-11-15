#ifndef RSA_GPU_CPUENCRYPTOR_HPP
#define RSA_GPU_CPUENCRYPTOR_HPP

#define MAX_THREADS 10

#include "../encryptor.hpp"

namespace encryption {
    class CPUEncryptor : public IEncryptor {
    public:
        void encrypt(Key *key, size_t size, byte *input, byte *output) override;
    };
}

#endif //RSA_GPU_CPUENCRYPTOR_HPP
