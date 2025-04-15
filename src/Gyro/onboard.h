#ifndef ONBOARD_H
#define ONBOARD_H

#include "mbed.h"
#include "stdint.h"
#include "BNO055.h"
#include "flash.h"
#include "USBSerial.h"

// Forward declare or include your actual flash class

#define FLASH_START_ADDR     0x000000
#define FLASH_TOTAL_SIZE     0x200000   // 2 MB = 16 Mbit
#define FLASH_SECTOR_SIZE    0x1000     // 4 KB

// Logs one sample of BNO055 data into flash memory
void logAllBNOData(BNO055 *bno, flash *flash, USBSerial *serial);

// Reads back the specified number of samples from flash and prints them
void readAllBNOData(flash *flash, USBSerial *serial, uint32_t entryCount);

#endif // ONBOARD_H