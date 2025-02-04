#include "mbed.h"
#include "BNO055.h" // todo: swap these to .h files
// main() runs in its own thread in the OS
#include "func.h"
#include "USBSerial.h"
DigitalOut test (PC_13); // Onboard LED
DigitalOut rst(PA_5); // RST pin for the BNO055
USBSerial serial;
I2C i2c(PB_7, PB_6);
int ack; 
int address;  
void scanI2C() {
  for(address=1;address<127;address++) {    
    ack = i2c.write(address, "11", 1);
    if (ack == 0) {
       serial.printf("\tFound at %3d -- %3x\r\n", address,address);
    }    
    wait(50);
  } 
}

int main()
{   
    BNO055 bno (PB_7, PB_6, 0x28 << 1);
    bno.setup();
    bno055_vector_t vec;
    while (true) {
        // scanI2C();
        vec = bno.getGravity();
        serial.printf("%f, %f, %f \n", vec.x, vec.y, vec.z);
        wait(100);
    }
}