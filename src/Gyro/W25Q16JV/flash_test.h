#ifndef FLASH_TEST_H
#define FLASH_TEST_H

#include "mbed.h"
#include "flash.h"
#include "USBSerial.h"

class FlashTest {
public:
    // Constructor
    FlashTest(flash* flashMem, USBSerial* serial);

    // Test Functions
    void test_read_write_byte();
    void test_read_write_page();
    void test_erase_sector();
    void test_enable_disable_write();
    void test_reset();
    void run_all_tests();

private:
    flash* flashMem;
    USBSerial* pc;

    void print_status(const char* test_name, bool passed);
};

#endif // FLASH_TEST_H
