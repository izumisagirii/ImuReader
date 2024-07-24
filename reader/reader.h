// Reader backend for Linux / Windows
// 06 / 24 Yongheng CUI
#ifndef READER_H
#define READER_H

#include "func.h"
class ImuReader
{
  private:
    ch34x_dev wch_dev;
    bmi160_dev bmi_dev;

  public:
    int getInt();
};

#endif