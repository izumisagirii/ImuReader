#include "func.h"
#include <cstdint>
#include <iostream>
#include <string>

int wch_init_dev(const char *pathname, ch34x_dev *dev)
{
#ifdef WINDOWS
    int device_number = 0;
    try
    {
        device_number = std::stoi(pathname);
        if (device_number < 0 || device_number > 255)
        {
            throw std::invalid_argument("Invalid devId.");
            return -1;
        }
    }
    catch (const std::invalid_argument &e)
    {
        std::cout << "Invalid devId." << std::endl;
        return -1;
    }
    dev->id = device_number;
    dev->fd = CH341OpenDevice(dev->id);
    if (dev->fd == INVALID_HANDLE_VALUE)
    {
        std::cout << "OpenDevice false." << std::endl;
        return -1;
    }

    /* Set Timeout */
    auto ret = CH341SetTimeout(dev->dev_id, 2000, 2000);
    if (ret == false)
    {
        std::cout << "SetTimeout error." << std::endl;
        return -1;
    }
    return 0;
#elif defined(LINUX)
    dev->fd = CH34xOpenDevice(pathname);
    if (dev->fd < 0)
    {
        std::cout << "OpenDevice false." << std::endl;
        return -1;
    }

    /* Get Driver Version */
    auto ret = CH34x_GetDriverVersion(dev->fd, (unsigned char *)dev->version);
    if (ret == false)
    {
        std::cout << "GetDriverVersion error." << std::endl;
        return -1;
    }
    std::cout << "Driver version: " << dev->version << std::endl;

    /* Get Chip Type */
    ret = CH34x_GetChipType(dev->fd, &dev->chiptype);
    if (ret == false)
    {
        std::cout << "GetChipType error." << std::endl;
        return -1;
    }

    /* Get Device ID */
    ret = CH34X_GetDeviceID(dev->fd, &dev->dev_id);
    if (ret == false)
    {
        std::cout << "GetDeviceID error." << std::endl;
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
        std::cout << "SetTimeout error." << std::endl;
        return -1;
    }
    return dev->fd;

#endif
    return -1;
}

int wch_close_dev(ch34x_dev *dev)
{
#ifdef WINDOWS
    CH341CloseDevice(dev->id);
    dev->id = -1;
    dev->fd = INVALID_HANDLE_VALUE;
    return 0;
#elif defined(LINUX)
    auto ret = CH34xCloseDevice(dev->fd);
    if (ret == false)
    {
        std::cout << "CloseDevice error." << std::endl;
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
    if (dev->fd == INVALID_HANDLE_VALUE)
    {
        std::cout << "Device not open." << std::endl;
        return -1;
    }

    auto ret = CH341SetStream(dev->id, ULONG(rate));
    if (ret == false)
    {
        std::cout << "I2CConfig error: Failed to set stream mode." << std::endl;
        return -1;
    }

    // TEST
    ULONG iIndex = 0;     // CH341 设备序号
    UCHAR iDevice = 0x69; // BMI160 I2C 地址
    UCHAR iAddr = 0x00;   // 芯片 ID 寄存器地址
    UCHAR oByte;          // 用于存储读取的数据

    BOOL result = CH341ReadI2C(iIndex, iDevice, iAddr, &oByte);
    if (result)
    {
        printf("BMI160 Chip ID: 0x%02X\n", oByte);
    }
    else
    {
        printf("Failed to read from BMI160\n");
    }

    return 0;
#elif defined(LINUX)
    if (dev->fd < 0)
    {
        std::cout << "Device not open." << std::endl;
        return -1;
    }

    uint8_t mode = 0x80;

    auto ret = CH34xSetStream(dev->fd, 0x00);
    if (ret == false)
    {
        std::cout << "I2CConfig error: Failed to set stream mode." << std::endl;
        return -1;
    }
    return 0;
#endif
    return -1;
}