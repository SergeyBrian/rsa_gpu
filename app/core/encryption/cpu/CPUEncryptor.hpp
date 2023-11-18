#ifndef RSA_GPU_CPUENCRYPTOR_HPP
#define RSA_GPU_CPUENCRYPTOR_HPP

#define MAX_THREADS 10

#include "../encryptor.hpp"

namespace encryption {
    class CPUEncryptor : public IEncryptor {
    public:
        byte *encrypt(Key *key, size_t size, byte *input) override;
    };
}

#endif //RSA_GPU_CPUENCRYPTOR_HPP
