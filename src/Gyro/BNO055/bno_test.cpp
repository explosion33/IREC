#include "bno_test.h"
#include "func.h"
#include "bno055_const.h"

BNO055Test::BNO055Test(BNO055* sensor, USBSerial* serial) {
    this->sensor = sensor;
    this->pc = serial;
}

void BNO055Test::print_status(const char* test_name, bool passed) {
    pc->printf("[%s] %s\n", passed ? "PASS" : "FAIL", test_name);
}

void BNO055Test::test_page(){
    char expected_page = 0x01;
    char page;
    sensor->writeData(0x07, 0x01, 1);
    sensor->readData(0x07, &page, 1);
    print_status("Page Test", expected_page == page);
}

void BNO055Test::test_set_get_OPMode() {
    char expected_mode = BNO055_OPERATION_MODE_NDOF;
    sensor->setOPMode(expected_mode);
    wait(10);
    char actual_mode = sensor->getOPMode();
    print_status("OP Mode Test", actual_mode == expected_mode);
}

void BNO055Test::test_set_get_PWRMode() {
    char expected_mode = 0x02;
    sensor->setPWR(PWRMode::Suspend);
    wait(10);
    char actual_mode;
    sensor->readData(BNO055_PWR_MODE, &actual_mode, 1);
    pc->printf("%x", actual_mode);
    print_status("Power Mode Test", actual_mode == expected_mode);
}

void BNO055Test::test_set_get_ACCConfig() {
    sensor-> setOPMode(0x00);
    char expected_config = 0x00;
    sensor->setACC(0x00, 0x00, 0x00);
    wait(10);
    char actual_config;
    sensor->readData(BNO055_ACC_CONFIG, &actual_config, 1);
    print_status("Accelerometer Config Test", actual_config == expected_config);
}

void BNO055Test::test_set_get_GYROConfig() {
    sensor-> setOPMode(0x00);
    char expected_config = 0x00;
    sensor->setGYR(0x00, 0x00, 0x00);
    wait(10);
    char actual_config;
    sensor->readData(BNO055_GYRO_CONFIG_0, &actual_config, 1);
    print_status("Gyroscope Config Test", actual_config == expected_config);
}

void BNO055Test::test_set_get_MAGConfig() {
    sensor-> setOPMode(0x00);
    char expected_config = 0x00;
    sensor->setMAG(0x00, 0x00, 0x00);
    wait(10);
    char actual_config;
    sensor->readData(BNO055_MAG_CONFIG, &actual_config, 1);
    print_status("Magnetometer Config Test", actual_config == expected_config);
}

void BNO055Test::test_set_get_Axes() {
    sensor->setAxes(Axes::Z, Axes::X, Axes::Y, true, true, true);
    wait(10);
    char actual_axes;
    sensor->readData(BNO055_AXIS_MAP_CONFIG, &actual_axes, 1);
    char expected_axes = 0b00010010;

    char actual_sign;
    sensor ->readData(BNO055_AXIS_MAP_SIGN, &actual_sign, 1);
    char expected_sign = 0b00000111;
    print_status("Axis Mapping Test", actual_axes == expected_axes);
    print_status("Axis Sign Test", actual_sign == expected_sign);
}

void BNO055Test::test_set_get_TemperatureSource() {
    sensor->writeData(BNO055_TEMP_SOURCE, 0x01, 1);
    wait(10);
    char actual_source;
    sensor->readData(BNO055_TEMP_SOURCE, &actual_source, 1);
    print_status("Temperature Source Test", actual_source == 0x01);
}

void BNO055Test::test_set_get_UnitConfig() {
    sensor->setOPMode(0x00);
    sensor->setUnit(true, false, true, false, true);
    wait(10);
    char actual_config;
    sensor->readData(BNO055_UNIT_SEL, &actual_config, 1);
    char expected_config = 0b10000101;
    print_status("Unit Config Test", actual_config == expected_config);
}

void BNO055Test::test_set_get_SysTrigger() {
    sensor->writeData(BNO055_SYS_TRIGGER, 0x10, 1);
    wait(10);
    char actual_value;
    sensor->readData(BNO055_SYS_TRIGGER, &actual_value, 1);
    print_status("System Trigger Test", actual_value == 0x10);
}

void BNO055Test::test_reset() {
    sensor->reset();
    wait(700);
    char sys_status;
    sensor->readData(BNO055_SYS_TRIGGER, &sys_status, 1);
    print_status("Software Reset Test", sys_status == 0x00);
}

void BNO055Test::run_all_tests() {
    // test_page(); pass
    // test_set_get_OPMode(); PASS
    // test_set_get_PWRMode(); PASS
    // test_set_get_ACCConfig(); PASS
    // test_set_get_GYROConfig(); PASS
    // test_set_get_MAGConfig(); PASS
    // test_set_get_Axes(); PASS
    // test_set_get_TemperatureSource(); PASS
    // test_set_get_UnitConfig(); PASS
    // test_set_get_SysTrigger(); PASS
    // test_reset(); PASS
}
