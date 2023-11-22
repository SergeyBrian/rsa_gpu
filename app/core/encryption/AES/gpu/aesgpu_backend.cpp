#include "aesgpu_backend.hpp"

AESGPUBackend::AESGPUBackend() {
    try {
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);
        if (platforms.empty()) {
            throw std::runtime_error("OpenCL platforms not found\n");
        }
        platform = platforms.front();
        std::vector<cl::Device> devices;
        platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
        if (devices.empty()) {
            throw std::runtime_error("OpenCL devices not found\n");
        }
        device = devices.front();
        context = cl::Context(device);
        command_queue = cl::CommandQueue(context, device);
        std::string source = load_kernel_source("AES.cl");
        program = cl::Program(context, source);
        program.build({device});
        for (int kf = 0; kf < KF_COUNT; kf++) {
            kernel[static_cast<KFunc>(kf)] = cl::Kernel(program, KFuncName[kf]);
        }
    } catch (const cl::Error &e) {
        if (e.err() == CL_BUILD_PROGRAM_FAILURE) {
            std::string buildLog = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
            std::cerr << "Build log:\n" << buildLog << std::endl;
            exit(1);
        }
    } catch (const std::exception &e) {
        std::cerr << "OpenCL initialization error: " << e.what() << "\n";
        exit(1);
    }
    std::cout << "Using GPU device " << device.getInfo<CL_DEVICE_NAME>() << "\n";
    SBox = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(encryption::aes::SBox));
    InvSBox = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(encryption::aes::InvSBox));
    RCon = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(encryption::aes::RCon));
    RoundKeys = cl::Buffer(context, CL_MEM_READ_WRITE, SECTION_SIZE * ROUNDS_COUNT);
    GF28 = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(encryption::aes::GF28));
    command_queue.enqueueWriteBuffer(SBox,
                                     CL_TRUE,
                                     0,
                                     sizeof(encryption::aes::SBox),
                                     encryption::aes::SBox);
    command_queue.enqueueWriteBuffer(InvSBox,
                                     CL_TRUE,
                                     0,
                                     sizeof(encryption::aes::InvSBox),
                                     encryption::aes::InvSBox);
    command_queue.enqueueWriteBuffer(RCon,
                                     CL_TRUE,
                                     0,
                                     sizeof(encryption::aes::RCon),
                                     encryption::aes::RCon);
    command_queue.enqueueWriteBuffer(GF28,
                                     CL_TRUE,
                                     0,
                                     sizeof(encryption::aes::GF28),
                                     encryption::aes::GF28);
}

byte *AESGPUBackend::encrypt(encryption::Key *key,
                             const byte *input,
                             size_t size) {
    states = cl::Buffer(context, CL_MEM_READ_WRITE, size);

    kernel[KF_SUB_BYTES].setArg(0, states);
    kernel[KF_SUB_BYTES].setArg(1, SBox);
    kernel[KF_SUB_BYTES].setArg(2, InvSBox);

    load_states(input, size);
    byte *round_keys = key->expand();
    command_queue.enqueueWriteBuffer(RoundKeys,
                                     CL_TRUE,
                                     0,
                                     SECTION_SIZE * ROUNDS_COUNT,
                                     round_keys);

    KeyXOR(states, RoundKeys, 0, size);

    for (int round_idx = 1; round_idx < ROUNDS_COUNT; round_idx++) {
        sub_bytes(states, size);
        shift_rows(states, size, COLS);
        mix_columns(states, size);
        KeyXOR(states, RoundKeys, round_idx, size);
    }

    command_queue.finish();
    delete[] round_keys;
    return unload_states(size);
}

void AESGPUBackend::XOR(byte *a, const byte *b, size_t size) {
    cl::Buffer bufA(context, CL_MEM_READ_WRITE, size);
    cl::Buffer bufB(context, CL_MEM_READ_WRITE, size);
    command_queue.enqueueWriteBuffer(bufA, CL_TRUE, 0, size, a);
    command_queue.enqueueWriteBuffer(bufB, CL_TRUE, 0, size, b);
    XOR(bufA, bufB, size);
    command_queue.finish();
    command_queue.enqueueReadBuffer(bufA, CL_TRUE, 0, size, a);
}

void AESGPUBackend::XOR(const cl::Buffer &a, const cl::Buffer &b, size_t size) {
    kernel[KF_XOR].setArg(0, a);
    kernel[KF_XOR].setArg(1, b);
    command_queue.enqueueNDRangeKernel(kernel[KF_XOR],
                                       cl::NullRange,
                                       cl::NDRange(size),
                                       cl::NullRange);
}

void AESGPUBackend::KeyXOR(const cl::Buffer &bytes, const cl::Buffer &key, int offset, size_t size, size_t key_size) {
    kernel[KF_KEY_XOR].setArg(0, bytes);
    kernel[KF_KEY_XOR].setArg(1, key);
    kernel[KF_KEY_XOR].setArg(2, offset);
    kernel[KF_KEY_XOR].setArg(3, static_cast<unsigned long>(key_size));
    command_queue.enqueueNDRangeKernel(kernel[KF_KEY_XOR],
                                       cl::NullRange,
                                       cl::NDRange(size));
}

void AESGPUBackend::load_states(const byte *input, size_t size) {
    cl::Buffer input_buf(context, CL_MEM_READ_WRITE, size);
    command_queue.enqueueWriteBuffer(input_buf, CL_TRUE, 0, size, input);
    kernel[KF_LOAD_STATES].setArg(0, states);
    kernel[KF_LOAD_STATES].setArg(1, input_buf);
    command_queue.enqueueNDRangeKernel(kernel[KF_LOAD_STATES],
                                       cl::NullRange,
                                       cl::NDRange(ROWS * (size / SECTION_SIZE)),
                                       cl::NDRange(COLS));
}

byte *AESGPUBackend::unload_states(size_t size) {
    cl::Buffer result_buf(context, CL_MEM_HOST_READ_ONLY, size);
    kernel[KF_UNLOAD_STATES].setArg(0, result_buf);
    kernel[KF_UNLOAD_STATES].setArg(1, states);
    command_queue.enqueueNDRangeKernel(kernel[KF_UNLOAD_STATES],
                                       cl::NullRange,
                                       cl::NDRange(ROWS * (size / SECTION_SIZE)),
                                       cl::NDRange(COLS));
    command_queue.finish();
    auto result = new byte[size];
    command_queue.enqueueReadBuffer(result_buf, CL_TRUE, 0, size, result);
    return result;
}

void AESGPUBackend::sub_bytes(const cl::Buffer &bytes, size_t size) {
    kernel[KF_SUB_BYTES].setArg(0, bytes);
    command_queue.enqueueNDRangeKernel(kernel[KF_SUB_BYTES],
                                       cl::NullRange,
                                       cl::NDRange(size),
                                       cl::NullRange);
}

void AESGPUBackend::shift_rows(const cl::Buffer &bytes, size_t size, int len) {
    kernel[KF_SHIFT_ROWS].setArg(0, bytes);
    kernel[KF_SHIFT_ROWS].setArg(1, len);
    command_queue.enqueueNDRangeKernel(kernel[KF_SHIFT_ROWS],
                                       cl::NullRange,
                                       cl::NDRange(size / len));
}

void AESGPUBackend::mix_columns(const cl::Buffer &bytes, size_t size) {
    kernel[KF_MIX_COLUMNS].setArg(0, bytes);
    kernel[KF_MIX_COLUMNS].setArg(1, GF28);
    kernel[KF_MIX_COLUMNS].setArg(2, COLS);
    command_queue.enqueueNDRangeKernel(kernel[KF_MIX_COLUMNS],
                                       cl::NullRange,
                                       cl::NDRange(size / COLS));
}
