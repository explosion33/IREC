#include "mbed.h"
#include "BNO055.h"
#include "func.h"
#include "EUSBSerial.h"
#include "motor.h"
#include "tmp102.h"
#include "flash.h"
#include "onboard.h"
#include "encoder.h"
#include "USBSerial.h"  

DigitalOut led (PC_13); // Onboard LED
DigitalOut rst(PA_5); // RST pin for the BNO055
//EUSBSerial serial(0x3232, 0x1);
USBSerial serial;
I2C i2c(PB_7, PB_8); 
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
BNO055 bno (PB_7, PB_8, 0x28 << 1);
//tmp102 tmp(PB_7, PB_6, 0x91);
//Servo myservo(PA_15); // motor pwm pin
flash f (PA_7, PA_6, PA_5, PA_4);
encoder e (PA_8, PA_9, 4096);

int main() {
    // bno.setup();
    // f.eraseSector(FLASH_START_ADDR);

    // logAllBNOData(&bno, &f, &serial);

    // readAllBNOData(&f, &serial, 1);
    float count;
    while(1){
        count = e.getRevolutions();
        serial.printf("%f\n",  count);
        ThisThread::sleep_for(50);
    }
}