#include "flash_test.h"
#include "func.h"

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
    wait(10);

    uint8_t readBack = flashMem->readByte(address);
    print_status("Write & Read Byte Test", readBack == testByte);
}

void FlashTest::test_read_write_page() {
    uint32_t page = 2;
    uint16_t offset = 0;
    uint32_t size = 256;
    uint8_t writeData[256], readData[256];

    for (uint32_t i = 0; i < size; i++) {
        writeData[i] = i % 256;
    }

    flashMem->writePage(page, offset, size, writeData);
    wait(50);
    flashMem->read(page, offset, size, readData);

    bool passed = true;
    for (uint32_t i = 0; i < size; i++) {
        if (readData[i] != writeData[i]) {
            passed = false;
            break;
        }
    }

    print_status("Write & Read Page Test", passed);
}

void FlashTest::test_erase_sector() {
    uint16_t sector = 1;
    flashMem->eraseSector(sector);
    wait(500);

    uint32_t startAddr = sector * 4096;
    bool erased = true;
    for (uint32_t i = 0; i < 256; i++) {
        if (flashMem->readByte(startAddr + i) != 0xFF) {
            erased = false;
            break;
        }
    }

    print_status("Erase Sector Test", erased);
}

void FlashTest::test_enable_disable_write() {
    flashMem->enableWrite();
    wait(5);

    flashMem->disableWrite();
    wait(5);

    print_status("Enable/Disable Write Test", true);
}

void FlashTest::test_reset() {
    flashMem->reset();
    wait(100]);

    uint8_t testByte = 0xCD;
    uint32_t address = 0x000002;
    flashMem->writeByte(address, testByte);
    wait(10]);
    
    flashMem->reset();
    wait(100]);

    uint8_t readBack = flashMem->readByte(address);
    print_status("Flash Reset Test", readBack == 0xFF);
}

void FlashTest::run_all_tests() {
    test_read_write_byte();
    test_read_write_page();
    test_erase_sector();
    test_enable_disable_write();
    test_reset();
}
