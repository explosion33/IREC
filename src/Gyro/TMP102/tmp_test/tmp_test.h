#ifndef TMP102_TEST_H
#define TMP102_TEST_H

#include "mbed.h"
#include "tmp102.h"
#include "USBSerial.h"

class TMP102Test {
public:
    // Constructor
    TMP102Test(tmp102* sensor, USBSerial* serial);

    // Test Functions
    void test_set_get_Temperature();
    void test_set_get_ShutDown();
    void test_set_get_ComparatorMode();
    void test_set_get_InterruptMode();
    void test_set_get_Polarity();
    void test_set_get_FaultQueue();
    void test_set_get_ExtendedMode();
    void test_set_get_OneShot();
    void test_set_get_ConversionRate();
    void test_set_get_HighLimit();
    void test_set_get_LowLimit();
    void test_set_get_Alert();
    void test_reset();
    void run_all_tests();

private:
    tmp102* sensor;
    USBSerial* pc;

    void print_status(const char* test_name, bool passed);
};

#endif // TMP102_TEST_H
