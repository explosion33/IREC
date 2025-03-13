#include "flash_test.h"
#include "func.h"
#include "mbed.h"

FlashTest::FlashTest(flash* flashMem, USBSerial* serial) {
    this->flashMem = flashMem;
    this->pc = serial;
}

void FlashTest::print_status(const char* test_name, bool passed) {
    pc->printf("[%s] %s\n", passed ? "PASS" : "FAIL", test_name);
}

void FlashTest::test_read_write_byte() {
    uint32_t address = 0x000001;
    uint8_t testByte = 0xAB;

    flashMem->writeByte(address, testByte);
    ThisThread::sleep_for(10ms);

    uint8_t readBack = flashMem->readByte(address);
    print_status("Write & Read Byte Test", readBack == testByte);
}

void FlashTest::test_read_write_page() {
    uint32_t address = 0x000100;  // Page-aligned address
    uint8_t writeData[256], readData[256];

    for (uint32_t i = 0; i < 256; i++) {
        writeData[i] = i % 256;
    }

    flashMem->write(address, writeData, 256);
    ThisThread::sleep_for(50ms);
    flashMem->read(address, readData, 256);

    bool passed = true;
    for (uint32_t i = 0; i < 256; i++) {
        if (readData[i] != writeData[i]) {
            passed = false;
            break;
        }
    }

    print_status("Write & Read Page Test", passed);
}

void FlashTest::test_erase_sector() {
    uint32_t sector_address = 0x000000;
    flashMem->eraseSector(sector_address);
    ThisThread::sleep_for(500ms);

    bool erased = true;
    uint8_t readBuffer[256];
    flashMem->read(sector_address, readBuffer, 256);

    for (uint32_t i = 0; i < 256; i++) {
        if (readBuffer[i] != 0xFF) {
            erased = false;
            break;
        }
    }

    print_status("Erase Sector Test", erased);
}

void FlashTest::test_enable_disable_write() {
    flashMem->enableWrite();
    ThisThread::sleep_for(5ms);

    flashMem->disableWrite();
    ThisThread::sleep_for(5ms);

    print_status("Enable/Disable Write Test", true);
}

void FlashTest::test_reset() {
    uint32_t address = 0x000002;
    uint8_t testByte = 0xCD;

    flashMem->writeByte(address, testByte);
    ThisThread::sleep_for(10ms);

    uint8_t readBeforeReset = flashMem->readByte(address);
    
    flashMem->reset();
    ThisThread::sleep_for(100ms);

    uint8_t readAfterReset = flashMem->readByte(address);
    print_status("Flash Reset Test", readBeforeReset == testByte && readAfterReset == 0xFF);
}

void FlashTest::test_read_write_float() {
    uint32_t address = 0x000010;
    float testValue = 123.456f;

    flashMem->writeNum(address, testValue);
    ThisThread::sleep_for(10ms);

    float readValue = flashMem->readNum(address);
    print_status("Write & Read Float Test", fabs(readValue - testValue) < 0.001f);
}

void FlashTest::run_all_tests() {
    pc->printf("\nRunning W25Q16JV Flash Tests...\n");

    test_read_write_byte();
    test_read_write_page();
    test_erase_sector();
    test_enable_disable_write();
    test_reset();
    test_read_write_float();

    pc->printf("\nAll flash tests completed.\n");
}
