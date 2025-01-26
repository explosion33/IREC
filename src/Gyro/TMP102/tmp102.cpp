#include "mbed.h"
#include "tmp102.h"

tmp102::tmp102(PinName SDA, PinName SCL, char addr){
    i2c = new I2C(SDA, SCL);
    tmp102::addr = addr;
}

int tmp102::readData(char regaddr, char* data, uint8_t len) {
    i2c->write(addr, &regaddr, 1);
    return i2c->read(addr, data, len);
}

int tmp102::writeData(char regaddr, char data, uint8_t len) {
    char buffer[2];
    buffer[0] = regaddr;
    buffer[1] = data;
    return i2c->write(addr, buffer, 2);
}

double tmp102::getTemp(){
    char temp[2];
    readData(TMP102_TEMP_REG, temp, 2);

}

void tmp102::shutDown(){

}

void tmp102::setThermostat(){

}

void tmp102::setPolarity(){

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