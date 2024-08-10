#pragma once

#include <cstdint>
#include <functional>
#include <string>

#ifdef WINDOWS

#include <windows.h>

#endif
#ifdef WINDOWS
#include "CH341DLL_EN.H"
#include <handleapi.h>
#include <synchapi.h>

typedef enum CHIP_TYPE
{
    CHIP_CH341 = 0,
    CHIP_CH347T = 1,
    CHIP_CH347F = 2,
} CHIP_TYPE;

struct ch34x_dev
{
    ULONG id;
    HANDLE fd;
    char version[50];
    CHIP_TYPE chiptype;
    uint32_t dev_id;
};

#elif defined(LINUX)
#include "ch341_lib.h"
#include "ch347_lib.h"

struct ch34x_dev
{
    int fd;
    char version[50];
    CHIP_TYPE chiptype;
    uint32_t dev_id;
};
#endif

#include "bmi160.h"
#include "bmi160_defs.h"

#define CMD_FLASH_SECTOR_ERASE 0x20
#define CMD_FLASH_BYTE_PROG 0x02
#define CMD_FLASH_READ 0x03
#define CMD_FLASH_RDSR 0x05
#define CMD_FLASH_WREN 0x06
#define CMD_FLASH_JEDEC_ID 0x9F

#define SPI_FLASH_PerWritePageSize 256

enum class I2C_Rate : uint8_t
{
    Low_20KHz = 0x00,       // 20KHz
    Standard_100KHz = 0x01, // 100KHz
    Fast_400KHz = 0x02,     // 400KHz
    High_750KHz = 0x03      // 750KHz
};

enum class SPI_Mode : uint8_t
{
    OneInOut_LowBit = 0x00,  // one in one out, low bit first
    OneInOut_HighBit = 0x80, // one in one out, high bit first
    TwoInOut_LowBit = 0x04,  // two in two out, low bit first
    TwoInOut_HighBit = 0x84  // two in two out, high bit first
};

struct SensorConfig
{
    // Accelerometer configuration
    uint8_t accel_odr;
    uint8_t accel_range;
    uint8_t accel_bw;
    uint8_t accel_power;

    // Gyroscope configuration
    uint8_t gyro_odr;
    uint8_t gyro_range;
    uint8_t gyro_bw;
    uint8_t gyro_power;
};

//
const SensorConfig defaultConfig = {
    // Accelerometer configuration
    BMI160_ACCEL_ODR_1600HZ,     // Output data rate
    BMI160_ACCEL_RANGE_16G,      // Range
    BMI160_ACCEL_BW_NORMAL_AVG4, // Bandwidth
    BMI160_ACCEL_NORMAL_MODE,    // Power mode

    // Gyroscope configuration
    BMI160_GYRO_ODR_1600HZ,     // Output data rate
    BMI160_GYRO_RANGE_2000_DPS, // Range
    BMI160_GYRO_BW_NORMAL_MODE, // Bandwidth
    BMI160_GYRO_NORMAL_MODE     // Power mode
};
