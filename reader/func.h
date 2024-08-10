#include "def.h"
#include <cstdint>
#include <iostream>

int wch_init_dev(const char *pathname, ch34x_dev *dev);
int wch_close_dev(ch34x_dev *dev);
int wch_config_i2c(ch34x_dev *dev, I2C_Rate rate);
int wch_test_i2c(ch34x_dev *dev);
void wch_delay_i2c(ch34x_dev *dev, uint32_t period);
int8_t wch_write_i2c(ch34x_dev *dev, uint8_t dev_addr, uint8_t reg_addr, uint8_t *write_data, uint16_t len);
int8_t wch_read_i2c(ch34x_dev *dev, uint8_t dev_addr, uint8_t reg_addr, uint8_t *read_data, uint16_t len);
int8_t bmi160_extract_sensor_time(struct bmi160_dev const *dev, uint32_t *sensor_time);