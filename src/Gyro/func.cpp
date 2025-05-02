#include "mbed.h"
#include "USBSerial.h"

void wait (int time_ms){
    ThisThread::sleep_for(time_ms);
}

int ack; 
I2C i2c(PB_7, PB_8); 
int address;  
USBSerial serial;
void scanI2C() {
  for(address=0;address<255;address++) {    
    ack = i2c.write(address, "11", 1);
    if (ack == 0) {
       serial.printf("\tFound at %3d -- %3x\r\n", address,address);
    }    
    wait(50);
  } 
} 