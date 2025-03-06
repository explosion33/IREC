#include "encoder_i2c.h"
#include "mbed.h"



int encoder_i2c::readData(char regaddr, char* data, uint8_t len) {
    i2c->write(addr, &regaddr, 1);
    return i2c->read(addr, data, len);
}


int encoder_i2c::writeData(char regaddr, char data, uint8_t len) {
    char buffer[2];
    buffer[0] = regaddr;
    buffer[1] = data;
    return i2c->write(addr, buffer, 2);
}