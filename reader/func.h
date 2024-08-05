#include "def.h"
#include <iomanip>
#include <iostream>

int wch_init_dev(const char *pathname, ch34x_dev *dev);
int wch_close_dev(ch34x_dev *dev);
int wch_config_i2c(ch34x_dev *dev, I2C_Rate rate);