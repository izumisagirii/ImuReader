#include "func.h"
#include "reader/def.h"

int wch_init_dev(const char *pathname, ch34x_dev *dev)
{
#ifdef WINDOWS
#elif defined(LINUX)
    dev->fd = CH34xOpenDevice(pathname);
    if (dev->fd < 0)
    {
        std::cerr << "OpenDevice false." << std::endl;
        return -2;
    }

    /* Get Driver Version */
    auto ret = CH34x_GetDriverVersion(dev->fd, (unsigned char *)dev->version);
    if (ret == false)
    {
        std::cerr << "GetDriverVersion error." << std::endl;
        return -3;
    }
    std::cout << "Driver version: " << dev->version << std::endl;

    /* Get Chip Type */
    ret = CH34x_GetChipType(dev->fd, &dev->chiptype);
    if (ret == false)
    {
        std::cerr << "GetChipType error." << std::endl;
        return -4;
    }

    /* Get Device ID */
    ret = CH34X_GetDeviceID(dev->fd, &dev->dev_id);
    if (ret == false)
    {
        printf("CH34X_GetDeviceID error.\n");
        return -5;
    }
    auto vendor = dev->dev_id;
    auto product = dev->dev_id >> 16;
    std::cout << "Vendor ID: 0x" << std::hex << std::setw(4) << std::setfill('0') << vendor << ", Product ID: 0x"
              << std::setw(4) << product << std::endl;
    std::cout << "Device operating has function [SPI+I2C+PARALLEL+GPIO]." << std::endl;

    return dev->dev_id;

#endif
    return -1;
}