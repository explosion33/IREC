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
    // USBSerial serial;
    BNO055 bno (PB_7, PB_6, &serial, 0x50);
        char data;
        char addr = 0x3D;
        char buffer[2];
        buffer[0] = 0x3D;
        buffer[1] = 0x0C;
        int i = i2c.write(0x50, buffer, 2);
        serial.printf("%d ", i);
        bno.readData(addr, &data, 1);
        serial.printf("%x\n", data);
        addr = 0x08;
        i2c.write(0x50, &addr, 1);
        char gyr;
    while (true) {
        i2c.read(0x50, &gyr, 1);
        serial.printf("%d\n", gyr);
        ThisThread::sleep_for(200);
    }
}