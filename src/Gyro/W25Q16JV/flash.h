#ifndef FLASH_H
#define FLASH_H

#include "mbed.h"

class flash {
public:
    // Constructor
    flash(PinName mosi, PinName miso, PinName sclk, PinName csPin);

    // Read operations
    void read(uint32_t address, uint8_t *buffer, size_t length);
    uint8_t readByte(uint32_t address);
    float readNum(uint32_t address);

    // Write operations
    void write(uint32_t address, const uint8_t *buffer, size_t length);
    void writeByte(uint32_t address, uint8_t data);
    void writeNum(uint32_t address, float data);

    // Erase operations
    void eraseSector(uint32_t address);

    // Control operations
    void enableWrite();
    void disableWrite();
    void reset();

private:
    SPI _spi;       // SPI communication interface
    DigitalOut _cs; // Chip Select (CS) pin

    // Helper functions for SPI communication
    void csLow();
    void csHigh();
};

// Float conversion functions
void float2Byte(uint8_t *ftoa_bytes_temp, float float_variable);
float bytes2float(uint8_t *ftoa_bytes_temp);

#endif // FLASH_H
