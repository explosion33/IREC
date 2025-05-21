#include "mbed.h"
#include "BNO055.h"
#include "func.h"
#include "EUSBSerial.h"
#include "motor.h"
#include "Servo.h"
#include "tmp102.h"
#include "flash.h"
#include "onboard.h"
#include "encoder.h"
#include "USBSerial.h"  
#include "bno055_const.h"
#include <chrono>


// System Parameters
#define WATCHDOG_TIMEOUT_MS 5000
#define MOTOR_SPEED 0.5
#define SENSOR_INTERVAL chrono::milliseconds(50)
#define ENCODER_INTERVAL chrono::milliseconds(10)
#define LOG_INTERVAL chrono::milliseconds(100)
#define ENCODER_PPM 2048
#define MAX_LOG_BYTES 0x10000
#define ENTRY_SIZE 51
#define FLASH_LOG_START_ADDR 0x0000

DigitalOut led (PA_9); // Onboard LED
DigitalOut rst(PA_5); // RST pin for the BNO055
EUSBSerial serial(0x3232, 0x1);
//USBSerial serial;

// Sensors
BNO055 bno (PB_4, PA_8, 0x50);
tmp102 tmp(PB_4, PA_8, 0x91);
Servo myservo(PA_15);
flash f (PA_7, PA_6, PA_5, PA_4);
encoder e1 (PA_8, PA_9, 2048);
encoder e2 (PB_6, PB_7, 2048);

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

struct EncoderDataRaw {
    int16_t encoder1_raw;
    int16_t encoder2_raw;
    uint32_t timestamp;
};

struct BNO055DataRaw {
    bno055_raw_vector_t acc;
    bno055_raw_vector_t gyr;
    bno055_raw_vector_t mag;
    bno055_raw_vector_t eul;
    bno055_raw_vector_t lin;
    bno055_raw_vector_t grav;
    bno055_raw_vector_t quat;
    uint32_t timestamp;
};

struct MotorDataRaw {
    int16_t speed_raw;
};

struct TMPDataRaw {
    int16_t temp_raw;
};

struct LogDataRaw {
    EncoderDataRaw encoder;
    BNO055DataRaw bno055;
    TMPDataRaw tmp;
};

enum class State {
    Idle,
    Setup,
    Reset,
    Main
};

LogData logdata;
LogDataRaw logdataraw;

void motor_thread() {
    //mymotor.setSpeed(MOTOR_SPEED);
}

void sensor_thread() {
    while (true) {
        bno055_vector_t acc = bno.getAccelerometer();
        bno055_vector_t gyr = bno.getGyroscope();
        bno055_vector_t mag = bno.getMagnetometer();
        bno055_vector_t eul = bno.getEuler();
        bno055_vector_t lin = bno.getLinearAccel();
        bno055_vector_t grav = bno.getGravity();
        bno055_vector_t quat = bno.getQuaternion();
        
        float temp = tmp.getTempCelsius();
        uint64_t timestamp_us = Kernel::Clock::now().time_since_epoch().count();

        logMutex.lock();
        logdata.tmp.temp = temp;
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

void sensor_thread_raw() {
    while (true) {
        bno055_raw_vector_t acc  = bno.getRawAccelerometer();
        bno055_raw_vector_t gyr  = bno.getRawGyroscope();
        bno055_raw_vector_t mag  = bno.getRawMagnetometer();
        bno055_raw_vector_t eul  = bno.getRawEuler();
        bno055_raw_vector_t lin  = bno.getRawLinearAccel();
        bno055_raw_vector_t grav = bno.getRawGravity();
        bno055_raw_vector_t quat = bno.getRawQuaternion();

        int16_t temp_raw = tmp.getTemp();

        uint32_t timestamp_us = static_cast<uint32_t>(
            Kernel::Clock::now().time_since_epoch().count()
        );

        logMutex.lock();
        logdataraw.tmp.temp_raw         = temp_raw;
        logdataraw.bno055.acc           = acc;
        logdataraw.bno055.gyr           = gyr;
        logdataraw.bno055.mag           = mag;
        logdataraw.bno055.eul           = eul;
        logdataraw.bno055.lin           = lin;
        logdataraw.bno055.grav          = grav;
        logdataraw.bno055.quat          = quat;
        logdataraw.bno055.timestamp     = timestamp_us;
        logMutex.unlock();

        ThisThread::sleep_for(SENSOR_INTERVAL);
    }
}

void encoder_thread(){
    while (true) {
        float pos1 = e1.getOrientationDegrees();
        float pos2 = e2.getOrientationDegrees();
        uint64_t timestamp_us = Kernel::Clock::now().time_since_epoch().count();

        logMutex.lock();
        logdata.encoder.encoder1_pos = pos1;
        logdata.encoder.encoder2_pos = pos2;
        logdata.encoder.timestamp = timestamp_us;
        logMutex.unlock();

        ThisThread::sleep_for(ENCODER_INTERVAL);
    }
}

void encoder_thread_raw(){
    while (true) {
        int16_t pos1 = static_cast<int16_t>(e1.getCount());
        int16_t pos2 = static_cast<int16_t>(e2.getCount());
        uint32_t timestamp_us = static_cast<uint32_t>(
            Kernel::Clock::now().time_since_epoch().count()
        );

        logMutex.lock();
        logdataraw.encoder.encoder1_raw = pos1;
        logdataraw.encoder.encoder2_raw = pos2;
        logdataraw.encoder.timestamp = timestamp_us;
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
                serial.printf(" Temp: %f\n", snapshot.tmp.temp);
            }

            last_snapshot = snapshot;
        }
        ThisThread::sleep_for(LOG_INTERVAL);
    }

}

void log_thread_raw() {
    LogDataRaw last_snapshot = {};
    uint32_t write_address = 0x000000;

    while(true) {
        logMutex.lock();
        LogDataRaw snapshot = logdataraw;
        logMutex.unlock();

        bool encoder_ready = snapshot.encoder.timestamp != last_snapshot.encoder.timestamp;
        bool sensor_ready  = snapshot.bno055.timestamp != last_snapshot.bno055.timestamp;

        if (encoder_ready || sensor_ready) {
            uint8_t buffer[128];
            uint8_t* ptr = buffer;

            // Header byte: bit flags
            uint8_t flags = 0;
            if (encoder_ready) flags |= 0x01;
            if (sensor_ready)  flags |= 0x02;
            *ptr++ = flags;

            if (encoder_ready) {
                memcpy(ptr, &snapshot.encoder.timestamp, sizeof(uint32_t)); ptr += 4;
                memcpy(ptr, &snapshot.encoder.encoder1_raw, sizeof(int16_t)); ptr += 2;
                memcpy(ptr, &snapshot.encoder.encoder2_raw, sizeof(int16_t)); ptr += 2;
            }

            if (sensor_ready) {
                memcpy(ptr, &snapshot.bno055.timestamp, sizeof(uint32_t)); ptr += 4;

                auto write_vec = [&](const bno055_raw_vector_t& v, bool with_w = false) {
                    if (with_w) { memcpy(ptr, &v.w, 2); ptr += 2; }
                    memcpy(ptr, &v.x, 2); ptr += 2;
                    memcpy(ptr, &v.y, 2); ptr += 2;
                    memcpy(ptr, &v.z, 2); ptr += 2;
                };

                write_vec(snapshot.bno055.acc);
                write_vec(snapshot.bno055.gyr);
                write_vec(snapshot.bno055.mag);
                write_vec(snapshot.bno055.eul);
                write_vec(snapshot.bno055.lin);
                write_vec(snapshot.bno055.grav);
                write_vec(snapshot.bno055.quat, true);

                memcpy(ptr, &snapshot.tmp.temp_raw, sizeof(int16_t)); ptr += 2;
            }

            size_t entry_size = ptr - buffer;
            f.write(write_address, buffer, entry_size);
            write_address += entry_size;

            last_snapshot = snapshot;
        }

        ThisThread::sleep_for(LOG_INTERVAL);
    }
}

void decode(const uint8_t* buffer, size_t length) {
    const uint8_t* ptr = buffer;
    uint8_t flags = *ptr++;

    if (flags & 0x01) {
        uint32_t ts_enc = *reinterpret_cast<const uint32_t*>(ptr); ptr += 4;
        int16_t enc1 = *reinterpret_cast<const int16_t*>(ptr); ptr += 2;
        int16_t enc2 = *reinterpret_cast<const int16_t*>(ptr); ptr += 2;

        float enc1_pos = static_cast<float>(enc1) / ENCODER_PPM;
        float enc2_pos = static_cast<float>(enc2) / ENCODER_PPM;

        serial.printf("[%u us] ENCODERS:\n", ts_enc);
        serial.printf("  ENC1: %.3f (raw %d)\n", enc1_pos, enc1);
        serial.printf("  ENC2: %.3f (raw %d)\n", enc2_pos, enc2);
    }

    if (flags & 0x02) {
        uint32_t ts_imu = *reinterpret_cast<const uint32_t*>(ptr); ptr += 4;

        auto read_vec3 = [&](const char* label, char vec_id) {
            bno055_raw_vector_t raw{};
            raw.x = *reinterpret_cast<const int16_t*>(ptr); ptr += 2;
            raw.y = *reinterpret_cast<const int16_t*>(ptr); ptr += 2;
            raw.z = *reinterpret_cast<const int16_t*>(ptr); ptr += 2;

            bno055_vector_t v = bno.convertRaw(raw, vec_id);
            serial.printf("  %s [x: %.3f, y: %.3f, z: %.3f]\n", label, v.x, v.y, v.z);
        };

        auto read_quat = [&]() {
            bno055_raw_vector_t raw{};
            raw.w = *reinterpret_cast<const int16_t*>(ptr); ptr += 2;
            raw.x = *reinterpret_cast<const int16_t*>(ptr); ptr += 2;
            raw.y = *reinterpret_cast<const int16_t*>(ptr); ptr += 2;
            raw.z = *reinterpret_cast<const int16_t*>(ptr); ptr += 2;

            bno055_vector_t v = bno.convertRaw(raw, BNO055_VECTOR_QUATERNION);
            serial.printf("  QUAT [w: %.4f, x: %.4f, y: %.4f, z: %.4f]\n", v.w, v.x, v.y, v.z);
        };

        serial.printf("[%u us] BNO055:\n", ts_imu);
        read_vec3("ACC ", BNO055_VECTOR_ACCELEROMETER);
        read_vec3("GYR ", BNO055_VECTOR_GYROSCOPE);
        read_vec3("MAG ", BNO055_VECTOR_MAGNETOMETER);
        read_vec3("EUL ", BNO055_VECTOR_EULER);
        read_vec3("LIN ", BNO055_VECTOR_LINEARACCEL);
        read_vec3("GRAV", BNO055_VECTOR_GRAVITY);
        read_quat();

        int16_t temp_raw = *reinterpret_cast<const int16_t*>(ptr); ptr += 2;
        float temp_celsius = temp_raw * 0.0625f;
        serial.printf("  TEMP: %.2f C\n", temp_celsius, temp_raw);
    }
}

void decodeCSV(const uint8_t* buffer, size_t length) {
    const uint8_t* ptr = buffer;
    uint8_t flags = *ptr++;

    uint32_t ts_enc = 0, ts_imu = 0;
    float enc1_pos = -999999.0f, enc2_pos = -999999.0f;
    float acc[3] = {-999999.0f, -999999.0f, -999999.0f};
    float gyr[3] = {-999999.0f, -999999.0f, -999999.0f};
    float mag[3] = {-999999.0f, -999999.0f, -999999.0f};
    float eul[3] = {-999999.0f, -999999.0f, -999999.0f};
    float lin[3] = {-999999.0f, -999999.0f, -999999.0f};
    float grav[3] = {-999999.0f, -999999.0f, -999999.0f};
    float quat[4] = {-999999.0f, -999999.0f, -999999.0f, -999999.0f};
    float temp_celsius = -999999.0f;

    if (flags & 0x01) {
        ts_enc = *reinterpret_cast<const uint32_t*>(ptr); ptr += 4;
        int16_t enc1 = *reinterpret_cast<const int16_t*>(ptr); ptr += 2;
        int16_t enc2 = *reinterpret_cast<const int16_t*>(ptr); ptr += 2;

        enc1_pos = static_cast<float>(enc1) / ENCODER_PPM;
        enc2_pos = static_cast<float>(enc2) / ENCODER_PPM;
    }

    if (flags & 0x02) {
        ts_imu = *reinterpret_cast<const uint32_t*>(ptr); ptr += 4;

        auto read_vec3 = [&](float* out, char vec_id) {
            bno055_raw_vector_t raw{};
            raw.x = *reinterpret_cast<const int16_t*>(ptr); ptr += 2;
            raw.y = *reinterpret_cast<const int16_t*>(ptr); ptr += 2;
            raw.z = *reinterpret_cast<const int16_t*>(ptr); ptr += 2;
            bno055_vector_t v = bno.convertRaw(raw, vec_id);
            out[0] = v.x;
            out[1] = v.y;
            out[2] = v.z;
        };

        auto read_quat = [&](float* out) {
            bno055_raw_vector_t raw{};
            raw.w = *reinterpret_cast<const int16_t*>(ptr); ptr += 2;
            raw.x = *reinterpret_cast<const int16_t*>(ptr); ptr += 2;
            raw.y = *reinterpret_cast<const int16_t*>(ptr); ptr += 2;
            raw.z = *reinterpret_cast<const int16_t*>(ptr); ptr += 2;
            bno055_vector_t v = bno.convertRaw(raw, BNO055_VECTOR_QUATERNION);
            out[0] = v.w;
            out[1] = v.x;
            out[2] = v.y;
            out[3] = v.z;
        };

        read_vec3(acc,  BNO055_VECTOR_ACCELEROMETER);
        read_vec3(gyr,  BNO055_VECTOR_GYROSCOPE);
        read_vec3(mag,  BNO055_VECTOR_MAGNETOMETER);
        read_vec3(eul,  BNO055_VECTOR_EULER);
        read_vec3(lin,  BNO055_VECTOR_LINEARACCEL);
        read_vec3(grav, BNO055_VECTOR_GRAVITY);
        read_quat(quat);

        int16_t temp_raw = *reinterpret_cast<const int16_t*>(ptr); ptr += 2;
        temp_celsius = temp_raw * 0.0625f;
    }

    serial.printf("%u,%.3f,%.3f,%u,", ts_enc, enc1_pos, enc2_pos, ts_imu);
    for (int i = 0; i < 3; i++) serial.printf("%.3f,", acc[i]);
    for (int i = 0; i < 3; i++) serial.printf("%.3f,", gyr[i]);
    for (int i = 0; i < 3; i++) serial.printf("%.3f,", mag[i]);
    for (int i = 0; i < 3; i++) serial.printf("%.3f,", eul[i]);
    for (int i = 0; i < 3; i++) serial.printf("%.3f,", lin[i]);
    for (int i = 0; i < 3; i++) serial.printf("%.3f,", grav[i]);
    for (int i = 0; i < 4; i++) serial.printf("%.4f,", quat[i]);
    serial.printf("%.2f\n", temp_celsius);
}

void suspend() {
    bno.suspend(); // suspend mode
    tmp.shutDown(); // SD mode
}

void setup() {
    bno.setup();
    tmp.turnOn();
    //mymotor.arm();
}

void wait_sequence() {
    State fsm_state = State::Idle;
    char cmd_buffer[32];

    while (true) {
        switch(fsm_state) {
            case State::Idle:
                if (serial.readline(cmd_buffer, sizeof(cmd_buffer))) {
                    if (strcmp(cmd_buffer, "start") == 0) {
                        fsm_state = State::Setup;
                        serial.printf("start received\n");
                    } else if (strcmp(cmd_buffer, "erase") == 0) {
                        fsm_state = State::Reset;
                        serial.printf("erase\n");
                    }
                }

                ThisThread::sleep_for(10ms);
                break;

            case State::Reset:
                f.eraseAll();
                fsm_state = State::Setup;
                break;


            case State::Setup:  
                setup();
                fsm_state = State::Main;
                break;

            case State::Main:
                return;
        }
        
    }
}
I2C i2c(PB_4, PA_8);  

int ack;   
int address;  
void scanI2C() {
  for(address=1;address<255;address++) {    
    ack = i2c.write(address, "11", 1);
    if (ack == 0) {
        serial.printf("\tFound at %3d -- %3x\r\n", address,address);
    }    
    wait(0.05);
  } 
}
 
// // Run to log data
int main() {
    // mymotor.setSpeed(0.0);
    suspend();
    wait_sequence();
    thread1.start(sensor_thread_raw);
    // thread2.start(encoder_thread_raw);
    // thread3.start(motor_thread);
    thread4.start(log_thread_raw);
}


// Run to read data out
// int main() {
//     ThisThread::sleep_for(5000ms);
//     uint32_t addr = FLASH_LOG_START_ADDR;
//     serial.printf("Starting\n");

//     while (true) {
//         serial.printf("%u", addr);
//         uint8_t flags = 0xFF;
//         f.read(addr, &flags, 1);

//         if (flags == 0xFF) {
//             break;
//         }

//         size_t entry_size = 1;
//         if (flags & 0x01) entry_size += 4 + 2 + 2;
//         if (flags & 0x02) entry_size += 4 + 6 * 3 * 2 + 4 * 2 + 2;

//         if (entry_size > 128) break;

//         uint8_t entry[128];
//         f.read(addr, entry, entry_size);

//         decode(entry, entry_size); // Readable Printout
//         decodeCSV(entry, entry_size); // CSV Printout

//         addr += entry_size;
//         ThisThread::sleep_for(100ms);
//     }

//     serial.printf("# END OF LOG\n");
//     ThisThread::sleep_for(1000ms);
// }