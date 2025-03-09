#ifndef AS5601_H
#define AS5601_H

#include "mbed.h"
#include "encoder.h"

/**
 * @brief A class for the AS5601 that implements i2c for a quadrature encoder
 *        - Reuses the existing Encoder for quadrature decoding.
 *        - Adds an I2C object to communicate with external devices (e.g., store or retrieve data).
 */
class AS5601 {
public:
    /**
     * @brief Construct a new AS5601
     * 
     * @param channelA      Pin for the encoder Channel A
     * @param channelB      Pin for the encoder Channel B
     * @param pulsesPerRev  PPR for the underlying Encoder (e.g. 360 or 1440)
     * @param sda           Pin for I2C SDA
     * @param scl           Pin for I2C SCL
     * @param i2cFreq       Frequency for I2C (default 100kHz)
     * @param deviceAddr    Address of the external I2C device we talk to
     */
    AS5601(PinName channelA, PinName channelB, int pulsesPerRev,
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

private:
    encoder _encoder;   ///< The quadrature decoder instance
    I2C     _i2c;       ///< The I2C interface
    char    _i2cAddr;   ///< Address of the external I2C device
};

#endif // AS5601_H
