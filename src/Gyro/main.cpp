#include "mbed.h"
#include "BNO055.h" // todo: swap these to .h files
// main() runs in its own thread in the OS
#include "func.h"
#include "USBSerial.h"
#include "Servo.h"
// DigitalOut test (PC_13); // Onboard LED
// DigitalOut rst(PA_5); // RST pin for the BNO055
// USBSerial serial;
// I2C i2c(PB_7, PB_6);
// int ack; 
// int address;  
// void scanI2C() {
//   for(address=1;address<127;address++) {    
//     ack = i2c.write(address, "11", 1);
//     if (ack == 0) {
//        serial.printf("\tFound at %3d -- %3x\r\n", address,address);
//     }    
//     wait(50);
//   } 
// }
Servo myservo(PA_9);
int main()
{   
    // BNO055 bno (PB_7, PB_6, 0x28 << 1);
    // bno.setup();
    // bno055_vector_t vec    
    // while (true) {
    //     // serial.printf("Scanning\n");
    //     // scanI2C();
    //     // wait(100);
    //     // vec = bno.getGravity();
    //     // serial.printf("%f, %f, %f \n", vec.x, vec.y, vec.z);
    //     // wait(100);
    // }

    myservo = 0.0;
    wait(500); //ESC detects signal
//Required ESC Calibration/Arming sequence  
//sends longest and shortest PWM pulse to learn and arm at power on
    myservo = 1.0; //send longest PWM
    wait(8000);
    myservo = 0.0; //send shortest PWM
    wait(8000);
//ESC now operational using standard servo PWM signals
    while (1) {
        for (float p=0.0; p<=1.0; p += 0.025) { //Throttle up slowly to full throttle
            myservo = p;
            wait(1000);
        }
        myservo = 0.0; //Motor off
        wait(4000);
        for (float p=1.0; p>=0.0; p -= 0.025) { //Throttle down slowly from full throttle
            myservo = p;
            wait(1000);
        }
        myservo = 0.0; //Motor off
        wait(4000);
    }
}z