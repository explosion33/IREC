#include "onboard.h"
uint32_t writeAddr = FLASH_START_ADDR;
void logAllBNOData(BNO055 *bno, flash *flash, EUSBSerial *serial) {
    bno055_vector_t acc = bno->getAccelerometer();
    bno055_vector_t gyr = bno->getGyroscope();
    bno055_vector_t mag = bno->getMagnetometer();
    bno055_vector_t eul = bno->getEuler();
    bno055_vector_t lin = bno->getLinearAccel();
    bno055_vector_t grav = bno->getGravity();
    bno055_vector_t quat = bno->getQuaternion();
    float temp = bno->getTemperature();

    double values[23] = { // we can write the raw bit values to save space
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
        flash->writeNum(writeAddr, values[i]); // this is writing as a float = 4 bytes
        writeAddr += 4;
    }

    serial->printf("Logged sample (92 bytes)\n");
    serial->printf("%f\n", acc.x);
}

void readAllBNOData(flash *flash, EUSBSerial *serial, uint32_t entryCount) {
    uint32_t addr = FLASH_START_ADDR;

    for (uint32_t i = 0; i < entryCount; i++) {
        double vals[23];
        for (int j = 0; j < 23; j++) {
            vals[j] = flash->readNum(addr);
            addr += 4;
        }

        serial->printf("Sample %d:\n", i);
        serial->printf("  Accel:      %.2f %.2f %.2f\n", vals[0], vals[1], vals[2]);
        serial->printf("  Gyro:       %.2f %.2f %.2f\n", vals[3], vals[4], vals[5]);
        serial->printf("  Magnet:     %.2f %.2f %.2f\n", vals[6], vals[7], vals[8]);
        serial->printf("  Euler:      %.2f %.2f %.2f\n", vals[9], vals[10], vals[11]);
        serial->printf("  LinAccel:   %.2f %.2f %.2f\n", vals[12], vals[13], vals[14]);
        serial->printf("  Gravity:    %.2f %.2f %.2f\n", vals[15], vals[16], vals[17]);
        serial->printf("  Quaternion: %.4f %.4f %.4f %.4f\n", vals[18], vals[19], vals[20], vals[21]);
        serial->printf("  Temp:       %.2f C\n", vals[22]);
    }
}
