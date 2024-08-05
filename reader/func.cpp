#include "func.h"
#include "ch341_lib.h"
#include "reader/def.h"
#include <cstdint>
#include <iostream>

int wch_init_dev(const char *pathname, ch34x_dev *dev)
{
#ifdef WINDOWS
#elif defined(LINUX)
    dev->fd = CH34xOpenDevice(pathname);
    if (dev->fd < 0)
    {
        std::cerr << "OpenDevice false." << std::endl;
        return -1;
    }

    /* Get Driver Version */
    auto ret = CH34x_GetDriverVersion(dev->fd, (unsigned char *)dev->version);
    if (ret == false)
    {
        std::cerr << "GetDriverVersion error." << std::endl;
        return -1;
    }
    std::cout << "Driver version: " << dev->version << std::endl;

    /* Get Chip Type */
    ret = CH34x_GetChipType(dev->fd, &dev->chiptype);
    if (ret == false)
    {
        std::cerr << "GetChipType error." << std::endl;
        return -1;
    }

    /* Get Device ID */
    ret = CH34X_GetDeviceID(dev->fd, &dev->dev_id);
    if (ret == false)
    {
        std::cerr << "GetDeviceID error." << std::endl;
        return -1;
    }
    auto vendor = dev->dev_id;
    auto product = dev->dev_id >> 16;
    std::cout << "Vendor ID: 0x" << std::hex << std::setw(4) << std::setfill('0') << vendor << ", Product ID: 0x"
              << std::setw(4) << product << std::endl;
    std::cout << "Device operating has function [SPI+I2C+PARALLEL+GPIO]" << std::endl;
    std::cout << "Chip type: "
              << (dev->chiptype == CHIP_CH341    ? "CHIP_CH341"
                  : dev->chiptype == CHIP_CH347T ? "CHIP_CH347T"
                  : dev->chiptype == CHIP_CH347F ? "CHIP_CH347F"
                                                 : "Unknown")
              << std::endl;

    /* Set Timeout */
    ret = CH34xSetTimeout(dev->fd, 2000, 2000);
    if (ret == false)
    {
        std::cerr << "SetTimeout error." << std::endl;
        return -1;
    }
    return dev->fd;

#endif
    return -1;
}

int wch_close_dev(ch34x_dev *dev)
{
#ifdef WINDOWS
#elif defined(LINUX)
    auto ret = CH34xCloseDevice(dev->fd);
    if (ret == false)
    {
        std::cerr << "CloseDevice error." << std::endl;
        return -1;
    }
    dev->fd = -1;
    return 0;
#endif
    return -1;
}

int wch_config_i2c(ch34x_dev *dev, I2C_Rate rate)
{
#ifdef WINDOWS
#elif defined(LINUX)
    if (dev->fd < 0)
    {
        std::cerr << "Device not open." << std::endl;
        return -1;
    }

    uint8_t mode = 0x80;

    auto ret = CH34xSetStream(dev->fd, 0x00);
    if (ret == false)
    {
        std::cerr << "I2CConfig error: Failed to set stream mode." << std::endl;
        return -1;
    }
    return 0;
#endif
    return -1;
}