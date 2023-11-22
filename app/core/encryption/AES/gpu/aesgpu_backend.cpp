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
}

byte *AESGPUBackend::encrypt(encryption::Key *key,
                             const byte *input,
                             size_t size) {
    auto result = new byte[size];
    cl::Buffer buf(context, CL_MEM_READ_WRITE, size);
    command_queue.enqueueWriteBuffer(buf, CL_TRUE, 0, size, input);
    kernel[KF_DOUBLE].setArg(0, buf);
    command_queue.enqueueNDRangeKernel(kernel[KF_DOUBLE], cl::NullRange, cl::NDRange(size), cl::NullRange);
    command_queue.finish();
    command_queue.enqueueReadBuffer(buf, CL_TRUE, 0, size, result);
    return result;
}

void AESGPUBackend::XOR(byte *a, const byte *b, size_t size) {
    cl::Buffer bufA(context, CL_MEM_READ_WRITE, size);
    cl::Buffer bufB(context, CL_MEM_READ_WRITE, size);
    command_queue.enqueueWriteBuffer(bufA, CL_TRUE, 0, size, a);
    command_queue.enqueueWriteBuffer(bufB, CL_TRUE, 0, size, b);
    kernel[KF_XOR].setArg(0, bufA);
    kernel[KF_XOR].setArg(1, bufB);
    command_queue.enqueueNDRangeKernel(kernel[KF_XOR], cl::NullRange, cl::NDRange(size), cl::NullRange);
    command_queue.finish();
    command_queue.enqueueReadBuffer(bufA, CL_TRUE, 0, size, a);
}
