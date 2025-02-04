#include "mbed.h"
#include "BNO055.h"
#include "bno055_const.h"
#include <map>
#include "func.h"

DigitalOut rst(PA_5);

BNO055::BNO055(PinName SDA, PinName SCL, char addr) {
    owned = true;
    BNO055::i2c = new I2C(SDA, SCL);
    BNO055::addr = addr;
}

BNO055::BNO055(I2C* i2c) {
    BNO055::i2c = i2c;
}

BNO055::~BNO055() {
    if (owned) {
        delete i2c;
    }
}

void BNO055::dummy(){

}

int BNO055::readData(char regaddr, char* data, uint8_t len) {
    i2c->write(addr, &regaddr, 1);
    return i2c->read(addr, data, len);
}

int BNO055::writeData(char regaddr, char data, uint8_t len) {
    char buffer[2];
    buffer[0] = regaddr;
    buffer[1] = data;
    return i2c->write(addr, buffer, 2);
}

int BNO055::getSysCalib(){ //returns 0 if functional
    char stat;
    readData(BNO055_CALIB_STAT, &stat, 1);
    if (((stat >> 6) & 0x03) != 3){
        return 1;
    }
    return 0;
}

int BNO055::getGyrCalib(){
    char stat;
    readData(BNO055_CALIB_STAT, &stat, 1);
    if (((stat >> 4) & 0x03) != 3){
        return 1;
    }
    return 0;
}

int BNO055::getAccCalib(){
    char stat;
    readData(BNO055_CALIB_STAT, &stat, 1);
    if (((stat >> 2) & 0x03) != 3){
        return 1;
    }
    return 0;
}

int BNO055::getMagCalib(){
    char stat;
    readData(BNO055_CALIB_STAT, &stat, 1);
    if (((stat) & 0x03) != 3){
        return 1;
    }
    return 0;
}

void BNO055::setPWR(PWRMode mode) { //test
    char modeData = 0x00;
    switch(mode) {
        case PWRMode::Normal:   modeData = 0x00; break;
        case PWRMode::LowPower: modeData = 0x01; break;
        case PWRMode::Suspend:  modeData = 0x02; break;
    }
    writeData(BNO055_PWR_MODE, modeData, 1);
}

char BNO055::getOPMode() {  
    setPage(0);
    char mode = 0;
    readData(BNO055_OPR_MODE, &mode, 1);
    return mode;
}

void BNO055::setOPMode(char mode) {
    setPage(0);
    writeData(BNO055_OPR_MODE, mode, 1);
    if (mode == BNO055_OPERATION_MODE_CONFIG) {
        wait(19);
    } else {
        wait(7);
    }
}

void BNO055::setACC(char GRange, char Bandwidth, char OPMode) { //todo: test all configuration stuff
    setPage(0);
    char config = GRange | Bandwidth | OPMode;
    writeData(BNO055_ACC_CONFIG, config, 1);
    wait(20);
}

void BNO055::setGYR(char Range, char Bandwidth, char OPMode) {
    setPage(0);
    char config0 = Range | Bandwidth;
    char config1 = OPMode;
    writeData(BNO055_GYRO_CONFIG_0, config0, 1);
    wait(20);
    writeData(BNO055_GYRO_CONFIG_1, config1, 1);
    wait(20);
}

void BNO055::setMAG(char Rate, char OPMode, char Power) {
    setPage(0);
    char config = Rate | OPMode | Power;
    writeData(BNO055_MAG_CONFIG, config, 1);
    wait(20);
}

void BNO055::setPage(uint8_t page) { // test
    char pageChar = static_cast<char>(page);
    writeData(BNO055_PAGE_ID, pageChar, 1);
}

void BNO055::setCLK(bool external) {
    setPage(0);
    char tmp = 0x00;
    readData(BNO055_SYS_TRIGGER, &tmp, 1);
    tmp |= external ? 0x80 : 0x00;
    writeData(BNO055_SYS_TRIGGER, tmp, 1);
    wait(700);
}

void BNO055::setUnit(bool acc, bool angular, bool euler, bool temp, bool fusion) {
    setPage(0);
    char config = 0x00;
    config |= acc;
    config |= (angular << 1);
    config |= (euler << 2);
    config |= (temp << 4);
    config |= (fusion << 7);
    writeData(BNO055_UNIT_SEL, config, 1);
    wait(20);
}

void BNO055::reset() {
    char resetVal = 0x20;
    writeData(BNO055_SYS_TRIGGER, resetVal, 1);
    wait(700);
}

void BNO055::nReset() {
    rst = 0;
    wait(500);
    rst = 1;
    wait(500);
}

void BNO055::setAxes(Axes newX, Axes newY, Axes newZ, bool xNeg, bool yNeg, bool zNeg) {
    char axes = getAxes(newX, newY, newZ);
    writeData(BNO055_AXIS_MAP_CONFIG, axes, 1);
    wait(20);
}

char BNO055::getAxes(Axes newX, Axes newY, Axes newZ) {
    static std::map<Axes, char> axesMap = {
        {Axes::X, 0x00},
        {Axes::Y, 0x01},
        {Axes::Z, 0x02}
    };
    char axes = 0x00;
    axes |= axesMap[newX];
    axes |= (axesMap[newY] << 2);
    axes |= (axesMap[newZ] << 4);
    return axes;
}

char BNO055::getAxesSign(bool xNeg, bool yNeg, bool zNeg) {
    char sign = 0x00;
    sign |= (xNeg ? 0x01 : 0x00);
    sign |= (yNeg ? 0x01 : 0x00);
    sign |= (zNeg ? 0x01 : 0x00);
    return sign;
}

void BNO055::setACCOffset(uint16_t offsetX, uint16_t offsetY, uint16_t offsetZ) {
    char lsbX  = (char)(offsetX & 0xFF); 
    char msbX = (char)((offsetX >> 8) & 0xFF);
    writeData(BNO055_ACC_OFFSET_X_LSB, lsbX, 1);
    writeData(BNO055_ACC_OFFSET_X_MSB, msbX, 1);

    char lsbY  = (char)(offsetY & 0xFF); 
    char msbY = (char)((offsetY >> 8) & 0xFF);
    writeData(BNO055_ACC_OFFSET_Y_LSB, lsbY, 1);
    writeData(BNO055_ACC_OFFSET_Y_MSB, msbY, 1);

    char lsbZ  = (char)(offsetZ & 0xFF); 
    char msbZ = (char)((offsetZ >> 8) & 0xFF);
    writeData(BNO055_ACC_OFFSET_Z_LSB, lsbZ, 1);
    writeData(BNO055_ACC_OFFSET_Z_MSB, msbZ, 1);
}

void BNO055::setMAGOffset(uint16_t offsetX, uint16_t offsetY, uint16_t offsetZ) {
    char lsbX  = (char)(offsetX & 0xFF); 
    char msbX = (char)((offsetX >> 8) & 0xFF);
    writeData(BNO055_MAG_OFFSET_X_LSB, lsbX, 1);
    writeData(BNO055_MAG_OFFSET_X_MSB, msbX, 1);

    char lsbY  = (char)(offsetY & 0xFF); 
    char msbY = (char)((offsetY >> 8) & 0xFF);
    writeData(BNO055_MAG_OFFSET_Y_LSB, lsbY, 1);
    writeData(BNO055_MAG_OFFSET_Y_MSB, msbY, 1);

    char lsbZ  = (char)(offsetZ & 0xFF); 
    char msbZ = (char)((offsetZ >> 8) & 0xFF);
    writeData(BNO055_MAG_OFFSET_Z_LSB, lsbZ, 1);
    writeData(BNO055_MAG_OFFSET_Z_MSB, msbZ, 1); 
}

void BNO055::setGYROffset(uint16_t offsetX, uint16_t offsetY, uint16_t offsetZ) {
    char lsbX  = (char)(offsetX & 0xFF); 
    char msbX = (char)((offsetX >> 8) & 0xFF);
    writeData(BNO055_GYR_OFFSET_X_LSB, lsbX, 1);
    writeData(BNO055_GYR_OFFSET_X_MSB, msbX, 1);

    char lsbY  = (char)(offsetY & 0xFF); 
    char msbY = (char)((offsetY >> 8) & 0xFF);
    writeData(BNO055_GYR_OFFSET_Y_LSB, lsbY, 1);
    writeData(BNO055_GYR_OFFSET_Y_MSB, msbY, 1);

    char lsbZ  = (char)(offsetZ & 0xFF); 
    char msbZ = (char)((offsetZ >> 8) & 0xFF);
    writeData(BNO055_GYR_OFFSET_Z_LSB, lsbZ, 1);
    writeData(BNO055_GYR_OFFSET_Z_MSB, msbZ, 1);
}

void BNO055::setRadius(uint16_t accRadius, uint16_t magRadius) {
    char lsbAcc  = (char)(accRadius & 0xFF); 
    char msbAcc = (char)((accRadius >> 8) & 0xFF);
    writeData(BNO055_ACC_RADIUS_LSB, lsbAcc, 1);
    writeData(BNO055_ACC_RADIUS_MSB, msbAcc, 1);

    char lsbMag  = (char)(magRadius & 0xFF); 
    char msbMag = (char)((magRadius >> 8) & 0xFF);
    writeData(BNO055_MAG_RADIUS_LSB, lsbMag, 1);
    writeData(BNO055_MAG_RADIUS_MSB, msbMag, 1);
}

uint16_t BNO055::getAccRadius(){
    char lsb, msb;

    readData(BNO055_ACC_RADIUS_LSB, &lsb, 1);
    readData(BNO055_ACC_RADIUS_MSB, &msb, 1);
    
    uint16_t radius = static_cast<uint16_t>((msb << 8) | lsb);
    return radius;
}

uint16_t BNO055::getMagRadius(){
    char lsb, msb;

    readData(BNO055_MAG_RADIUS_LSB, &lsb, 1);
    readData(BNO055_MAG_RADIUS_MSB, &msb, 1);
    
    uint16_t radius = static_cast<uint16_t>((msb << 8) | lsb);
    return radius;  
}

offset BNO055::getMagOffset(){
    char lsbX, msbX;

    readData(BNO055_MAG_OFFSET_X_LSB, &lsbX, 1);
    readData(BNO055_MAG_OFFSET_X_MSB, &msbX, 1);
    
    uint16_t offsetX = static_cast<uint16_t>((msbX << 8) | lsbX);

    char lsbY, msbY;

    readData(BNO055_MAG_OFFSET_Y_LSB, &lsbY, 1);
    readData(BNO055_MAG_OFFSET_Y_MSB, &msbY, 1);
    
    uint16_t offsetY = static_cast<uint16_t>((msbY << 8) | lsbY);

    char lsbZ, msbZ;

    readData(BNO055_MAG_OFFSET_Z_LSB, &lsbZ, 1);
    readData(BNO055_MAG_OFFSET_Z_MSB, &msbZ, 1);
    
    uint16_t offsetZ = static_cast<uint16_t>((msbZ << 8) | lsbZ);
    
    return offset{offsetX, offsetY, offsetZ};
}

offset BNO055::getAccOffset(){
    char lsbX, msbX;

    readData(BNO055_ACC_OFFSET_X_LSB, &lsbX, 1);
    readData(BNO055_ACC_OFFSET_X_MSB, &msbX, 1);
    
    uint16_t offsetX = static_cast<uint16_t>((msbX << 8) | lsbX);

    char lsbY, msbY;

    readData(BNO055_ACC_OFFSET_Y_LSB, &lsbY, 1);
    readData(BNO055_ACC_OFFSET_Y_MSB, &msbY, 1);
    
    uint16_t offsetY = static_cast<uint16_t>((msbY << 8) | lsbY);

    char lsbZ, msbZ;

    readData(BNO055_ACC_OFFSET_Z_LSB, &lsbZ, 1);
    readData(BNO055_ACC_OFFSET_Z_MSB, &msbZ, 1);
    
    uint16_t offsetZ = static_cast<uint16_t>((msbZ << 8) | lsbZ);
    
    return offset{offsetX, offsetY, offsetZ};
}


offset BNO055::getGyrOffset(){
    char lsbX, msbX;

    readData(BNO055_GYR_OFFSET_X_LSB, &lsbX, 1);
    readData(BNO055_GYR_OFFSET_X_MSB, &msbX, 1);
    
    uint16_t offsetX = static_cast<uint16_t>((msbX << 8) | lsbX);

    char lsbY, msbY;

    readData(BNO055_GYR_OFFSET_Y_LSB, &lsbY, 1);
    readData(BNO055_GYR_OFFSET_Y_MSB, &msbY, 1);
    
    uint16_t offsetY = static_cast<uint16_t>((msbY << 8) | lsbY);

    char lsbZ, msbZ;

    readData(BNO055_GYR_OFFSET_Z_LSB, &lsbZ, 1);
    readData(BNO055_GYR_OFFSET_Z_MSB, &msbZ, 1);
    
    uint16_t offsetZ = static_cast<uint16_t>((msbZ << 8) | lsbZ);
    
    return offset{offsetX, offsetY, offsetZ};
}


char BNO055::get_SysErr() {
    setPage(0);
    char err = 0x00;
    readData(BNO055_SYS_ERR, &err, 1);
    return err;
}

char BNO055::get_SysStatus() {
    setPage(0);
    char status = 0x00;
    readData(BNO055_SYS_STATUS, &status, 1);
    return status;
}

void BNO055::runSelfTest() {
    char set;
    readData(BNO055_SYS_TRIGGER, &set, 1);
    set |= 0x01;
    writeData(BNO055_SYS_TRIGGER, set, 1);
    wait(20);
}

BNO055Result BNO055::readSelfTest() {
    setPage(0);
    char res = 0;
    readData(BNO055_ST_RESULT, &res, 1);
    char sys = (res >> 3) & 0x01;
    char gyr = (res >> 2) & 0x01;
    char acc = (res >> 1) & 0x01;
    char mag = res & 0x01;
    if (sys != 1) {
        return BNO055Result::SysErr;
    } else if (gyr != 1) {
        return BNO055Result::GyrErr;
    } else if (acc != 1) {
        return BNO055Result::AccErr;
    } else if (mag != 1) {
        return BNO055Result::MagErr;
    } else {
        return BNO055Result::Ok;
    }
}

BNO055Result BNO055::setup() {
    reset();
    setPage(0);
    setOPMode(BNO055_OPERATION_MODE_NDOF);
    // todo: add additional settings

    return BNO055Result::Ok;
}

BNO055Result BNO055::stop() {
    setPWR(PWRMode::Suspend);
    return BNO055Result::Ok;
}


bno055_vector_t BNO055::bno055_getVector(char vec) {
    setPage(0);
    char buffer[8] = {0};
    if (vec == BNO055_VECTOR_QUATERNION) {
        readData(vec, buffer, 8);
    } else {
        readData(vec, buffer, 6);
    }
    double scale = 1.0;
    if (vec == BNO055_VECTOR_MAGNETOMETER) {
        scale = magScale;
    } else if (vec == BNO055_VECTOR_ACCELEROMETER ||
               vec == BNO055_VECTOR_LINEARACCEL  ||
               vec == BNO055_VECTOR_GRAVITY) {
        scale = accelScale;
    } else if (vec == BNO055_VECTOR_GYROSCOPE) {
        scale = angularRateScale;
    } else if (vec == BNO055_VECTOR_EULER) {
        scale = eulerScale;
    } else if (vec == BNO055_VECTOR_QUATERNION) {
        scale = quaScale;
    }
    bno055_vector_t xyz{};
    xyz.w = 0.0;
    if (vec == BNO055_VECTOR_QUATERNION) {
        xyz.w = static_cast<int16_t>((buffer[1] << 8) | buffer[0]) / scale;
        xyz.x = static_cast<int16_t>((buffer[3] << 8) | buffer[2]) / scale;
        xyz.y = static_cast<int16_t>((buffer[5] << 8) | buffer[4]) / scale;
        xyz.z = static_cast<int16_t>((buffer[7] << 8) | buffer[6]) / scale;
    } else {
        xyz.x = static_cast<int16_t>((buffer[1] << 8) | buffer[0]) / scale;
        xyz.y = static_cast<int16_t>((buffer[3] << 8) | buffer[2]) / scale;
        xyz.z = static_cast<int16_t>((buffer[5] << 8) | buffer[4]) / scale;
    }
    return xyz;
}


float BNO055::getTemperature(){
    char temp;
    readData(BNO055_TEMP, &temp, 1);
    return temp;
}

bno055_vector_t BNO055::getAccelerometer() {
    return bno055_getVector(BNO055_VECTOR_ACCELEROMETER);
}

bno055_vector_t BNO055::getMagnetometer() {
    return bno055_getVector(BNO055_VECTOR_MAGNETOMETER);
}

bno055_vector_t BNO055::getGyroscope() {
    return bno055_getVector(BNO055_VECTOR_GYROSCOPE);
}

bno055_vector_t BNO055::getEuler() {
    return bno055_getVector(BNO055_VECTOR_EULER);
}

bno055_vector_t BNO055::getLinearAccel() {
    return bno055_getVector(BNO055_VECTOR_LINEARACCEL);
}

bno055_vector_t BNO055::getGravity() {
    return bno055_getVector(BNO055_VECTOR_GRAVITY);
}

bno055_vector_t BNO055::getQuaternion() {
    return bno055_getVector(BNO055_VECTOR_QUATERNION);
}
