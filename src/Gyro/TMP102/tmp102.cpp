#include "mbed.h"
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

uint16_t tmp102::getTemp(){ //returns as a 12 or 13 bit value
    char temp[2];
    readData(TMP102_TEMP_REG, temp, 2);
    if (extendedMode){ // 13 bit temperature
        return static_cast<uint16_t>((temp[0] << 5) | (temp[1] >> 3));
    } else {
        return static_cast<uint16_t>(temp[0] << 4) | static_cast<uint16_t>(temp[1] >> 4);
    }
}

void tmp102::shutDown(){ // sets device into shutdown mode
    char config[2];
    readData(TMP102_CONFIG, config, 2);
    config[0] = config[0] | 0x01;
    writeData(TMP102_CONFIG, config, 2);
}

void tmp102::turnOn(){
    char config[2];
    readData(TMP102_CONFIG, config, 2);
    config[0] = config[0] & 0xFE;
    writeData(TMP102_CONFIG, config, 2);
}

void tmp102::setComparator(){
    char config[2];
    readData(TMP102_CONFIG, config, 2);
    config[0] = config[0] & 0xFD;
    writeData(TMP102_CONFIG, config, 2);
}

void tmp102::setInterrupt(){
    char config[2];
    readData(TMP102_CONFIG, config, 2);
    config[0] = config[0] | 0x02;
    writeData(TMP102_CONFIG, config, 2);
}

void tmp102::setPolarity(int polarity){
    tmp102::polarity = polarity;
    char config[2];
    readData(TMP102_CONFIG, config, 2);
    if (polarity){
        config[0] = config[0] | 0x04;
    } else {
        config[0] = config[0] & 0xFB;
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
    return static_cast<uint8_t>(config[0] >> 5 & 0x03);
}

uint16_t tmp102::oneShot(){// todo: test when one shot read happens
    char config[2];
    readData(TMP102_CONFIG, config, 2);
    config[0] = 0x80 & config[0];
    writeData(TMP102_CONFIG, config, 2);
    return getTemp();
}

void tmp102::setEM(int EM){
    if (EM == extendedMode){ // if bits same
        return;
    } else { // need to toggle
        extendedMode = EM; // set new EM value
        char config[2];
        readData(TMP102_CONFIG, config, 2);
        config[1] = config[1] ^ (1 << 4); // invert the EM bit
        writeData(TMP102_CONFIG, config, 2);
    }
}   

int tmp102::readAlert(){ // by default if polarity is set to 0, alert is an active high and a low interrupt
    char config[2];
    readData(TMP102_CONFIG, config, 2);
    if (config[1] & 0x20){
        return polarity;
    } else{
        return !polarity;
    }
}

void tmp102::setConversion(char rate){
    char config[2];
    readData(TMP102_CONFIG, config, 2);
    config[1] = (config[1] & ~0xC0) | ((rate & 0x03) << 6);
    writeData(TMP102_CONFIG, config, 2);
}

void tmp102::setHigh(uint16_t high){
    char val[2];
    val[0] = static_cast<char>(high & 0xFF);
    val[1] = static_cast<char>((high >> 8) & 0xFF);
    writeData(TMP102_THIGH, val, 2);
}

void tmp102::setLow(uint16_t low){
    char val[2];
    val[0] = static_cast<char>(low & 0xFF);
    val[1] = static_cast<char>((low >> 8) & 0xFF);
    writeData(TMP102_TLOW, val, 2);
}


void tmp102::setup(){
    
}

void tmp102::reset(){
    char reset = 0x06;
    i2c->write(0x00, &reset, 2);
}