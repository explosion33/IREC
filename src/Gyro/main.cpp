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

// TODO: get full flight code written and tested
// Sensors
BNO055 bno (PB_7, PB_8, 0x50);
tmp102 tmp(PB_7, PB_6, 0x91);
Motor mymotor(PA_15); // motor pwm pin
flash f (PA_7, PA_6, PA_5, PA_4);
encoder e1 (PA_8, PA_9, 4096);
encoder e2 (PA_8, PA_9, 4096);

// Threads
Thread thread1;
Thread thread2;
Thread thread3;
Mutex logMutex;

struct EncoderData{
    float encoder1_pos;
    float encoder2_pos;
};

struct BNO055Data{
    bno055_vector_t acc;
    bno055_vector_t gyr;
    bno055_vector_t mag;
    bno055_vector_t eul;
    bno055_vector_t lin;
    bno055_vector_t grav;
    bno055_vector_t quat;
};

struct MotorData{
    float speed;
};

struct TMPData{
    float temp;
};

struct LogData {
    EncoderData encoder;
    BNO055Data bno055;
    TMPData tmp;
};

LogData logdata;
void motor_thread() {
    mymotor.arm();
    while (true) {
        for (float i = 0.00; i < 1.0; i += 0.025) {
            mymotor.setSpeed(i);
            ThisThread::sleep_for(100ms);
        }
    }
}

void sensor_thread() {
    bno.setup();
    while (true) {
        bno055_vector_t acc = bno.getAccelerometer();
        bno055_vector_t gyr = bno.getGyroscope();
        bno055_vector_t mag = bno.getMagnetometer();
        bno055_vector_t eul = bno.getEuler();
        bno055_vector_t lin = bno.getLinearAccel();
        bno055_vector_t grav = bno.getGravity();
        bno055_vector_t quat = bno.getQuaternion();
        
        float temp = tmp.getTempCelsius();

        logMutex.lock();
        logdata.tmp.temp = temp;
        logdata.bno055.acc = acc;
        logdata.bno055.gyr = gyr;
        logdata.bno055.mag = mag;
        logdata.bno055.eul = eul;
        logdata.bno055.lin = lin;
        logdata.bno055.grav = grav;
        logdata.bno055.quat = quat;
        logMutex.unlock();

        ThisThread::sleep_for(10ms);
    }
}

void encoder_thread(){
    while (1) {
        float pos1 = e1.getOrientationDegrees();
        float pos2 = e2.getOrientationDegrees();

        logMutex.lock();
        logdata.encoder.encoder1_pos = pos1;
        logdata.encoder.encoder2_pos = pos2;
        logMutex.unlock();

        ThisThread::sleep_for(10ms);
    }
}

void log() {
    while (1) {
        LogData snapshot;

        logMutex.lock();
        snapshot = logdata;
        logMutex.unlock();
        
        // flash logging

        printf("Enc1: %.2f Enc2: %.2f | "
                "ACC [w: %.2f x: %.2f y: %.2f z: %.2f] "
                "GYR [w: %.2f x: %.2f y: %.2f z: %.2f] "
                "MAG [w: %.2f x: %.2f y: %.2f z: %.2f] "
                "EUL [w: %.2f x: %.2f y: %.2f z: %.2f] "
                "LIN [w: %.2f x: %.2f y: %.2f z: %.2f] "
                "GRAV [w: %.2f x: %.2f y: %.2f z: %.2f] "
                "QUAT [w: %.2f x: %.2f y: %.2f z: %.2f] "
                "TEMP: %.2f\n",
                snapshot.encoder.encoder1_pos, snapshot.encoder.encoder2_pos,
                snapshot.bno055.acc.w, snapshot.bno055.acc.x, snapshot.bno055.acc.y, snapshot.bno055.acc.z,
                snapshot.bno055.gyr.w, snapshot.bno055.gyr.x, snapshot.bno055.gyr.y, snapshot.bno055.gyr.z,
                snapshot.bno055.mag.w, snapshot.bno055.mag.x, snapshot.bno055.mag.y, snapshot.bno055.mag.z,
                snapshot.bno055.eul.w, snapshot.bno055.eul.x, snapshot.bno055.eul.y, snapshot.bno055.eul.z,
                snapshot.bno055.lin.w, snapshot.bno055.lin.x, snapshot.bno055.lin.y, snapshot.bno055.lin.z,
                snapshot.bno055.grav.w, snapshot.bno055.grav.x, snapshot.bno055.grav.y, snapshot.bno055.grav.z,
                snapshot.bno055.quat.w, snapshot.bno055.quat.x, snapshot.bno055.quat.y, snapshot.bno055.quat.z,
                snapshot.tmp.temp);
        ThisThread::sleep_for(100ms);
    }

}
int main() {
    thread1.start(sensor_thread);
    thread2.start(encoder_thread);
    thread3.start(motor_thread);
    log();

}