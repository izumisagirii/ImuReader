// Reader backend for Linux / Windows
// 06 / 24 Yongheng CUI
#ifndef READER_H
#define READER_H
#include <cstdint>
#ifdef WINDOWS
#ifdef BUILDING_DLL
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif
#else
#define DLL_EXPORT
#endif

#include "func.h"
class DLL_EXPORT ImuReader
{
  private:
    ch34x_dev wch_dev{};
    bmi160_dev bmi160dev{};
    // bmi160_sensor_data bmi160_accel{};
    // bmi160_sensor_data bmi160_gyro{};
    static std::string intToConstChar(int value);
    inline void delay(uint32_t period);
    uint8_t fifo_data[1028]{}; // FIFO data buffer

  public:
    int init(int devId);
    int setSensorConfig(const SensorConfig &config = defaultConfig);
    int setSensorCalibration();
    int test(int devId = 0);
    int getSensorData(bmi160_sensor_data *accel, bmi160_sensor_data *gyro);
    int getFIFOData(bmi160_sensor_data *accel, uint8_t *accel_len, bmi160_sensor_data *gyro, uint8_t *gyro_len,
                    uint32_t *sensor_time);
    ~ImuReader();
};

#endif