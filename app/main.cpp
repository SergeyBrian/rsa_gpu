#include <iostream>

#include <CL/opencl.hpp>

#include "libs/cargs/cargs.h"

#include "core/encryption/cpu/CPUEncryptor.hpp"

static struct cag_option options[] = {
        {
                .identifier='g',
                .access_letters="g",
                .access_name="gen-key",
                .value_name=nullptr,
                .description="Generate private key",
        },
        {
                .identifier='k',
                .access_letters="k",
                .access_name="key",
                .value_name="KEY_NAME",
                .description="Key file name. Default is 'key'",
        },
        {
                .identifier='d',
                .access_letters="d",
                .access_name="decrypt",
                .value_name=nullptr,
                .description="Decrypt input file and write result into output file. (Encrypt if this flag is not set)"
        },
        {
                .identifier='o',
                .access_letters="o",
                .access_name="output",
                .value_name="OUTPUT_FILENAME",
                .description="Output file name",
        },
        {
                .identifier='h',
                .access_letters="h",
                .access_name="help",
                .value_name=nullptr,
                .description="Print list of options",
        },
};

struct configuration {
    bool gen_keys;
    const char *key_name;
    bool decrypt;
    const char *input_file_name;
    const char *output_file_name;
};

void print_help() {
    std::cout << "Usage: rsa_gpu [OPTIONS] <filename>\n\n" << "Available options:\n";
    cag_option_print(options, CAG_ARRAY_SIZE(options), stdout);
}


int main(int argc, char **argv) {
    if (argc == 1) {
        print_help();
        return 1;
    }

    char identifier;
    cag_option_context context;

    configuration config = {
            .gen_keys = false,
            .key_name = "key",
            .decrypt = false,
            .input_file_name = nullptr,
            .output_file_name = nullptr,
    };

    cag_option_prepare(&context, options, CAG_ARRAY_SIZE(options), argc, argv);

    while (cag_option_fetch(&context)) {
        identifier = cag_option_get(&context);
        switch (identifier) {
            case 'g':
                config.gen_keys = true;
                break;
            case 'd':
                config.decrypt = true;
                break;
            case 'k':
                config.key_name = cag_option_get_value(&context);
                break;
            case 'o':
                config.output_file_name = cag_option_get_value(&context);
                break;
            case 'h':
                print_help();
                return 0;
        }
    }

    if (context.index < argc) {
        config.input_file_name = argv[context.index];
    }

    if (config.gen_keys) {
        encryption::CPUEncryptor encryptor = encryption::CPUEncryptor();

        byte x[] = "ya ochen lublu sosat man cocks <3";

        auto key = new encryption::Key();
        byte *result = encryptor.encrypt(key, sizeof(x), x);
        std::cout << "Encrypted: " << result << "\n";
        byte *decrypted_result = encryptor.decrypt(key, result);
        if (decrypted_result)
            std::cout << "Decrypted: " << decrypted_result << "\n";
    }

    if (!config.input_file_name) {
        return 0;
    }

    if (config.decrypt) {
        // decrypt
        return 0;
    }

    // encrypt

    return 0;
}
