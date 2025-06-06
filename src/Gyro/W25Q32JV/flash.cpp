#include "mbed.h"
#include "flash.h"

#ifndef FLASH_ENABLE_RESET
#define FLASH_ENABLE_RESET  0x66
#endif

#ifndef FLASH_RESET
#define FLASH_RESET         0x99
#endif

/**
 * Constructor: Initializes SPI interface and chip select pin.
 * @param mosi - SPI MOSI pin
 * @param miso - SPI MISO pin
 * @param sclk - SPI Clock pin
 * @param csPin - Chip Select pin
 */
flash::flash(PinName mosi, PinName miso, PinName sclk, PinName csPin)
    : _spi(mosi, miso, sclk), _cs(csPin, 1) {
    _spi.format(8, 0);           // 8-bit frame, mode 0
    _spi.frequency(1000000);     // 1 MHz SPI clock
}

/**
 * Drives chip select (CS) line low to initiate communication.
 */
void flash::csLow() {
    _cs = 0;
    wait_us(5); // Chip select stabilization delay
}

/**
 * Releases chip select (CS) line to end communication.
 */
void flash::csHigh() {
    wait_us(5); // Deselect stabilization delay
    _cs = 1;
}

bool flash::isDone(uint32_t timeout_ms) {
    const uint8_t READ_STATUS = 0x05;
    char tx_rx[2];
    uint8_t status = 0;

    uint32_t elapsed = 0;
    const uint32_t sleep_step = 1;

    while (elapsed < timeout_ms) {
        tx_rx[0] = READ_STATUS;
        tx_rx[1] = 0x00;

        csLow();
        _spi.write(tx_rx, 2, tx_rx, 2);
        csHigh();

        status = tx_rx[1];
        if ((status & 0x01) == 0) {
            return true;
        }

        ThisThread::sleep_for(sleep_step);
        elapsed += sleep_step;
    }

    return false;
}
/**
 * Writes a buffer of data to a specific address in flash memory.
 * @param address - 24-bit target address
 * @param buffer - Pointer to data buffer
 * @param length - Number of bytes to write
 */
void flash::write(uint32_t address, const uint8_t *buffer, size_t length) {
    const size_t PAGE_SIZE = 256;

    while (length > 0) {
        enableWrite();

        uint32_t page_offset = address % PAGE_SIZE;
        uint32_t bytes_to_page_end = PAGE_SIZE - page_offset;
        size_t chunk = (length < bytes_to_page_end) ? length : bytes_to_page_end;

        uint8_t cmd[4] = {
            0x02,
            static_cast<uint8_t>((address >> 16) & 0xFF),
            static_cast<uint8_t>((address >> 8) & 0xFF),
            static_cast<uint8_t>(address & 0xFF)
        };

        csLow();
        _spi.write((const char *)cmd, 4, nullptr, 0);
        _spi.write((const char *)buffer, chunk, nullptr, 0);
        csHigh();

        if (!isDone(100)) {
            return;
        }

        address += chunk;
        buffer += chunk;
        length -= chunk;
    }
}

/**
 * Reads data from a specific address in flash memory.
 * @param address - 24-bit source address
 * @param buffer - Buffer to store read data
 * @param length - Number of bytes to read
 */
void flash::read(uint32_t address, uint8_t *buffer, size_t length) {
    uint8_t cmd[4];
    cmd[0] = 0x03; // Read Data command
    cmd[1] = (address >> 16) & 0xFF;
    cmd[2] = (address >> 8) & 0xFF;
    cmd[3] = address & 0xFF;

    csLow();
    _spi.write((const char *)cmd, 4, NULL, 0);
    _spi.write(NULL, 0, (char *)buffer, length); // Only receive data
    csHigh();
}

/**
 * Reads a single byte from flash memory.
 * @param address - Address to read from
 * @return Value of the byte
 */
uint8_t flash::readByte(uint32_t address) {
    uint8_t data = 0xFF;
    read(address, &data, 1);
    return data;
}

/**
 * Writes a single byte to flash memory.
 * @param address - Address to write to
 * @param data - Byte value to write
 */
void flash::writeByte(uint32_t address, uint8_t data) {
    write(address, &data, 1);
}

/**
 * Erases a 4KB sector at the given address.
 * @param address - Address within the sector to erase
 */
void flash::eraseSector(uint32_t address) {
    enableWrite();

    uint8_t cmd[4];
    cmd[0] = 0x20; // Sector Erase command
    cmd[1] = (address >> 16) & 0xFF;
    cmd[2] = (address >> 8) & 0xFF;
    cmd[3] = address & 0xFF;

    csLow();
    _spi.write((const char *)cmd, 4, NULL, 0);
    csHigh();

    isDone(4000);
}

void flash::eraseAll() {
    for (uint32_t addr = 0; addr < 0x400000; addr += 0x1000) {
        eraseSector(addr);
    }
}


/**
 * Sends Write Enable command to allow write/erase operations.
 */
void flash::enableWrite() {
    uint8_t cmd = 0x06; // Write Enable
    csLow();
    _spi.write((const char *)&cmd, 1, NULL, 0);
    csHigh();
    uint8_t status = 0;
    do {
        uint8_t readCmd = 0x05;
        csLow();
        _spi.write((const char *)&readCmd, 1, NULL, 0);
        _spi.write(NULL, 0, (char *)&status, 1);
        csHigh();
        ThisThread::sleep_for(1ms);
    } while (!(status & 0x02)); // Wait until WEL is set
}

/**
 * Sends Write Disable command to block write operations.
 */
void flash::disableWrite() {
    uint8_t cmd = 0x04; // Write Disable
    csLow();
    _spi.write((const char *)&cmd, 1, NULL, 0);
    csHigh();
    uint8_t status = 0;
    do {
        uint8_t readCmd = 0x05;
        csLow();
        _spi.write((const char *)&readCmd, 1, NULL, 0);
        _spi.write(NULL, 0, (char *)&status, 1);
        csHigh();
        ThisThread::sleep_for(1ms);
    } while (status & 0x02); // Wait until WEL is cleared
}

/**
 * Resets the flash chip using the two-command reset sequence.
 */
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

    ThisThread::sleep_for(100ms); // Wait for reset completion
}

/**
 * Writes a 32-bit float to flash memory at specified address.
 * @param address - Flash memory address
 * @param data - Float value to store
 */
void flash::writeNum(uint32_t address, float data) {
    uint8_t tempBytes[4];
    float2Byte(tempBytes, data);
    write(address, tempBytes, 4);
}

/**
 * Reads a 32-bit float from flash memory.
 * @param address - Flash memory address
 * @return Float value read from memory
 */
float flash::readNum(uint32_t address) {
    uint8_t rData[4];
    read(address, rData, 4);
    return bytes2float(rData);
}

/**
 * Converts a float value into a byte array (little endian).
 * @param ftoa_bytes_temp - Output byte array
 * @param float_variable - Input float value
 */
void float2Byte(uint8_t *ftoa_bytes_temp, float float_variable) {
    union {
        float f;
        uint8_t b[4];
    } conv;
    conv.f = float_variable;
    memcpy(ftoa_bytes_temp, conv.b, 4);
}

/**
 * Converts a byte array into a float value.
 * @param ftoa_bytes_temp - Input byte array (4 bytes)
 * @return Float reconstructed from byte array
 */
float bytes2float(uint8_t *ftoa_bytes_temp) {
    union {
        float f;
        uint8_t b[4];
    } conv;
    memcpy(conv.b, ftoa_bytes_temp, 4);
    return conv.f;
}
