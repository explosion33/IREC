#include "mbed.h"
#include "BNO055.h" // todo: swap these to .h files
// main() runs in its own thread in the OS
#include "func.h"
#include "USBSerial.h"
DigitalOut test (PC_13);
int bno_addr = 0x50;
I2C i2c (PB_7, PB_6);
USBSerial serial;
int ack; 
int address;  
void scanI2C() {
  for(address=1;address<127;address++) {    
    ack = i2c.write(address, "11", 1);
    if (ack == 0) {
       serial.printf("\tFound at %3d -- %3x\r\n", address,address);
    }    
    wait(0.05);
  } 
}
int main()
{   
    BNO055 bno (PB_7, PB_6, &serial, 0x50);
    bno.setOPMode(BNO055_OPERATION_MODE_NDOF);
    char unit;
    char data[2];
    while (true) {
        bno.readData(0x0C, data, 1);
        uint16_t val = static_cast<int16_t>((data[1] << 8) | data[0]);
        serial.printf("%d\n", val/100);
        ThisThread::sleep_for(100);
    }
}