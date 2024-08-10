#include "func.h"
#include "bmi160_defs.h"
#include <cstdint>
#include <synchapi.h>

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
    Sleep(500);
    return 0;
#elif defined(LINUX)
    if (dev->fd < 0)
    {
        std::cout << "Device not open." << std::endl;
        return -1;
    }

    uint8_t mode = 0x02;

    auto ret = CH34xSetStream(dev->fd, mode);
    if (ret == false)
    {
        std::cout << "I2CConfig error: Failed to set stream mode." << std::endl;
        return -1;
    }
    return 0;
#endif
    return -1;
}

int wch_test_i2c(ch34x_dev *dev)
{
#ifdef WINDOWS
    if (dev->fd == INVALID_HANDLE_VALUE)
    {
        std::cout << "Device not open." << std::endl;
        return -1;
    }

    uint8_t chip_id = 0xFF;
    int8_t result = wch_read_i2c(dev, 0x69, 0x00, &chip_id, 1);
    if (result == 0)
    {
        printf("BMI160 Chip ID: 0x%02X\n", chip_id);
        if (chip_id == 0xD1)
        {
            std::cout << "Verification successful" << std::endl;
        }
        else
        {
            std::cout << "Verification failed" << std::endl;
        }
    }

    return 0;
#elif defined(LINUX)
    return -1;
#endif
    return -1;
}

void wch_delay_i2c(ch34x_dev *dev, uint32_t period)
{

#ifdef WINDOWS
    CH341SetDelaymS(dev->id, period);
#elif defined(LINUX)
    CH34xSetDelaymS(dev->fd, period);
#endif
}

int8_t wch_write_i2c(ch34x_dev *dev, uint8_t dev_addr, uint8_t reg_addr, uint8_t *write_data, uint16_t len)
{
#ifdef WINDOWS
    ULONG iIndex = dev->id;
    UCHAR iWriteBuffer[256];
    iWriteBuffer[0] = dev_addr << 1;
    iWriteBuffer[1] = reg_addr;
    memcpy(&iWriteBuffer[2], write_data, len);

    BOOL result = CH341StreamI2C(iIndex, len + 2, iWriteBuffer, 0, NULL);
    if (!result)
    {
        return -1;
    }

    return 0;
#elif defined(LINUX)
    // TODO: Linux implementation
    return -1;
#endif
    return -1;
}

int8_t wch_read_i2c(ch34x_dev *dev, uint8_t dev_addr, uint8_t reg_addr, uint8_t *read_data, uint16_t len)
{
#ifdef WINDOWS
    ULONG iIndex = dev->id;
    UCHAR iWriteBuffer[2];
    iWriteBuffer[0] = dev_addr << 1;
    iWriteBuffer[1] = reg_addr;

    BOOL result = CH341StreamI2C(iIndex, 2, iWriteBuffer, len, read_data);
    if (!result)
    {
        return -1;
    }

    return 0;
#elif defined(LINUX)
    // TODO: Linux implementation
    return -1;
#endif
    return -1;
}

int8_t bmi160_extract_sensor_time(struct bmi160_dev const *dev, uint32_t *sensor_time)
{
    return 0;
}