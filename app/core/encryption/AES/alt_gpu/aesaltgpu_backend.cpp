#include "aesaltgpu_backend.hpp"
#include "../gpu/aesgpu_backend.hpp"

AESALTGPUBackend::AESALTGPUBackend() {
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
        std::string source = load_kernel_source("ALT_AES.cl");
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
    std::cout << "(Alternative GPU mode) Using GPU device " << device.getInfo<CL_DEVICE_NAME>() << "\n";
    SBox = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(encryption::aes::SBox));
    InvSBox = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(encryption::aes::InvSBox));
    RoundKeys = cl::Buffer(context, CL_MEM_READ_WRITE, SECTION_SIZE * (ROUNDS_COUNT + 1));
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
    command_queue.enqueueWriteBuffer(GF28,
                                     CL_TRUE,
                                     0,
                                     sizeof(encryption::aes::GF28),
                                     encryption::aes::GF28);
}

byte *AESALTGPUBackend::encrypt(encryption::Key *key,
                             const byte *input,
                             size_t size) {
    states = cl::Buffer(context, CL_MEM_READ_WRITE, size);

    byte *round_keys = key->expand();
    command_queue.enqueueWriteBuffer(RoundKeys,
                                     CL_TRUE,
                                     0,
                                     SECTION_SIZE * (ROUNDS_COUNT + 1),
                                     round_keys);

    return apply_aes(input, size);
}
byte *AESALTGPUBackend::decrypt(encryption::Key *key,
                                const byte *input,
                                size_t size) {
    states = cl::Buffer(context, CL_MEM_READ_WRITE, size);

    byte *round_keys = key->expand();
    command_queue.enqueueWriteBuffer(RoundKeys,
                                     CL_TRUE,
                                     0,
                                     SECTION_SIZE * (ROUNDS_COUNT + 1),
                                     round_keys);

    return apply_aes(input, size);
}

void AESALTGPUBackend::XOR(byte *a, const byte *b, const size_t size) {
    cl::Buffer bufA(context, CL_MEM_READ_WRITE, size);
    cl::Buffer bufB(context, CL_MEM_READ_WRITE, size);
    command_queue.enqueueWriteBuffer(bufA, CL_TRUE, 0, size, a);
    command_queue.enqueueWriteBuffer(bufB, CL_TRUE, 0, size, b);
    XOR(bufA, bufB, size);
    command_queue.finish();
    command_queue.enqueueReadBuffer(bufA, CL_TRUE, 0, size, a);
}

byte *AESALTGPUBackend::apply_aes(const byte *input, size_t size) {
    auto input_buf = cl::Buffer(context, CL_MEM_READ_WRITE, size);
    states = cl::Buffer(context, CL_MEM_READ_WRITE, size);
    kernel[KF_ENCRYPT].setArg(0, input_buf);
    kernel[KF_ENCRYPT].setArg(1, RoundKeys);
    kernel[KF_ENCRYPT].setArg(2, SBox);
    kernel[KF_ENCRYPT].setArg(3, GF28);
    kernel[KF_ENCRYPT].setArg(4, states);
    command_queue.enqueueWriteBuffer(input_buf, CL_TRUE, 0, size, input);
    command_queue.enqueueNDRangeKernel(kernel[KF_ENCRYPT],
                                       cl::NullRange,
                                       cl::NDRange(size/SECTION_SIZE),
                                       cl::NullRange);

    command_queue.finish();
    auto result = new byte[size];
    command_queue.enqueueReadBuffer(input_buf, CL_TRUE, 0, size, result);
    return result;
}

void AESALTGPUBackend::XOR(const cl::Buffer &a, const cl::Buffer &b, size_t size) {
    kernel[KF_XOR].setArg(0, a);
    kernel[KF_XOR].setArg(1, b);
    command_queue.enqueueNDRangeKernel(kernel[KF_XOR],
                                       cl::NullRange,
                                       cl::NDRange(size),
                                       cl::NullRange);
}
