

#include "mbed.h"
#include "stdint.h"
#include "BNO055.h"
#include "flash.h"
#include "USBSerial.h"

// Forward declare or include your actual flash class

#define FLASH_START_ADDR 0x000000

// Logs one sample of BNO055 data into flash memory
void logAllBNOData(BNO055 *bno, flash *flash, USBSerial *serial);

// Reads back the specified number of samples from flash and prints them
void readAllBNOData(flash *flash, USBSerial *serial, uint32_t entryCount);
