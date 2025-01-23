#include <cstdint> // For std::uint16_t
#include "mbed.h"
#include "USBSerial.h"

#define START_BYTE 0xAA
#define RESPONSE_BYTE 0xBB
#define ERROR_BYTE 0xEE

#define BNO055_I2C_ADDR_HI 0x29
#define BNO055_I2C_ADDR_LO 0x28
#define BNO055_I2C_ADDR    BNO055_I2C_ADDR_LO

#define BNO055_READ_TIMEOUT 100
#define BNO055_WRITE_TIMEOUT 10

#define ERROR_WRITE_SUCCESS 0x01  // Everything working as expected
#define ERROR_WRITE_FAIL                                                       \
  0x03  // Check connection, protocol settings and operation more of BNO055
#define ERROR_REGMAP_INV_ADDR 0x04   // Invalid register address
#define ERROR_REGMAP_WRITE_DIS 0x05  // Register is read-only
#define ERROR_WRONG_START_BYTE 0x06  // Check if the first byte
#define ERROR_BUS_OVERRUN_ERR                                                  \
  0x07  // Resend the command, BNO055 was not able to clear the receive buffer
#define ERROR_MAX_LEN_ERR                                                      \
  0x08  // Split the command, max fire size can be up to 128 bytes
#define ERROR_MIN_LEN_ERR 0x09  // Min length of data is less than 1
#define ERROR_RECV_CHAR_TIMEOUT                                                \
  0x0A  // Decrease the waiting time between sending of two bytes of one frame

#define REG_WRITE 0x00
#define REG_READ 0x01

// Page 0
#define BNO055_ID (0xA0)
#define BNO055_CHIP_ID 0x00        // value: 0xA0
#define BNO055_ACC_ID 0x01         // value: 0xFB
#define BNO055_MAG_ID 0x02         // value: 0x32
#define BNO055_GYRO_ID 0x03        // value: 0x0F
#define BNO055_SW_REV_ID_LSB 0x04  // value: 0x08
#define BNO055_SW_REV_ID_MSB 0x05  // value: 0x03
#define BNO055_BL_REV_ID 0x06      // N/A
#define BNO055_PAGE_ID 0x07
#define BNO055_ACC_DATA_X_LSB 0x08
#define BNO055_ACC_DATA_X_MSB 0x09
#define BNO055_ACC_DATA_Y_LSB 0x0A
#define BNO055_ACC_DATA_Y_MSB 0x0B
#define BNO055_ACC_DATA_Z_LSB 0x0C
#define BNO055_ACC_DATA_Z_MSB 0x0D
#define BNO055_MAG_DATA_X_LSB 0x0E
#define BNO055_MAG_DATA_X_MSB 0x0F
#define BNO055_MAG_DATA_Y_LSB 0x10
#define BNO055_MAG_DATA_Y_MSB 0x11
#define BNO055_MAG_DATA_Z_LSB 0x12
#define BNO055_MAG_DATA_Z_MSB 0x13
#define BNO055_GYR_DATA_X_LSB 0x14
#define BNO055_GYR_DATA_X_MSB 0x15
#define BNO055_GYR_DATA_Y_LSB 0x16
#define BNO055_GYR_DATA_Y_MSB 0x17
#define BNO055_GYR_DATA_Z_LSB 0x18
#define BNO055_GYR_DATA_Z_MSB 0x19
#define BNO055_EUL_HEADING_LSB 0x1A
#define BNO055_EUL_HEADING_MSB 0x1B
#define BNO055_EUL_ROLL_LSB 0x1C
#define BNO055_EUL_ROLL_MSB 0x1D
#define BNO055_EUL_PITCH_LSB 0x1E
#define BNO055_EUL_PITCH_MSB 0x1F
#define BNO055_QUA_DATA_W_LSB 0x20
#define BNO055_QUA_DATA_W_MSB 0x21
#define BNO055_QUA_DATA_X_LSB 0x22
#define BNO055_QUA_DATA_X_MSB 0x23
#define BNO055_QUA_DATA_Y_LSB 0x24
#define BNO055_QUA_DATA_Y_MSB 0x25
#define BNO055_QUA_DATA_Z_LSB 0x26
#define BNO055_QUA_DATA_Z_MSB 0x27
#define BNO055_LIA_DATA_X_LSB 0x28
#define BNO055_LIA_DATA_X_MSB 0x29
#define BNO055_LIA_DATA_Y_LSB 0x2A
#define BNO055_LIA_DATA_Y_MSB 0x2B
#define BNO055_LIA_DATA_Z_LSB 0x2C
#define BNO055_LIA_DATA_Z_MSB 0x2D
#define BNO055_GRV_DATA_X_LSB 0x2E
#define BNO055_GRV_DATA_X_MSB 0x2F
#define BNO055_GRV_DATA_Y_LSB 0x30
#define BNO055_GRV_DATA_Y_MSB 0x31
#define BNO055_GRV_DATA_Z_LSB 0x32
#define BNO055_GRV_DATA_Z_MSB 0x33
#define BNO055_TEMP 0x34
#define BNO055_CALIB_STAT 0x35
#define BNO055_ST_RESULT 0x36
#define BNO055_INT_STATUS 0x37
#define BNO055_SYS_CLK_STATUS 0x38
#define BNO055_SYS_STATUS 0x39
#define BNO055_SYS_ERR 0x3A
#define BNO055_UNIT_SEL 0x3B
#define BNO055_OPR_MODE 0x3D
#define BNO055_PWR_MODE 0x3E
#define BNO055_SYS_TRIGGER 0x3F
#define BNO055_TEMP_SOURCE 0x40
#define BNO055_AXIS_MAP_CONFIG 0x41
#define BNO055_AXIS_MAP_SIGN 0x42
#define BNO055_ACC_OFFSET_X_LSB 0x55
#define BNO055_ACC_OFFSET_X_MSB 0x56
#define BNO055_ACC_OFFSET_Y_LSB 0x57
#define BNO055_ACC_OFFSET_Y_MSB 0x58
#define BNO055_ACC_OFFSET_Z_LSB 0x59
#define BNO055_ACC_OFFSET_Z_MSB 0x5A
#define BNO055_MAG_OFFSET_X_LSB 0x5B
#define BNO055_MAG_OFFSET_X_MSB 0x5C
#define BNO055_MAG_OFFSET_Y_LSB 0x5D
#define BNO055_MAG_OFFSET_Y_MSB 0x5E
#define BNO055_MAG_OFFSET_Z_LSB 0x5F
#define BNO055_MAG_OFFSET_Z_MSB 0x60
#define BNO055_GYR_OFFSET_X_LSB 0x61
#define BNO055_GYR_OFFSET_X_MSB 0x62
#define BNO055_GYR_OFFSET_Y_LSB 0x63
#define BNO055_GYR_OFFSET_Y_MSB 0x64
#define BNO055_GYR_OFFSET_Z_LSB 0x65
#define BNO055_GYR_OFFSET_Z_MSB 0x66
#define BNO055_ACC_RADIUS_LSB 0x67
#define BNO055_ACC_RADIUS_MSB 0x68
#define BNO055_MAG_RADIUS_LSB 0x69
#define BNO055_MAG_RADIUS_MSB 0x6A
//
// BNO055 Page 1
#define BNO055_PAGE_ID 0x07
#define BNO055_ACC_CONFIG 0x08
#define BNO055_MAG_CONFIG 0x09
#define BNO055_GYRO_CONFIG_0 0x0A
#define BNO055_GYRO_CONFIG_1 0x0B
#define BNO055_ACC_SLEEP_CONFIG 0x0C
#define BNO055_GYR_SLEEP_CONFIG 0x0D
#define BNO055_INT_MSK 0x0F
#define BNO055_INT_EN 0x10
#define BNO055_ACC_AM_THRES 0x11
#define BNO055_ACC_INT_SETTINGS 0x12
#define BNO055_ACC_HG_DURATION 0x13
#define BNO055_ACC_HG_THRESH 0x14
#define BNO055_ACC_NM_THRESH 0x15
#define BNO055_ACC_NM_SET 0x16
#define BNO055_GYR_INT_SETTINGS 0x17
#define BNO055_GYR_HR_X_SET 0x18
#define BNO055_GYR_DUR_X 0x19
#define BNO055_GYR_HR_Y_SET 0x1A
#define BNO055_GYR_DUR_Y 0x1B
#define BNO055_GYR_HR_Z_SET 0x1C
#define BNO055_GYR_DUR_Z 0x1D
#define BNO055_GYR_AM_THRESH 0x1E
#define BNO055_GYR_AM_SET 0x1F

#define BNO055_OPERATION_MODE_CONFIG       0x00
// Sensor Mode
#define BNO055_OPERATION_MODE_ACCONLY     0x01
#define BNO055_OPERATION_MODE_MAGONLY     0x02
#define BNO055_OPERATION_MODE_GYRONLY     0x03
#define BNO055_OPERATION_MODE_ACCMAG     0x04
#define BNO055_OPERATION_MODE_ACCGYRO    0x05
#define BNO055_OPERATION_MODE_MAGGYRO    0x06
#define BNO055_OPERATION_MODE_AMG        0x07
// Fusion Mode
#define BNO055_OPERATION_MODE_IMU        0x08
#define BNO055_OPERATION_MODE_COMPASS    0x09
#define BNO055_OPERATION_MODE_M4G        0x0A
#define BNO055_OPERATION_MODE_NDOF_FMC_OFF  0x0B
#define BNO055_OPERATION_MODE_NDOF       0x0C

static constexpr std::uint16_t accelScale       = 100;
static constexpr std::uint16_t tempScale        = 1;
static constexpr std::uint16_t angularRateScale = 16;
static constexpr std::uint16_t eulerScale       = 16;
static constexpr std::uint16_t magScale         = 16;
static constexpr std::uint16_t quaScale         = (1 << 14); // 2^14

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

class BNO055 {
public:
    BNO055(PinName SDA, PinName SCL, USBSerial* serial, char addr);
    BNO055(I2C* i2c);
    ~BNO055();

    void dummy();
    BNO055Result setup();
    BNO055Result stop();
    bno055_vector_t bno055_getVectorAccelerometer();
    bno055_vector_t bno055_getVectorMagnetometer();
    bno055_vector_t bno055_getVectorGyroscope();
    bno055_vector_t bno055_getVectorEuler();
    bno055_vector_t bno055_getVectorLinearAccel();
    bno055_vector_t bno055_getVectorGravity();
    bno055_vector_t bno055_getVectorQuaternion();
    int readData(char regaddr, char* data, uint8_t len);

private:
    I2C* i2c;
    bool owned;
    USBSerial* bnoserial;
    char addr;
    int writeData(uint8_t addr, char* data, uint8_t len);
    void setPWR(PWRMode mode);
    char getOPMode();
    void setOPMode(char mode);
    void setACC(char GRange, char Bandwidth, char OPMode);
    void setGYR(char Range, char Bandwidth, char OPMode);
    void setMAG(char Rate, char OPMode, char Power);
    void setPage(uint8_t page);
    void setCLK(bool external = false);
    void setUnit(bool acc, bool angular, bool euler, bool temp, bool fusion);
    void reset();
    void nReset();
    void setAxes(Axes newX, Axes newY, Axes newZ, bool xNeg = 0, bool yNeg = 0, bool zNeg = 0);
    char getAxes(Axes newX, Axes newY, Axes newZ);
    char getAxesSign(bool xNeg, bool yNeg, bool zNeg);
    void setACCOffset();
    void setMAGOffset();
    void setGYRPOffset();
    void setRadius();
    void get_SysErr();
    void get_SysStatus();
    BNO055Result checkCalibration();
    void runSelfTest();
    BNO055Result readSelfTest();
    bno055_vector_t bno055_getVector(std::uint8_t vec);
};