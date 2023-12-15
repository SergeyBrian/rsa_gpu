#include <iostream>
#include <fstream>
#include <chrono>

#include "libs/cargs/cargs.h"

#include "core/encryption/encryptor.hpp"

#define MAX_INPUT_BUFFER 1_GB

static struct cag_option options[] = {
        {
                .identifier='g',
                .access_letters="g",
                .access_name="gen-key",
                .value_name=nullptr,
                .description="Generate private key",
        },
        {
                .identifier='G',
                .access_letters="G",
                .access_name="GPU",
                .value_name=nullptr,
                .description="Use gpu acceleration",
        },
        {
                .identifier='P',
                .access_letters="P",
                .access_name="Parallel",
                .value_name=nullptr,
                .description="Use alternative version of gpu acceleration",
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
    encryption::aes::gpu_mode gpu_mode;
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
            .gpu_mode = encryption::aes::CPU,
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
            case 'G':
                config.gpu_mode = encryption::aes::DEFAULT;
                break;
            case 'P':
                config.gpu_mode = encryption::aes::PARALLEL;
                break;
            case 'h':
            default:
                print_help();
                return 0;
        }
    }

    if (context.index < argc) {
        config.input_file_name = argv[context.index];
    }

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
        return 0;
    }

    std::cout << "Reading data from file " << config.input_file_name << "\n";

    std::ifstream input_file(config.input_file_name, std::ifstream::binary | std::ifstream::ate);
    size_t size_input_file = input_file.tellg();
    input_file.clear();
    input_file.seekg(0, std::ios_base::beg);
    input_file.seekg(0, std::ios::beg);


    size_t old_size = size_input_file;
    size_t extended_size;
    size_t extende = 0;
    if (!config.decrypt) {
        extended_size = size_input_file + sizeof(size_input_file);
        extende = SECTION_SIZE - (extended_size % SECTION_SIZE);
        extended_size += extende;
    } else {
        extended_size = size_input_file;
        if (size_input_file % SECTION_SIZE) {
            std::cerr << "Invalid file size\n";
            return 1;
        }
    }

    size_t buff_size = MIN(extended_size, MAX_INPUT_BUFFER);

    auto encryptor = encryption::Encryptor(config.gpu_mode, extended_size / SECTION_SIZE, buff_size);

    std::vector<byte> input_buffer(buff_size);

    std::ofstream output_file;
    if (!config.output_file_name) {
        std::string file_name = config.input_file_name;
        std::string enc_extension = ".encrypted";

        if (!config.decrypt) {
            file_name += enc_extension;
        } else {
            size_t i = file_name.find(enc_extension);
            if (i != std::string::npos)
                file_name.erase(i, enc_extension.length());
        }

        output_file = std::ofstream(file_name, std::ios::binary);
    } else {
        output_file = std::ofstream(config.output_file_name, std::ios::binary);
    }

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    size_t read_size = 0;
    size_t content_size = 0;
    if (!config.decrypt) {
        // Сохраняем размер изначального файла, соответственно считываем на sizeof(size_t) меньше
        *reinterpret_cast<size_t *>(input_buffer.data()) = old_size;
        int tmp = buff_size - sizeof(old_size);
        input_file.read(reinterpret_cast<char *>(input_buffer.data() + sizeof(old_size)),
                        tmp - extende);
        read_size += tmp - extende;
        content_size = extended_size;
    } else {
        input_file.read(reinterpret_cast<char *>(input_buffer.data()), buff_size);
        read_size += buff_size;
    }

    do {
        encryptor.apply(key, buff_size, input_buffer.data(), config.decrypt);
        size_t last_write_size;

        if (!content_size) {
            content_size = (*reinterpret_cast<size_t*>(input_buffer.data()));
            int tmp = buff_size - sizeof(content_size);
            last_write_size = MIN(tmp, content_size);
            output_file.write(reinterpret_cast<const char *>(input_buffer.data() + sizeof(size_t)),
                              last_write_size);
        } else {
            last_write_size = MIN(buff_size, content_size);
            output_file.write(reinterpret_cast<const char *>(input_buffer.data()), last_write_size);
        }
        content_size -= last_write_size;


        size_t last_read_size = MIN(buff_size, size_input_file - read_size);
        input_file.read(reinterpret_cast<char *>(input_buffer.data()), last_read_size);
        read_size += last_read_size;
    } while (content_size > 0);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cout << "Time spent: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[s]\n";

    output_file.close();
    input_file.close();

    return 0;
}
