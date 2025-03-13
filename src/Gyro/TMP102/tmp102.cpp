#include "mbed.h"
#include "func.h"
#include "tmp102.h"

tmp102::tmp102(PinName SDA, PinName SCL, char addr){
    i2c = new I2C(SDA, SCL);
    tmp102::addr = addr;
    extendedMode = 0;
    polarity = 0;
    owned = true;
}

tmp102::tmp102(I2C* i2c, char addr){
    tmp102::i2c = i2c;
    tmp102::addr = addr;
    extendedMode = 0;
    polarity = 0;
    owned = false;
}

tmp102::~tmp102(){
    if (owned){ 
        delete i2c;
    }
}

int tmp102::readData(char regaddr, char* data, uint8_t len) {
    i2c->write(addr, &regaddr, 1);
    return i2c->read(addr, data, len);
}

int tmp102::writeData(char regaddr, char data[2], uint8_t len) {
    char buffer[3];
    buffer[0] = regaddr;
    buffer[1] = data[0];
    buffer[2] = data[1];
    return i2c->write(addr, buffer, 3);
}

int16_t tmp102::getTemp(){ // returns as a 12 or 13-bit signed value
    char temp[2];
    readData(TMP102_TEMP_REG, temp, 2);

    int16_t rawTemp;
    if (extendedMode){ // 13-bit temperature
        rawTemp = (static_cast<int16_t>(temp[0]) << 5) | (static_cast<int16_t>(temp[1]) >> 3);
        if (rawTemp & 0x1000) { // Sign-extend for 13-bit value
            rawTemp |= 0xE000;
        }
    } else { // 12-bit temperature
        rawTemp = (static_cast<int16_t>(temp[0]) << 4) | (static_cast<int16_t>(temp[1]) >> 4);
        if (rawTemp & 0x0800) { // Sign-extend for 12-bit value
            rawTemp |= 0xF000;
        }
    }
    return rawTemp;
}

float tmp102::getTempCelsius() {
    return getTemp() * 0.0625f; // TMP102 resolution is 0.0625°C per bit
}

float tmp102::getTempFahrenheit() {
    return (getTempCelsius() * 1.8f) + 32.0f;
}

void tmp102::shutDown(){
    char config[2];
    readData(TMP102_CONFIG, config, 2);
    config[0] |= 0x01;
    writeData(TMP102_CONFIG, config, 2);
}

void tmp102::turnOn(){
    char config[2];
    readData(TMP102_CONFIG, config, 2);
    config[0] &= 0xFE;
    writeData(TMP102_CONFIG, config, 2);
}

void tmp102::setComparator(){
    char config[2];
    readData(TMP102_CONFIG, config, 2);
    config[0] &= 0xFD;
    writeData(TMP102_CONFIG, config, 2);
}

void tmp102::setInterrupt(){
    char config[2];
    readData(TMP102_CONFIG, config, 2);
    config[0] |= 0x02;
    writeData(TMP102_CONFIG, config, 2);
}

void tmp102::setPolarity(int polarity){
    tmp102::polarity = polarity;
    char config[2];
    readData(TMP102_CONFIG, config, 2);
    if (polarity){
        config[0] |= 0x04;
    } else {
        config[0] &= 0xFB;
    }
    writeData(TMP102_CONFIG, config, 2);
}

void tmp102::setFaultQueue(char faults){
    char config[2];
    readData(TMP102_CONFIG, config, 2);
    config[0] = (config[0] & ~0x18) | ((faults & 0x03) << 3);
    writeData(TMP102_CONFIG, config, 2);
}

uint8_t tmp102::getResolution(){
    char config[2];
    readData(TMP102_CONFIG, config, 2);
    return static_cast<uint8_t>((config[0] >> 5) & 0x03);
}

int16_t tmp102::oneShot(){
    char config[2];
    readData(TMP102_CONFIG, config, 2);
    config[0] |= 0x80;
    writeData(TMP102_CONFIG, config, 2);\
    wait(10);
    return getTemp();
}

void tmp102::setEM(int EM){
    if (EM == extendedMode){
        return;
    } else {
        extendedMode = EM;
        char config[2];
        readData(TMP102_CONFIG, config, 2);
        config[1] ^= (1 << 4);
        writeData(TMP102_CONFIG, config, 2);
    }
}

int tmp102::readAlert(){
    char config[2];
    readData(TMP102_CONFIG, config, 2);
    return (config[1] & 0x20) ? polarity : !polarity;
}

void tmp102::setConversion(char rate){
    char config[2];
    readData(TMP102_CONFIG, config, 2);
    config[1] = (config[1] & ~0xC0) | ((rate & 0x03) << 6);
    writeData(TMP102_CONFIG, config, 2);
}

void tmp102::setHigh(uint16_t high){
    char val[2];
    val[0] = static_cast<char>((high >> 8) & 0xFF);
    val[1] = static_cast<char>(high & 0xFF);
    writeData(TMP102_THIGH, val, 2);
}

void tmp102::setLow(uint16_t low){
    char val[2];
    val[0] = static_cast<char>((low >> 8) & 0xFF);
    val[1] = static_cast<char>(low & 0xFF);
    writeData(TMP102_TLOW, val, 2);
}

void tmp102::setup(){
}

void tmp102::reset(){
    char reset = 0x06;
    i2c->write(0x00, &reset, 2);
}
