#include "reader.h"

int ImuReader::getInt() { return CH34xOpenDevice("/dev/ch34x_pis0"); }