#include "mbed.h"
#include "tmp102.h"

tmp102::tmp102(PinName SDA, PinName SCL, char addr){
    i2c = new I2C(SDA, SCL);
    tmp102::addr = addr;
    extendedMode = 0;
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
    readData(TMP102_config, config, 2);
    config[0] = config[0] | 0x01;
    writeData(TMP102_CONFIG, config, 2);
}

void tmp102::turnOn(){
    char config[2];
    readData(TMP102_config, config, 2);
    config[0] = config[0] & 0xFE;
    writeData(TMP102_CONFIG, config, 2);
}

void tmp102::setComparator(){
    cchar config[2];
    readData(TMP102_config, config, 2);
    config[0] = config[0] & 0xFD;
    writeData(TMP102_CONFIG, config, 2);
}

void tmp102::setInterrupt(){
    char config[2];
    readData(TMP102_config, config, 2); 
    config[0] = config[0] | 0x02;
    writeData(TMP102_CONFIG, config, 2);
}

void tmp102::setPolarity(int polarity){
    char config[2];
    readData(TMP102_CONFIG, config, 2);
    if (polarity){
        config[0] = config[0] | 0x04;
    } else {
        config[0] = config[0] & 0xFB;
    }
    writeData(TMP102_CONFIG, config, 2);
}

void tmp102::setFaultQueue(){
    
}

void tmp102::getResolution(){

}

void tmp102::oneShot(){

}

void tmp102::setEM(){

}

void tmp102::readAlert(){

}

void tmp102::setConversion(){

}

void tmp102::setHigh(){

}

void tmp102::setLow(){

}


void tmp102::setup(){

}

void tmp102::reset(){
    
}