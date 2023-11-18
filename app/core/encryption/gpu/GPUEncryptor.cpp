#include <iostream>
#include "GPUEncryptor.hpp"


namespace encryption {
    GPUEncryptor::GPUEncryptor() {
        try {
            std::vector<cl::Platform> platforms;
            cl::Platform::get(&platforms);
            if (platforms.empty()) {
                throw std::runtime_error("OpenCL platforms not found\n");
            }

            this->platform = platforms.front();

            std::vector<cl::Device> devices;
            this->platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
            if (devices.empty()) {
                throw std::runtime_error("OpenCL devices not found\n");
            }
            this->device = devices.front();

            this->context = cl::Context(this->device);
            this->command_queue = cl::CommandQueue(this->context, this->device);

            this->AES_encrypt_source = load_kernel_source("AES_encrypt.cl");
            this->AES_decrypt_source = load_kernel_source("AES_decrypt.cl");

            this->AES_encrypt_program = cl::Program(this->context, this->AES_encrypt_source);
            this->AES_decrypt_program = cl::Program(this->context, this->AES_decrypt_source);

            this->AES_encrypt_program.build({this->device});
            this->AES_decrypt_program.build({this->device});
        } catch (const cl::Error &e) {
            if (e.err() == CL_BUILD_PROGRAM_FAILURE) {
                std::string buildLog = this->AES_encrypt_program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
                std::cerr << "Encrypt build log:\n" << buildLog << std::endl;
                buildLog = this->AES_decrypt_program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
                std::cerr << "Decrypt build log:\n" << buildLog << std::endl;
                exit(1);
            }
        } catch (const std::exception &e) {
            std::cerr << "OpenCL initialization error: " << e.what() << "\n";
            exit(1);
        }
        std::cout << "Using GPU device " << this->device.getInfo<CL_DEVICE_NAME>() << "\n";
    }

    byte *GPUEncryptor::encrypt(encryption::Key *key, size_t size, byte *input) {
        size_t additional_bytes;
        int states_count;
        State *states = get_states(input, size, &additional_bytes, &states_count);

        cl::Buffer statesBuffer(this->context, CL_MEM_READ_WRITE, SECTION_SIZE * states_count);
        this->command_queue.enqueueWriteBuffer(
                statesBuffer,
                CL_TRUE,
                0,
                SECTION_SIZE * states_count,
                *states);
        cl::Kernel kernel(this->AES_encrypt_program, "doubleMatrix");
        kernel.setArg(0, statesBuffer);
        this->command_queue.enqueueNDRangeKernel(kernel,
                                                 cl::NullRange,
                                                 cl::NDRange(SECTION_SIZE * states_count),
                                                 cl::NullRange);
        this->command_queue.finish();
        this->command_queue.enqueueReadBuffer(
                statesBuffer,
                CL_TRUE,
                0,
                SECTION_SIZE * states_count,
                *states);

        auto output = new byte[size + additional_bytes + sizeof(size_t)];

        for (int i = 0, state_offset = 0; i < states_count; i++, state_offset += R * Nb) {
            State *state = &states[i];
            for (int r = 0; r < R; r++) {
                for (int c = 0; c < Nb; c++) {
                    output[sizeof(size_t) + state_offset + r + 4 * c] = (*state)[r][c];
                }
            }
        }

        *output = size + additional_bytes;

        delete[] states;

        return output;
    }

    byte *GPUEncryptor::decrypt(Key *key, byte *input) {
        size_t size = *input;
        input += sizeof(size_t);
        auto result = new byte[size];

        if (size % SECTION_SIZE) {
            std::cerr << "ERROR: Incorrect data size\n";
            return nullptr;
        }

        auto states = new State[size / SECTION_SIZE];
        int state_idx = 0;
        for (int state_offset = 0; state_offset < size; state_offset += R * Nb, state_idx++) {
            State *state = &states[state_idx];
            for (int r = 0; r < R; r++) {
                for (int c = 0; c < Nb; c++) {
                    (*state)[r][c] = input[state_offset + r + 4 * c];
                }
            }
        }

        cl::Buffer bufferState(this->context, CL_MEM_READ_WRITE, SECTION_SIZE * state_idx);

        this->command_queue.enqueueWriteBuffer(
                bufferState,
                CL_TRUE,
                0,
                SECTION_SIZE * state_idx,
                *states);
        cl::Kernel kernel(this->AES_decrypt_program, "unDoubleMatrix");
        kernel.setArg(0, bufferState);
        this->command_queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(SECTION_SIZE * state_idx));
        this->command_queue.finish();
        this->command_queue.enqueueReadBuffer(
                bufferState,
                CL_TRUE,
                0,
                SECTION_SIZE * state_idx,
                *states);

        // Копирует по правилу символы из дешифрованных секций в выходной массив
        for (int i = 0, state_offset = 0; i < state_idx; i++, state_offset += R * Nb) {
            State *state = &states[i];
            for (int r = 0; r < R; r++) {
                for (int c = 0; c < Nb; c++) {
                    result[state_offset + r + 4 * c] = (*state)[r][c];
                }
            }
        }

        delete[] states;

        return result;
    }
}
