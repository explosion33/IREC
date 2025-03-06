#ifndef ENCODER_I2C_H
#define ENCODER_I2C_H

#include "mbed.h"
#include "encoder.h"

/**
 * @brief A composite class that combines the Encoder driver with an I2C interface.
 *        - Reuses the existing Encoder for quadrature decoding.
 *        - Adds an I2C object to communicate with external devices (e.g., store or retrieve data).
 */
class encoder_i2c {
public:
    /**
     * @brief Construct a new encoder_i2c object
     * 
     * @param channelA      Pin for the encoder Channel A
     * @param channelB      Pin for the encoder Channel B
     * @param pulsesPerRev  PPR for the underlying Encoder (e.g. 360 or 1440)
     * @param sda           Pin for I2C SDA
     * @param scl           Pin for I2C SCL
     * @param i2cFreq       Frequency for I2C (default 100kHz)
     * @param deviceAddr    Address of the external I2C device we talk to
     */
    encoder_i2c(PinName channelA, PinName channelB, int pulsesPerRev,
               PinName sda, PinName scl, int i2cFreq = 100000,
               char deviceAddr = 0x50);

    // -------------------------
    // Forwarded methods to Encoder
    // -------------------------
    int getCount() const;
    float getOrientationDegrees() const;
    float getOrientationRadians() const;
    int getDirection() const;
    void reset();

    // -------------------------
    // Additional I2C methods
    // -------------------------

    /**
     * @brief Example: Store the current encoder count in an external I2C device.
     *        Could be an EEPROM, FRAM, or another MCU register.
     * @return true if success, false if an I2C error occurred
     */
    bool storeCount();

    /**
     * @brief Example: Load the encoder count from an external I2C device
     *        and overwrite the current count in the Encoder.
     * @return true if success, false if an I2C error occurred
     */
    bool loadCount();

private:
    encoder _encoder;   ///< The quadrature decoder instance
    I2C     _i2c;       ///< The I2C interface
    char    _i2cAddr;   ///< Address of the external I2C device
};

#endif // ENCODER_I2C_H
