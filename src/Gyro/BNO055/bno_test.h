#ifndef BNO055_TEST_H
#define BNO055_TEST_H

#include "mbed.h"
#include "BNO055.h"
#include "USBSerial.h"

class BNO055Test {
public:
    // Constructor
    BNO055Test(BNO055* sensor, USBSerial* serial);

    // Test Functions
    void test_set_get_OPMode();
    void test_set_get_PWRMode();
    void test_set_get_ACCConfig();
    void test_set_get_GYROConfig();
    void test_set_get_MAGConfig();
    void test_set_get_Axes();
    void test_set_get_TemperatureSource();
    void test_set_get_UnitConfig();
    void test_set_get_SysTrigger();
    void test_reset();
    void run_all_tests();
    void Dummy();
    void test_page();

private:
    BNO055* sensor;
    USBSerial* pc;

    void print_status(const char* test_name, bool passed);
};

#endif // BNO055_TEST_H
