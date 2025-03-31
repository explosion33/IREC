#ifndef FLASH_TEST_H
#define FLASH_TEST_H

#include "mbed.h"
#include "flash.h"
#include "USBSerial.h"

class FlashTest {
public:
    // Constructor
    FlashTest(flash* flashMem, USBSerial* serial);

    // Runs all tests
    void run_all_tests();

    // Individual test functions
    void test_read_write_byte();
    void test_read_write_page();
    void test_erase_sector();
    void test_enable_disable_write();
    void test_reset();
    void test_read_write_float();

private:
    // Helper function to print test results
    void print_status(const char* test_name, bool passed);

    // Pointer to the flash memory instance
    flash* flashMem;

    // Pointer to USB serial output for logging
    USBSerial* pc;
};

#endif // FLASH_TEST_H
