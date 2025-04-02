#include "mbed.h"
#include "BNO055.h"
#include "func.h"
#include "EUSBSerial.h"
#include "Servo.h"
#include "tmp102.h"
#include "AS5601.h"
#include "flash_test.h"
//#include "tmp_test.h"
//#include "bno_test.h"
DigitalOut led (PC_13); // Onboard LED
//DigitalOut rst(PA_5); // RST pin for the BNO055
EUSBSerial serial;

I2C i2c(PB_7, PB_6); 
int ack; 
int address;  
void scanI2C() {
  for(address=0;address<255;address++) {    
    ack = i2c.write(address, "11", 1);
    if (ack == 0) {
       serial.printf("\tFound at %3d -- %3x\r\n", address,address);
    }    
    wait(50);
  } 
} 
// void startESC() {
//     myservo = 0.0;
//     wait(500);
//     myservo = 1.0;
//     wait(8000);
//     myservo = 0.0;
//     wait(8000);
// }

// void motorTest() {
//     startESC();
//     while (1) {
//         for (float p=0.0; p<=1.0; p += 0.025) { //Throttle up slowly to full throttle
//             myservo = p;
//             wait(1000);
//         }
//         myservo = 0.0; //Motor off
//         wait(4000);
//         for (float p=1.0; p>=0.0; p -= 0.025) { //Throttle down slowly from full throttle
//             myservo = p;
//             wait(1000);
//         }
//         myservo = 0.0; //Motor off
//         wait(4000);
//     }
// }
//BNO055 bno (PB_7, PB_6, 0x28 << 1);
//tmp102 tmp(PB_7, PB_6, 0x91);
Servo myservo(PA_15); // motor pwm pin
//flash flash (PA_7, PA_6, PA_5, PA_4);
void motor(){
    // myservo = 0.0;
    // wait(500);
    // myservo = 1.0;
    // wait(8000);
    // myservo = 0.0;
    // wait(8000);
    // char buf[100] = {0};
    // while(1) {
    //     if (serial.readline(buf, 100)){
    //         int speed = 0;
    //         sscanf(buf, "%d", &speed);
    //         myservo = speed/100.0;
    //     }
    // }
}
int main()
{   
    myservo = 0.0;
    wait(500);
    myservo = 1.0;
    wait(8000);
    myservo = 0.0;
    wait(8000);
    char buf[100] = {0};
    while(1) {
        if (serial.readline(buf, 100)){
            int speed = 0;
            sscanf(buf, "%d", &speed);
            myservo = speed/100.0;
        }
    }
}