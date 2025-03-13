#include "mbed.h"
#include "flash.h"

#ifndef FLASH_ENABLE_RESET
#define FLASH_ENABLE_RESET  0x66
#endif
#ifndef FLASH_RESET
#define FLASH_RESET         0x99
#endif

// Constructor
flash::flash(PinName mosi, PinName miso, PinName sclk, PinName csPin)
    : _spi(mosi, miso, sclk), _cs(csPin, 1) {
    _spi.format(8, 0);
    _spi.frequency(1000000);
}

// Chip Select Control
void flash::csLow() {
    _cs = 0;
    wait_us(5); // Short delay for chip select stabilization
}

void flash::csHigh() {
    wait_us(5); // Short delay for chip deselect
    _cs = 1;
}

// **FIXED: Write to Flash Memory**
void flash::write(uint32_t address, const uint8_t *buffer, size_t length) {
    enableWrite(); // Ensure write is enabled

    uint8_t cmd[4];
    cmd[0] = 0x02;  // Page Program Command
    cmd[1] = (address >> 16) & 0xFF;
    cmd[2] = (address >> 8) & 0xFF;
    cmd[3] = address & 0xFF;

    csLow();
    _spi.write((const char *)cmd, 4, NULL, 0);
    _spi.write((const char *)buffer, length, NULL, 0);
    csHigh();

    wait_us(5000); // **Important: Allow time for write operation to complete**
}

// **FIXED: Read from Flash Memory**
void flash::read(uint32_t address, uint8_t *buffer, size_t length) {
    uint8_t cmd[4];
    cmd[0] = 0x03;  // Read command
    cmd[1] = (address >> 16) & 0xFF;
    cmd[2] = (address >> 8) & 0xFF;
    cmd[3] = address & 0xFF;

    csLow();
    _spi.write((const char *)cmd, 4, NULL, 0);
    _spi.write(NULL, 0, (char *)buffer, length); // **Now correctly handling read buffer**
    csHigh();
}

// **FIXED: Read Single Byte**
uint8_t flash::readByte(uint32_t address) {
    uint8_t data = 0xFF;
    read(address, &data, 1);
    return data;
}

// **FIXED: Write a Single Byte**
void flash::writeByte(uint32_t address, uint8_t data) {
    write(address, &data, 1);
}

// Erase a sector (4KB)
void flash::eraseSector(uint32_t address) {
    enableWrite();  // Ensure write is enabled

    uint8_t cmd[4];
    cmd[0] = 0x20;  // Sector Erase Command
    cmd[1] = (address >> 16) & 0xFF;
    cmd[2] = (address >> 8) & 0xFF;
    cmd[3] = address & 0xFF;

    csLow();
    _spi.write((const char *)cmd, 4, NULL, 0);
    csHigh();

    wait_us(500000); // **Wait for erase operation to complete**
}

// Enable write operations
void flash::enableWrite() {
    uint8_t cmd = 0x06; // Write Enable Command
    csLow();
    _spi.write((const char *)&cmd, 1, NULL, 0);
    csHigh();
    wait_us(5000);
}

// Disable write operations
void flash::disableWrite() {
    uint8_t cmd = 0x04;  // Write Disable Command
    csLow();
    _spi.write((const char *)&cmd, 1, NULL, 0);
    csHigh();
    wait_us(5000);
}


void flash::reset() {
    uint8_t cmd;

    cmd = FLASH_ENABLE_RESET;
    csLow();
    _spi.write((const char *)&cmd, 1, NULL, 0);
    csHigh();
    ThisThread::sleep_for(5ms);

    cmd = FLASH_RESET;
    csLow();
    _spi.write((const char *)&cmd, 1, NULL, 0);
    csHigh();

    ThisThread::sleep_for(100ms);
}


// Write a floating-point number to flash
void flash::writeNum(uint32_t address, float data) {
    uint8_t tempBytes[4];
    float2Byte(tempBytes, data);
    write(address, tempBytes, 4);
}

// Read a floating-point number from flash
float flash::readNum(uint32_t address) {
    uint8_t rData[4];
    read(address, rData, 4);
    return bytes2float(rData);
}

// Convert float to byte array
void float2Byte(uint8_t *ftoa_bytes_temp, float float_variable) {
    union {
        float f;
        uint8_t b[4];
    } conv;
    conv.f = float_variable;
    memcpy(ftoa_bytes_temp, conv.b, 4);
}

// Convert byte array to float
float bytes2float(uint8_t *ftoa_bytes_temp) {
    union {
        float f;
        uint8_t b[4];
    } conv;
    memcpy(conv.b, ftoa_bytes_temp, 4);
    return conv.f;
}
