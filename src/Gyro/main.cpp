#include "mbed.h"
#include "BNO055.h" // todo: swap these to .h files
// main() runs in its own thread in the OS
#include "func.h"
#include "USBSerial.h"
DigitalOut test (PC_13); // Onboard LED
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
    BNO055 bno (PB_7, PB_6, 0x28 << 1);
    bno.setup();
    while (true) {
        
    }
}