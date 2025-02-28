#include "bno055_test.h"
#include "func.h"

BNO055Test::BNO055Test(BNO055* sensor, USBSerial* serial) {
    this->sensor = sensor;
    this->pc = serial;
}

void BNO055Test::print_status(const char* test_name, bool passed) {
    pc->printf("[%s] %s\n", passed ? "PASS" : "FAIL", test_name);
}

void BNO055Test::test_set_get_OPMode() {
    char expected_mode = BNO055_OPERATION_MODE_IMU;
    sensor->setOPMode(expected_mode);
    wait(10);
    char actual_mode = sensor->getOPMode();
    print_status("OP Mode Test", actual_mode == expected_mode);
}

void BNO055Test::test_set_get_PWRMode() {
    PWRMode expected_mode = PWRMode::LowPower;
    sensor->setPWR(expected_mode);
    wait(10);
    char actual_mode;
    sensor->readData(BNO055_PWR_MODE, &actual_mode, 1);
    print_status("Power Mode Test", actual_mode == expected_mode);
}

void BNO055Test::test_set_get_ACCConfig() {
    char expected_config = 0x0D;
    sensor->setACC(0x04, 0x02, 0x01);
    wait(10);
    char actual_config;
    sensor->readData(BNO055_ACC_CONFIG, &actual_config, 1);
    print_status("Accelerometer Config Test", actual_config == expected_config);
}

void BNO055Test::test_set_get_GYROConfig() {
    char expected_config = 0x07;
    sensor->setGYR(0x03, 0x02, 0x02);
    wait(10);
    char actual_config;
    sensor->readData(BNO055_GYRO_CONFIG_0, &actual_config, 1);
    print_status("Gyroscope Config Test", actual_config == expected_config);
}

void BNO055Test::test_set_get_MAGConfig() {
    char expected_config = 0x01;
    sensor->setMAG(0x01, 0x00, 0x00);
    wait(10);
    char actual_config;
    sensor->readData(BNO055_MAG_CONFIG, &actual_config, 1);
    print_status("Magnetometer Config Test", actual_config == expected_config);
}

void BNO055Test::test_set_get_Axes() {
    sensor->setAxes(Axes::Z, Axes::X, Axes::Y, false, false, false);
    wait(10);
    char actual_axes;
    sensor->readData(BNO055_AXIS_MAP_CONFIG, &actual_axes, 1);
    char expected_axes = 0b00101000;
    print_status("Axis Mapping Test", actual_axes == expected_axes);
}

void BNO055Test::test_set_get_TemperatureSource() {
    sensor->writeData(BNO055_TEMP_SOURCE, 0x01, 1);
    wait(10);
    char actual_source;
    sensor->readData(BNO055_TEMP_SOURCE, &actual_source, 1);
    print_status("Temperature Source Test", actual_source == 0x01);
}

void BNO055Test::test_set_get_UnitConfig() {
    sensor->setUnit(true, false, true, false, true);
    wait(10);
    char actual_config;
    sensor->readData(BNO055_UNIT_SEL, &actual_config, 1);
    char expected_config = 0b00010101;
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
    char sys_status = sensor->get_SysStatus();
    print_status("Software Reset Test", sys_status == 0x01);
}

void BNO055Test::run_all_tests() {
    test_set_get_OPMode();
    test_set_get_PWRMode();
    test_set_get_ACCConfig();
    test_set_get_GYROConfig();
    test_set_get_MAGConfig();
    test_set_get_Axes();
    test_set_get_TemperatureSource();
    test_set_get_UnitConfig();
    test_set_get_SysTrigger();
    test_reset();
}
