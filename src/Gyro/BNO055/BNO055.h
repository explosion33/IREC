#ifndef BNO055_H
#define BNO055_H

#include <cstdint> // For std::uint16_t
#include "mbed.h"
#include "USBSerial.h"

enum class BNO055Result {
    Ok,
    SysErr,
    MagErr,
    AccErr,
    GyrErr,
};

enum class PWRMode {
    Normal,
    LowPower,
    Suspend,
};

enum class Axes {
    X,
    Y, 
    Z,
};

enum bno055_vector_type_t {
  BNO055_VECTOR_ACCELEROMETER = 0x08,  // Default: m/s²
  BNO055_VECTOR_MAGNETOMETER = 0x0E,   // Default: uT
  BNO055_VECTOR_GYROSCOPE = 0x14,      // Default: rad/s
  BNO055_VECTOR_EULER = 0x1A,          // Default: degrees
  BNO055_VECTOR_QUATERNION = 0x20,     // No units
  BNO055_VECTOR_LINEARACCEL = 0x28,    // Default: m/s²
  BNO055_VECTOR_GRAVITY = 0x2E         // Default: m/s²
};

struct bno055_vector_t {
    double w;
    double x;
    double y;
    double z;
};

struct offset {
    uint16_t offsetX;
    uint16_t offsetY;
    uint16_t offsetZ;
};

class BNO055 {
public:
    /**
     * Constructor using pins for I2C and a device address.
     * @param SDA I2C data pin
     * @param SCL I2C clock pin
     * @param addr 8-bit device address (convert from 7 to 8 bit)
     */
    BNO055(PinName SDA, PinName SCL, char addr);

    /**
     * Constructor using an existing I2C object.
     * @param i2c Pointer to an existing I2C interface
     */
    BNO055(I2C* i2c, char addr);

    /**
     * Destructor.
     * Deletes I2C only if owned by this object.
     */
    ~BNO055();

    // Simple test function
    void dummy();


    // Calibration checks
    int getSysCalib();
    int getGyrCalib();
    int getAccCalib();
    int getMagCalib();

    // Power/operation modes
    char getOPMode();
    void setOPMode(char mode);

    // Resets (soft/hard)
    void reset();
    void nReset();

    // Status / Self-test
    char get_SysErr();
    char get_SysStatus();
    void runSelfTest();
    BNO055Result readSelfTest();

    // Lifecycle
    BNO055Result setup();
    BNO055Result stop();

    // Data retrieval
    bno055_vector_t bno055_getVector(char vec);
    bno055_vector_t getAccelerometer();
    bno055_vector_t getMagnetometer();
    bno055_vector_t getGyroscope();
    bno055_vector_t getEuler();
    bno055_vector_t getLinearAccel();
    bno055_vector_t getGravity();
    bno055_vector_t getQuaternion();
    float getTemperature();

    // Calibration 
    offset getGyrOffset();
    offset getAccOffset();
    offset getMagOffset();
    uint16_t getMagRadius();
    uint16_t getAccRadius();

private:
    I2C* i2c;
    bool owned;
    char addr;

    // Configuration
    void setACC(char GRange, char Bandwidth, char OPMode);
    void setGYR(char Range, char Bandwidth, char OPMode);
    void setMAG(char Rate, char OPMode, char Power);
    void setPage(uint8_t page);
    void setCLK(bool external);
    void setUnit(bool acc, bool angular, bool euler, bool temp, bool fusion);

    // Sensor offsets
    void setACCOffset(uint16_t offsetX, uint16_t offsetY, uint16_t offsetZ);
    void setMAGOffset(uint16_t offsetX, uint16_t offsetY, uint16_t offsetZ);
    void setGYROffset(uint16_t offsetX, uint16_t offsetY, uint16_t offsetZ);
    void setRadius(uint16_t accRadius, uint16_t magRadius);

    // Axis configuration
    void setAxes(Axes newX, Axes newY, Axes newZ, bool xNeg, bool yNeg, bool zNeg);
    char getAxes(Axes newX, Axes newY, Axes newZ);
    char getAxesSign(bool xNeg, bool yNeg, bool zNeg);

    // Low-level reads/writes
    int readData(char regaddr, char* data, uint8_t len);
    int writeData(char regaddr, char data, uint8_t len);
    void setPWR(PWRMode mode);
};

#endif // BNO055_H