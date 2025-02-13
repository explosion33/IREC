#include "mbed.h"
#include "flash.h"

#ifndef FLASH_ENABLE_RESET
#define FLASH_ENABLE_RESET  0x66
#endif
#ifndef FLASH_RESET
#define FLASH_RESET         0x99
#endif

flash::flash(PinName mosi, PinName miso, PinName sclk, PinName csPin)
    : _spi(mosi, miso, sclk), _cs(csPin, 1) {
    _spi.format(8, 0);
    _spi.frequency(1000000);
}

void flash::csLow() {
    _cs = 0;
}

void flash::csHigh() {
    _cs = 1;
}

void flash::read(uint32_t startPage, uint8_t offset, uint32_t size, uint8_t *rData) {
    uint8_t tData[4];
    uint32_t memAddr = (startPage * 256) + offset;
    tData[0] = 0x03;
    tData[1] = (memAddr >> 16) & 0xFF;
    tData[2] = (memAddr >> 8) & 0xFF;
    tData[3] = memAddr & 0xFF;
    csLow();
    _spi.write((const char *)tData, 4, (char *)rData, size);
    csHigh();
}

void flash::fastRead(uint32_t startPage, uint8_t offset, uint32_t size, uint8_t *rData) {
    uint8_t tData[4];
    uint32_t memAddr = (startPage * 256) + offset;
    tData[0] = 0x0B;
    tData[1] = (memAddr >> 16) & 0xFF;
    tData[2] = (memAddr >> 8) & 0xFF;
    tData[3] = memAddr & 0xFF;
    csLow();
    _spi.write((const char *)tData, 4, (char *)rData, size);
    csHigh();
}

void flash::enableWrite() {
    uint8_t cmd = 0x06;
    csLow();
    _spi.write(cmd);
    csHigh();
    ThisThread::sleep_for(5ms);
}

void flash::disableWrite() {
    uint8_t cmd = 0x04;
    csLow();
    _spi.write(cmd);
    csHigh();
    ThisThread::sleep_for(5ms);
}

void flash::eraseSector(uint16_t sector) {
    uint8_t tData[4];
    uint32_t memAddr = (uint32_t)sector * 4096;
    enableWrite();
    tData[0] = 0x20;
    tData[1] = (memAddr >> 16) & 0xFF;
    tData[2] = (memAddr >> 8) & 0xFF;
    tData[3] = memAddr & 0xFF;
    csLow();
    _spi.write((const char *)tData, 4, NULL, 0);
    csHigh();
    ThisThread::sleep_for(450ms);
    disableWrite();
}

void flash::write(uint32_t page, uint16_t offset, uint32_t size, uint8_t *data) {
    uint16_t startSector = page / 16;
    uint16_t endSector = (page + ((size + offset - 1) / 256)) / 16;
    uint16_t numSectors = endSector - startSector + 1;
    uint8_t previousData[4096];
    uint32_t sectorOffset = ((page % 16) * 256) + offset;
    uint32_t dataIndex = 0;
    for (uint16_t s = 0; s < numSectors; s++) {
        uint32_t startPage = startSector * 16;
        fastRead(startPage, 0, 4096, previousData);
        uint16_t bytesRemaining = bytestomodify(size, sectorOffset);
        for (uint16_t i = 0; i < bytesRemaining; i++) {
            previousData[i + sectorOffset] = data[i + dataIndex];
        }
        writePage(startPage, 0, 4096, previousData);
        startSector++;
        sectorOffset = 0;
        dataIndex += bytesRemaining;
        size -= bytesRemaining;
    }
}

uint8_t flash::readByte(uint32_t Addr) {
    uint8_t cmd[4];
    uint8_t rData = 0x00;
    cmd[0] = 0x0B;
    cmd[1] = (Addr >> 16) & 0xFF;
    cmd[2] = (Addr >> 8) & 0xFF;
    cmd[3] = Addr & 0xFF;
    csLow();
    _spi.write((const char *)cmd, 4, (char *)&rData, 1);
    csHigh();
    return rData;
}

void flash::writeByte(uint32_t Addr, uint8_t data) {
    uint8_t tData[5];
    uint8_t length = 5;
    tData[0] = 0x02;
    tData[1] = (Addr >> 16) & 0xFF;
    tData[2] = (Addr >> 8) & 0xFF;
    tData[3] = Addr & 0xFF;
    tData[4] = data;
    if (readByte(Addr) == 0xFF) {
        enableWrite();
        csLow();
        _spi.write((const char *)tData, length, NULL, 0);
        csHigh();
        ThisThread::sleep_for(5ms);
        disableWrite();
    }
}

void flash::writePage(uint32_t page, uint16_t offset, uint32_t size, uint8_t *data) {
    uint32_t startPage = page;
    uint32_t endPage = page + ((size + offset - 1) / 256);
    uint32_t numPages = endPage - startPage + 1;
    uint16_t startSector = startPage / 16;
    uint16_t endSector = endPage / 16;
    uint16_t numSectors = endSector - startSector + 1;
    for (uint16_t s = 0; s < numSectors; s++) {
        eraseSector(startSector + s);
    }
    uint32_t dataPos = 0;
    uint8_t tData[260];
    for (uint32_t p = 0; p < numPages; p++) {
        uint32_t memAddr = (startPage * 256) + offset;
        uint16_t bytesThisPage = bytestowrite(size, offset);
        enableWrite();
        tData[0] = 0x02;
        tData[1] = (memAddr >> 16) & 0xFF;
        tData[2] = (memAddr >> 8) & 0xFF;
        tData[3] = memAddr & 0xFF;
        for (uint16_t i = 0; i < bytesThisPage; i++) {
            tData[4 + i] = data[dataPos + i];
        }
        csLow();
        _spi.write((const char *)tData, 4 + bytesThisPage, NULL, 0);
        csHigh();
        ThisThread::sleep_for(5ms);
        disableWrite();
        startPage++;
        offset = 0;
        size -= bytesThisPage;
        dataPos += bytesThisPage;
    }
}

uint32_t flash::bytestowrite(uint32_t size, uint16_t offset) {
    if ((size + offset) <= 256) {
        return size;
    } else {
        return (256 - offset);
    }
}

uint16_t flash::bytestomodify(uint32_t size, uint32_t offset) {
    if ((size + offset) <= 4096) {
        return size;
    } else {
        return (4096 - offset);
    }
}

void flash::reset() {
    uint8_t data[2];
    data[0] = FLASH_ENABLE_RESET;
    data[1] = FLASH_RESET;
    csLow();
    _spi.write((const char *)data, 2, NULL, 0);
    csHigh();
    ThisThread::sleep_for(100ms);
}

void float2Byte(uint8_t *ftoa_bytes_temp, float float_variable) {
    union {
        float f;
        uint8_t b[4];
    } conv;
    conv.f = float_variable;
    for (int i = 0; i < 4; i++) {
        ftoa_bytes_temp[i] = conv.b[i];
    }
}

float bytes2float(uint8_t *ftoa_bytes_temp) {
    union {
        float f;
        uint8_t b[4];
    } conv;
    for (int i = 0; i < 4; i++) {
        conv.b[i] = ftoa_bytes_temp[i];
    }
    return conv.f;
}

void writeNum(uint32_t page, uint16_t offset, float data) {
    uint8_t tempBytes[4];
    float2Byte(tempBytes, data);
}

float readNum(uint32_t page, uint16_t offset) {
    uint8_t rData[4];
    return 0.0f;
}
