// Reader backend for Linux / Windows
// 06 / 24 Yongheng CUI
#ifndef READER_H
#define READER_H
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

  public:
    int init(int devId);
    int setSensorConfig(const SensorConfig &config = defaultConfig);
    int test(int devId = 0);
    ~ImuReader();
};

#endif