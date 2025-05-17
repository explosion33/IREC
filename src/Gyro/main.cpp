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
#include <chrono>


// System Parameters
#define WATCHDOG_TIMEOUT_MS 5000
#define MOTOR_SPEED 0.5
#define SENSOR_INTERVAL chrono::milliseconds(10)
#define ENCODER_INTERVAL chrono::milliseconds(10)
#define LOG_INTERVAL chrono::milliseconds(50)

DigitalOut led (PA_10); // Onboard LED
DigitalOut rst(PA_5); // RST pin for the BNO055
EUSBSerial serial(0x3232, 0x1);
//USBSerial serial;

// Sensors
BNO055 bno (PB_7, PB_8, 0x50);
tmp102 tmp(PB_7, PB_6, 0x91);
Motor mymotor(PA_15); // motor pwm pin
// flash f (PA_7, PA_6, PA_5, PA_4);
encoder e1 (PA_8, PA_9, 4096);
//encoder e2 (PA_8, PA_9, 1024);

// TODO: figure out operating frequencies to get system fully functional with no errors
/* 
Maximums: 
Acc - 100Hz
Mag - 20Hz
Gyro - 100Hz
Fusion Data - 100Hz
TMP - 8Hz
*/

// TODO: Need to overwrite defaults to run sensors faster

// watchdog stuff
Watchdog &watchdog = Watchdog::get_instance();
void watchdog_thread() {
    watchdog.start(WATCHDOG_TIMEOUT_MS);
    while (true) {
        watchdog.kick();
        ThisThread::sleep_for(1000ms);
    }
}



Thread thread1;
Thread thread2;
Thread thread3;
Thread thread4;
Mutex logMutex;

struct EncoderData{
    float encoder1_pos;
    float encoder2_pos;
    uint64_t timestamp;
};

struct BNO055Data{
    bno055_vector_t acc;
    bno055_vector_t gyr;
    bno055_vector_t mag;
    bno055_vector_t eul;
    bno055_vector_t lin;
    bno055_vector_t grav;
    bno055_vector_t quat;
    uint64_t timestamp;
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

enum class State {
    Idle,
    Setup,
    Main
};

LogData logdata;

void motor_thread() {
    mymotor.setSpeed(MOTOR_SPEED);
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
        
        //float temp = tmp.getTempCelsius();
        uint64_t timestamp_us = Kernel::Clock::now().time_since_epoch().count();

        logMutex.lock();
        //logdata.tmp.temp = temp;
        logdata.bno055.acc = acc;
        logdata.bno055.gyr = gyr;
        logdata.bno055.mag = mag;
        logdata.bno055.eul = eul;
        logdata.bno055.lin = lin;
        logdata.bno055.grav = grav;
        logdata.bno055.quat = quat;
        logdata.bno055.timestamp = timestamp_us;
        logMutex.unlock();

        ThisThread::sleep_for(SENSOR_INTERVAL);
    }
}

void encoder_thread(){
    while (true) {
        float pos1 = e1.getOrientationDegrees();
        //float pos2 = e2.getOrientationDegrees();
        uint64_t timestamp_us = Kernel::Clock::now().time_since_epoch().count();

        logMutex.lock();
        logdata.encoder.encoder1_pos = pos1;
        //logdata.encoder.encoder2_pos = pos2;
        logdata.encoder.timestamp = timestamp_us;
        logMutex.unlock();

        ThisThread::sleep_for(ENCODER_INTERVAL);
    }
}

void log_thread() {
    LogData last_snapshot = {};
    while (true) {
        LogData snapshot;

        logMutex.lock();
        snapshot = logdata;
        logMutex.unlock();
        

        bool encoder_ready = snapshot.encoder.timestamp != last_snapshot.encoder.timestamp;
        bool sensor_ready = snapshot.bno055.timestamp != last_snapshot.bno055.timestamp;
        // flash logging

        // Print statements for debugging
        if (encoder_ready || sensor_ready) {
            if (encoder_ready) {
                serial.printf(
                    "[%llu us] ENC1: %.2f ENC2: %.2f\n",
                    snapshot.encoder.timestamp,
                    snapshot.encoder.encoder1_pos,
                    snapshot.encoder.encoder2_pos
                );
            }

            if (sensor_ready) {
                serial.printf("[%llu us] IMU:\n", snapshot.bno055.timestamp);
                serial.printf("  ACC  [x: %.2f, y: %.2f, z: %.2f]\n",
                    snapshot.bno055.acc.x, snapshot.bno055.acc.y, snapshot.bno055.acc.z);
                serial.printf("  GYR  [x: %.2f, y: %.2f, z: %.2f]\n",
                    snapshot.bno055.gyr.x, snapshot.bno055.gyr.y, snapshot.bno055.gyr.z);
                serial.printf("  MAG  [x: %.2f, y: %.2f, z: %.2f]\n",
                    snapshot.bno055.mag.x, snapshot.bno055.mag.y, snapshot.bno055.mag.z);
                serial.printf("  EUL  [x: %.2f, y: %.2f, z: %.2f]\n",
                    snapshot.bno055.eul.x, snapshot.bno055.eul.y, snapshot.bno055.eul.z);
                serial.printf("  LIN  [x: %.2f, y: %.2f, z: %.2f]\n",
                    snapshot.bno055.lin.x, snapshot.bno055.lin.y, snapshot.bno055.lin.z);
                serial.printf("  GRAV [x: %.2f, y: %.2f, z: %.2f]\n",
                    snapshot.bno055.grav.x, snapshot.bno055.grav.y, snapshot.bno055.grav.z);
                serial.printf("  QUAT [w: %.2f, x: %.2f, y: %.2f, z: %.2f]\n",
                    snapshot.bno055.quat.w, snapshot.bno055.quat.x,
                    snapshot.bno055.quat.y, snapshot.bno055.quat.z);
            }

            last_snapshot = snapshot;
        }

        ThisThread::sleep_for(LOG_INTERVAL);
    }

}

void print_vector(const bno055_vector_t& v) {
    serial.printf("%.2f %.2f %.2f ", v.x, v.y, v.z);
}


// IR Sensor Stuff
DigitalIn ir (PB_1);
volatile int curr_count = 0;
Ticker t;
int last_state = 0;

void check_sensor() {
    int current_state = ir.read();
    if (last_state == 0 && current_state == 1) {
        // Rising edge detected (object passing)
        curr_count++;
    }
    last_state = current_state;
}

void start() {
    // set sensors into low power states
    bno.start(); // suspend mode
    //tmp.start(); // SD mode

}

void setup() {
    // power on sensors
    // set settings
    // arm esc
    mymotor.arm();
}

// void wait_sequence() {
//     State fsm_state = State::Idle;
//     char cmd_buffer[32];

//     while (true) {
//         switch(fsm_state) {
//             case State::Idle:
//                 if (serial.readline(cmd_buffer, sizeof(cmd_buffer))) {
//                     if (strcmp(cmd_buffer, "Start command") == 0) {
//                         fsm_state = State::Setup;
//                     }
//                 }

//                 ThisThread::sleep_for(10ms);
//                 break;
            
//             case State::Setup:  
//                 setup();
//                 fsm_state = State::Main;
//                 break;

//             case State::Main:
//                 return;
//         }
        
//     }
// }

I2C i2c (PB_4, PA_8);
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

int main() {
    // start();
    // wait_sequence();
    // mymotor.arm();
    // mymotor.setSpeed(0.5);
    //thread1.start(sensor_thread);
    // thread2.start(encoder_thread);
    // thread3.start(motor_thread);
    //thread4.start(log_thread);

    while (true) {
        serial.printf("Scanning");
        scanI2C();
    }
}