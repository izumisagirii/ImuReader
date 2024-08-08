#include "reader.h"
#include "reader/def.h"
#include <cstdint>
#include <functional>

ImuReader::~ImuReader()
{
    bmi160_soft_reset(&this->bmi160dev);
    wch_close_dev(&this->wch_dev);
}
int ImuReader::test(int devId)
{
    auto ret = wch_init_dev(intToConstChar(devId).c_str(), &this->wch_dev);
    wch_config_i2c(&this->wch_dev, I2C_Rate::High_750KHz);
    wch_test_i2c(&this->wch_dev);
    wch_close_dev(&this->wch_dev);
    return ret;
}

int ImuReader::init(int devId)
{
    wch_init_dev(intToConstChar(devId).c_str(), &this->wch_dev);
    wch_config_i2c(&this->wch_dev, I2C_Rate::High_750KHz);
    // interface i2c

    static ch34x_dev *dev_ptr = &this->wch_dev;
    bmi160dev.write = [](unsigned char PH1, unsigned char PH2, unsigned char *PH3, unsigned short PH4) -> signed char {
        return wch_write_i2c(dev_ptr, PH1, PH2, PH3, PH4);
    };
    bmi160dev.read = [](unsigned char PH1, unsigned char PH2, unsigned char *PH3, unsigned short PH4) -> signed char {
        return wch_read_i2c(dev_ptr, PH1, PH2, PH3, PH4);
    };
    bmi160dev.delay_ms = [](unsigned int PH1) -> void { wch_delay_i2c(dev_ptr, PH1); };

    /* set correct i2c address */
    bmi160dev.id = BMI160_I2C_ADDR + 1;
    bmi160dev.intf = BMI160_I2C_INTF;
    int8_t rslt;
    rslt = bmi160_init(&this->bmi160dev);
    if (rslt == BMI160_OK)
    {
        printf("BMI160 initialization success !\n");
        printf("Chip ID 0x%X\n", bmi160dev.chip_id);
    }
    else
    {
        printf("BMI160 initialization failure !\n");
    }
    return rslt;
}

int ImuReader::setSensorConfig(const SensorConfig &config)
{
    /* Select the Output data rate, range of accelerometer sensor */
    bmi160dev.accel_cfg.odr = config.accel_odr;
    bmi160dev.accel_cfg.range = config.accel_range;
    bmi160dev.accel_cfg.bw = config.accel_bw;

    /* Select the power mode of accelerometer sensor */
    bmi160dev.accel_cfg.power = config.accel_power;

    /* Select the Output data rate, range of Gyroscope sensor */
    bmi160dev.gyro_cfg.odr = config.gyro_odr;
    bmi160dev.gyro_cfg.range = config.gyro_range;
    bmi160dev.gyro_cfg.bw = config.gyro_bw;

    /* Select the power mode of Gyroscope sensor */
    bmi160dev.gyro_cfg.power = config.gyro_power;

    /* Set the sensor configuration */
    return bmi160_set_sens_conf(&bmi160dev);
}

std::string ImuReader::intToConstChar(int value)
{
#ifdef WINDOWS
    return std::to_string(value);

#elif defined(LINUX)
    return "/dev/ch34x_pis" + std::to_string(value);
#endif
}