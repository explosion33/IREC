#include "mbed.h"
#include "BNO055.h"
#include "func.h"
#include "EUSBSerial.h"
#include "Servo.h"
#include "tmp102.h"
#include "AS5601.h"
#include "flash.h"

//DigitalOut led (PC_13); // Onboard LED
//DigitalOut rst(PA_5); // RST pin for the BNO055
EUSBSerial serial(0x3232, 0x1);

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
flash flash (PA_7, PA_6, PA_5, PA_4);
Ticker logTicker;
#define FLASH_START_ADDR 0x000000
uint32_t writeAddr = FLASH_START_ADDR;

void logAllBNOData() {
    bno055_vector_t acc = bno.getAccelerometer();
    bno055_vector_t gyr = bno.getGyroscope();
    bno055_vector_t mag = bno.getMagnetometer();
    bno055_vector_t eul = bno.getEuler();
    bno055_vector_t lin = bno.getLinearAccel();
    bno055_vector_t grav = bno.getGravity();
    bno055_vector_t quat = bno.getQuaternion();
    float temp = bno.getTemperature();

    double values[23] = {
        acc.x, acc.y, acc.z,
        gyr.x, gyr.y, gyr.z,
        mag.x, mag.y, mag.z,
        eul.x, eul.y, eul.z,
        lin.x, lin.y, lin.z,    
        grav.x, grav.y, grav.z,
        quat.w, quat.x, quat.y, quat.z,
        temp
    };

    for (int i = 0; i < 23; i++) {
        flash.writeNum(writeAddr, values[i]);
        writeAddr += 4;
    }

    serial.printf("Logged sample (92 bytes)\n");
    serial.printf("%f\n", acc.x);
}

// void readAllBNOData(uint32_t entryCount) {
//     uint32_t addr = FLASH_START_ADDR;

//     for (uint32_t i = 0; i < entryCount; i++) {
//         double vals[23];
//         for (int j = 0; j < 23; j++) {
//             vals[j] = flash.readNum(addr);
//             addr += 4;
//         }

//         printf("Sample %d:\n", i);
//         printf("  Accel:      %.2f %.2f %.2f\n", vals[0], vals[1], vals[2]);
//         printf("  Gyro:       %.2f %.2f %.2f\n", vals[3], vals[4], vals[5]);
//         printf("  Magnet:     %.2f %.2f %.2f\n", vals[6], vals[7], vals[8]);
//         printf("  Euler:      %.2f %.2f %.2f\n", vals[9], vals[10], vals[11]);
//         printf("  LinAccel:   %.2f %.2f %.2f\n", vals[12], vals[13], vals[14]);
//         printf("  Gravity:    %.2f %.2f %.2f\n", vals[15], vals[16], vals[17]);
//         printf("  Quaternion: %.4f %.4f %.4f %.4f\n", vals[18], vals[19], vals[20], vals[21]);
//         printf("  Temp:       %.2f C\n", vals[22]);
//     }
// }

// Sample main to log data
int main()
{   
    flash.reset();
    flash.eraseSector(FLASH_START_ADDR); // Start clean

    bno.setup();
    thread_sleep_for(1000); // Let sensor boot and settle

    logTicker.attach(&logAllBNOData, 0.5); // Log every 0.5 seconds

    // Run indefinitely
    while (1) {
        thread_sleep_for(1000);
    }
}

// Sample main to read back data
// int main()
// {
//     flash.reset();

//     printf("READY. Send 'r' to read data from flash.\n");

//     while (true) {
//         char c;
//         if (pc.read(&c, 1) > 0 && c == 'r') {
//             printf("Reading back 100 samples...\n");
//             readAllBNOData(100); // Adjust this number as needed
//         }
//         thread_sleep_for(100);
//     }
// }