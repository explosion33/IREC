#ifndef FLASH_H
#define FLASH_H

#include "mbed.h"

class flash {
public:
    flash(PinName mosi, PinName miso, PinName sclk, PinName csPin);
    void csLow();
    void csHigh();
    void read(uint32_t startPage, uint8_t offset, uint32_t size, uint8_t *rData);
    void fastRead(uint32_t startPage, uint8_t offset, uint32_t size, uint8_t *rData);
    void enableWrite();
    void disableWrite();
    void eraseSector(uint16_t sector);
    void write(uint32_t page, uint16_t offset, uint32_t size, uint8_t *data);
    uint8_t readByte(uint32_t Addr);
    void writeByte(uint32_t Addr, uint8_t data);
    void writePage(uint32_t page, uint16_t offset, uint32_t size, uint8_t *data);
    void reset();

private:
    uint32_t bytestowrite(uint32_t size, uint16_t offset);
    uint16_t bytestomodify(uint32_t size, uint32_t offset);
    SPI _spi;
    DigitalOut _cs;
};

void float2Byte(uint8_t *ftoa_bytes_temp, float float_variable);
float bytes2float(uint8_t *ftoa_bytes_temp);
void writeNum(uint32_t page, uint16_t offset, float data);
float readNum(uint32_t page, uint16_t offset);

#endif