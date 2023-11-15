#include <CL/opencl.hpp>

#include <iostream>

std::vector<cl::Device> devices;

int find_devices() {
    std::vector<cl::Platform> platforms;
    std::vector<cl::Device> devices_available;
    int n = 0;
    cl::Platform::get(&platforms);
    for (const auto &platform: platforms) {
        devices_available.clear();
        platform.getDevices(CL_DEVICE_TYPE_ALL, &devices_available);
        if (devices_available.empty()) continue;
        for (const auto &j: devices_available) {
            n++;
            devices.push_back(j);
        }
    }
    if (platforms.empty() || devices.empty()) {
        std::cout << "Error: There are no OpenCL devices available!" << std::endl;
        return -1;
    }
    for (int i = 0; i < n; i++)
        std::cout << "ID: " << i << ", Device: " << devices[i].getInfo<CL_DEVICE_NAME>() << std::endl;
    return n;
}

int main() {
    find_devices();

    return 0;
}
