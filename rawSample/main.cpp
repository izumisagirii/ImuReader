#include "reader/reader.h"
#include <iostream>

int main()
{
    std::cout << "CommandLine IMU Test" << std::endl;
    ImuReader imuReader{};
    std::cout << "ImuReader Cmdline Tester by CYH" << std::endl;

#ifdef WINDOWS
    std::cout << "Running on Windows" << std::endl;
#elif defined(LINUX)
    std::cout << "Running on Linux" << std::endl;
#endif
    imuReader.init(0);
    std::cout << imuReader.setSensorConfig() << std::endl;
    return 0;
}