#include "tmp102_test.h"
#include "func.h"

TMP102Test::TMP102Test(tmp102* sensor, USBSerial* serial) {
    this->sensor = sensor;
    this->pc = serial;
}

void TMP102Test::print_status(const char* test_name, bool passed) {
    pc->printf("[%s] %s\n", passed ? "PASS" : "FAIL", test_name);
}

void TMP102Test::test_set_get_Temperature() {
    uint16_t temp = sensor->getTemp();
    pc->printf("Temperature Reading: %d\n", temp);
}

void TMP102Test::test_set_get_ShutDown() {
    sensor->shutDown();
    wait(10);
    
    char config[2];
    sensor->readData(TMP102_CONFIG, config, 2);

    print_status("Shutdown Mode Test", (config[0] & 0x01) == 0x01);
    
    sensor->turnOn();
    wait(10);
    sensor->readData(TMP102_CONFIG, config, 2);

    print_status("Turn On Test", (config[0] & 0x01) == 0x00);
}

void TMP102Test::test_set_get_ComparatorMode() {
    sensor->setComparator();
    wait(10);
    
    char config[2];
    sensor->readData(TMP102_CONFIG, config, 2);
    
    print_status("Comparator Mode Test", (config[0] & 0x02) == 0x00);
}

void TMP102Test::test_set_get_InterruptMode() {
    sensor->setInterrupt();
    wait(10);
    
    char config[2];
    sensor->readData(TMP102_CONFIG, config, 2);
    
    print_status("Interrupt Mode Test", (config[0] & 0x02) == 0x02);
}

void TMP102Test::test_set_get_Polarity() {
    sensor->setPolarity(1);
    wait(10);
    
    char config[2];
    sensor->readData(TMP102_CONFIG, config, 2);
    print_status("Polarity High Test", (config[0] & 0x04) == 0x04);

    sensor->setPolarity(0);
    wait(10);
    sensor->readData(TMP102_CONFIG, config, 2);
    print_status("Polarity Low Test", (config[0] & 0x04) == 0x00);
}

void TMP102Test::test_set_get_FaultQueue() {
    sensor->setFaultQueue(2);
    wait(10);
    
    char config[2];
    sensor->readData(TMP102_CONFIG, config, 2);
    
    print_status("Fault Queue Test", ((config[0] >> 3) & 0x03) == 2);
}

void TMP102Test::test_set_get_ExtendedMode() {
    sensor->setEM(1);
    wait(10);
    
    char config[2];
    sensor->readData(TMP102_CONFIG, config, 2);
    
    print_status("Extended Mode Test", (config[1] & 0x10) == 0x10);

    sensor->setEM(0);
    wait(10);
    sensor->readData(TMP102_CONFIG, config, 2);
    
    print_status("Normal Mode Test", (config[1] & 0x10) == 0x00);
}

void TMP102Test::test_set_get_OneShot() {
    sensor->oneShot();
    wait(10);
    
    char config[2];
    sensor->readData(TMP102_CONFIG, config, 2);
    
    print_status("One Shot Test", (config[0] & 0x80) == 0x80);
}

void TMP102Test::test_set_get_ConversionRate() {
    sensor->setConversion(3);
    wait(10);
    
    char config[2];
    sensor->readData(TMP102_CONFIG, config, 2);
    
    print_status("Conversion Rate Test", ((config[1] >> 6) & 0x03) == 3);
}

void TMP102Test::test_set_get_HighLimit() {
    sensor->setHigh(30000);
    wait(10);

    char high[2];
    sensor->readData(TMP102_THIGH, high, 2);

    uint16_t high_value = (high[1] << 8) | high[0];
    print_status("High Limit Test", high_value == 30000);
}

void TMP102Test::test_set_get_LowLimit() {
    sensor->setLow(10000);
    wait(10);

    char low[2];
    sensor->readData(TMP102_TLOW, low, 2);

    uint16_t low_value = (low[1] << 8) | low[0];
    print_status("Low Limit Test", low_value == 10000);
}

void TMP102Test::test_set_get_Alert() {
    int alert = sensor->readAlert();
    pc->printf("Alert Value: %d\n", alert);
}

void TMP102Test::test_reset() {
    sensor->reset();
  
