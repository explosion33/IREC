#include "mbed.h"
#include "BNO055.h" // todo: swap these to .h files
// main() runs in its own thread in the OS
#include "func.h"
#include "USBSerial.h"
DigitalOut test (PC_13);

int main()
{   
    USBSerial serial;
    BNO055 bno (PB_7, PB_6, &serial, 0x29);
    bno.dummy();
    while (true) {
        
        // bno055_vector_t  vec;
        // vec = bno.bno055_getVectorAccelerometer();
        // serial.printf("%f", vec.x);
    }
}