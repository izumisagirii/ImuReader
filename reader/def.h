#pragma once

#include <cstdint>
#ifdef WINDOWS
#include "CH341DLL.h"
#elif defined(LINUX)
#include "ch341_lib.h"
#endif

#define CMD_FLASH_SECTOR_ERASE 0x20
#define CMD_FLASH_BYTE_PROG 0x02
#define CMD_FLASH_READ 0x03
#define CMD_FLASH_RDSR 0x05
#define CMD_FLASH_WREN 0x06
#define CMD_FLASH_JEDEC_ID 0x9F

#define SPI_FLASH_PerWritePageSize 256

struct ch34x {
  int fd;
  char version[100];
  CHIP_TYPE chiptype;
  uint32_t dev_id;
};