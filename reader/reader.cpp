#include "reader.h"

int ImuReader::getInt()
{
    return wch_init_dev("/dev/ch34x_pis0", &this->wch_dev);
}