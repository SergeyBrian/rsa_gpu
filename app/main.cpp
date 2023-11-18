#include <iostream>
#include <fstream>

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

    encryption::CPUEncryptor encryptor = encryption::CPUEncryptor();
    encryption::Key *key;

    if (config.gen_keys) {
        std::cout << "Generating key...\n";
        key = new encryption::Key();

        std::ofstream file(config.key_name, std::ios::binary);
        file.write(reinterpret_cast<const char *>(key->value), KEYSIZE);
        file.close();
        std::cout << "Key written to file '" << config.key_name << "'\n";
    } else {
        std::ifstream file(config.key_name, std::ios::binary);
        key = new encryption::Key(file);
        file.close();
    }

    if (!config.input_file_name) {
        std::cout << "Input filename not specified. Exiting...\n";
        return 0;
    }

    std::cout << "Reading data from file " << config.input_file_name << "\n";

    std::ifstream input_file(config.input_file_name, std::ios::binary | std::ios::ate);
    std::streamsize size_input_file = input_file.tellg();
    input_file.seekg(0, std::ios::beg);

    std::vector<byte> input_buffer(size_input_file);

    if (!input_file.read(reinterpret_cast<char *>(input_buffer.data()), size_input_file)) {
        std::cout << "Unable to read data from file. Exiting...\n";
        return 1;
    }
    input_buffer.push_back(0);

    byte *result;
    size_t result_size;

    if (config.decrypt) {
        result = encryptor.decrypt(key, input_buffer.data());
        result_size = *result;
        std::cout << "Decrypted: " << result << "\n";
    } else {
        result = encryptor.encrypt(key, size_input_file + 1, input_buffer.data());
        result_size = *result;
        result_size += sizeof(result_size);
        std::cout << "Encrypted: " << result << "\n";
    }

    if (config.output_file_name) {
        std::string suffix;
        std::string suffix_1 = ".decrypted";
        std::string suffix_2 = ".encrypted";

        suffix = (config.decrypt) ? suffix_1 : suffix_2;

        std::string file_name = config.input_file_name;

        std::cout << "Creating " << file_name + suffix << " instead\n";

        std::ofstream output_file(file_name + suffix, std::ios::binary);
        output_file.write(reinterpret_cast<const char *>(result), result_size);
        output_file.close();
    }


    return 0;
}
