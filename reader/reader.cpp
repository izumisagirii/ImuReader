#include "reader.h"
#include "reader/def.h"

int ImuReader::getInt()
{
    auto ret = wch_init_dev("/dev/ch34x_pis0", &this->wch_dev);
    // auto ret = wch_init_dev("0", &this->wch_dev);
    wch_config_i2c(&this->wch_dev, I2C_Rate::Fast_400KHz);
    wch_close_dev(&this->wch_dev);
    return ret;
}