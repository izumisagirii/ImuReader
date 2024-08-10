#include "reader.h"
#include "bmi160.h"
#include "bmi160_defs.h"
#include "reader/def.h"
#include <cstdint>
#include <synchapi.h>

ImuReader::~ImuReader()
{
    delete this->bmi160dev.fifo;
    bmi160_soft_reset(&this->bmi160dev);
    wch_close_dev(&this->wch_dev);
}
inline void ImuReader::delay(uint32_t period)
{
#ifdef WINDOWS
    Sleep(period);
#elif defined(LINUX)
// TODO: Linux implementation
#endif
}
int ImuReader::test(int devId)
{
    auto ret = wch_init_dev(intToConstChar(devId).c_str(), &this->wch_dev);
    wch_config_i2c(&this->wch_dev, I2C_Rate::Fast_400KHz);
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
    delay(100);

    /* set correct i2c address */
    bmi160dev.id = BMI160_I2C_ADDR + 1;
    bmi160dev.intf = BMI160_I2C_INTF;
    int8_t rslt;
    // bmi160_soft_reset(&this->bmi160dev);
    rslt = bmi160_init(&this->bmi160dev);
    if (rslt == BMI160_OK)
    {
        printf("BMI160 initialization success !\n");
        printf("Chip ID 0x%X\n", bmi160dev.chip_id);
    }
    else
    {
        printf("BMI160 initialization failure !\n");
        return rslt;
    }

    // this->setSensorConfig();
    delay(500);
    rslt = bmi160_perform_self_test(BMI160_ACCEL_ONLY, &this->bmi160dev);
    if (rslt == BMI160_OK)
    {
        printf("BMI160 accelerometer self test success !\n");
    }
    else
    {
        printf("BMI160 accelerometer self test failure !\n");
        return rslt;
    }
    rslt = bmi160_perform_self_test(BMI160_GYRO_ONLY, &this->bmi160dev);
    if (rslt == BMI160_OK)
    {
        printf("BMI160 gyroscope self test success !\n");
    }
    else
    {
        printf("BMI160 gyroscope self test failure !\n");
        return rslt;
    }
    delay(100);
    if (bmi160dev.fifo == nullptr)
    {
        bmi160dev.fifo = new bmi160_fifo_frame();
    }
    bmi160dev.fifo->data = this->fifo_data;
    bmi160dev.fifo->length = 1024;
    uint8_t fifo_conf = BMI160_FIFO_HEADER | BMI160_FIFO_ACCEL | BMI160_FIFO_GYRO | BMI160_FIFO_TIME;
    rslt = bmi160_set_fifo_config(fifo_conf, BMI160_ENABLE, &this->bmi160dev);
    if (rslt == BMI160_OK)
    {
        printf("BMI160 set fifo configuration success !\n");
    }
    else
    {
        printf("BMI160 set fifo configuration failure !\n");
        return rslt;
    }
    return 0;
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
    auto ret = bmi160_set_sens_conf(&bmi160dev);
    delay(100);
    if (ret == BMI160_OK)
    {
        printf("BMI160 set sensor configuration success !\n");
    }
    else
    {
        printf("BMI160 set sensor configuration failure !\n");
    }
    return ret;
}

// int ImuReader::setSensorCalibration()
// {
//     auto ret = bmi160_set_foc(&bmi160dev);
//     Sleep(200);
//     return ret;
// }

int ImuReader::getSensorData(bmi160_sensor_data *accel, bmi160_sensor_data *gyro)
{
    auto ret = bmi160_get_sensor_data((BMI160_TIME_SEL | BMI160_ACCEL_SEL | BMI160_GYRO_SEL), accel, gyro, &bmi160dev);
    if (ret != BMI160_OK)
    {
        printf("BMI160 get sensor data failure !\n");
    }
    return ret;
}

std::string ImuReader::intToConstChar(int value)
{
#ifdef WINDOWS
    return std::to_string(value);

#elif defined(LINUX)
    return "/dev/ch34x_pis" + std::to_string(value);
#endif
}

int ImuReader::getFIFOData(bmi160_sensor_data *accel, uint8_t *accel_len, bmi160_sensor_data *gyro, uint8_t *gyro_len,
                           uint32_t *sensor_time)
{
    int8_t result = bmi160_get_fifo_data(&bmi160dev);

    if (result != BMI160_OK)
    {
        printf("BMI160 get fifo data failure ! code %d\n", result);
        return result;
    }

    // Extract accelerometer data from FIFO
    result = bmi160_extract_accel(accel, accel_len, &bmi160dev);
    if (result != BMI160_OK)
    {
        printf("BMI160 extract accelerometer data failure !\n");
        return result; // Return error code if extracting accelerometer data fails
    }

    // Extract gyroscope data from FIFO
    result = bmi160_extract_gyro(gyro, gyro_len, &bmi160dev);
    if (result != BMI160_OK)
    {
        printf("BMI160 extract gyroscope data failure !\n");
        return result; // Return error code if extracting gyroscope data fails
    }
    // bmi160_extract_sensor_time(&bmi160dev, sensor_time);
    *sensor_time = bmi160dev.fifo->sensor_time;
    // uint8_t fifo_conf = BMI160_FIFO_HEADER | BMI160_FIFO_ACCEL | BMI160_FIFO_GYRO | BMI160_FIFO_TIME;
    // bmi160_set_fifo_config(fifo_conf, BMI160_ENABLE, &this->bmi160dev);
    return BMI160_OK; // Return success code
}