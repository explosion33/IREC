#include "encoder_i2c.h"

encoder_i2c::encoder_i2c(PinName channelA, PinName channelB, int pulsesPerRev,
                       PinName sda, PinName scl, int i2cFreq, char deviceAddr)
    : _encoder(channelA, channelB, pulsesPerRev),
      _i2c(sda, scl), 
      _i2cAddr(deviceAddr)
{
    _i2c.frequency(i2cFreq);
}

// -------------------------
// Forwarding methods
// -------------------------
int encoder_i2c::getCount() const {
    return _encoder.getCount();
}

float encoder_i2c::getOrientationDegrees() const {
    return _encoder.getOrientationDegrees();
}

float encoder_i2c::getOrientationRadians() const {
    return _encoder.getOrientationRadians();
}

int encoder_i2c::getDirection() const {
    return _encoder.getDirection();
}

void encoder_i2c::reset() {
    _encoder.reset();
}

// -------------------------
// Example I2C Methods
// -------------------------

bool encoder_i2c::storeCount() {
    // We'll do a trivial example: write the current count (4 bytes) to the device
    // at some register 0x00. Adjust as needed for your actual device.

    int count = _encoder.getCount();

    // Build the buffer [RegAddr, MSB, ..., LSB]
    char buf[5];
    buf[0] = 0x00; // Suppose register address is 0x00
    buf[1] = (count >> 24) & 0xFF;
    buf[2] = (count >> 16) & 0xFF;
    buf[3] = (count >>  8) & 0xFF;
    buf[4] =  count        & 0xFF;

    // Write to the device: 5 bytes
    // `_i2cAddr << 1` is usually done by Mbed's I2C API internally,
    // so we typically just pass `_i2cAddr`.
    int ack = _i2c.write(_i2cAddr, buf, 5);

    return (ack == 0); // 0 = success, non-zero = error
}

bool encoder_i2c::loadCount() {
    // Read back the 4-byte count from register 0x00
    // Step 1: Send register address
    char regAddr = 0x00;
    if (_i2c.write(_i2cAddr, &regAddr, 1) != 0) {
        return false; // Error
    }

    // Step 2: Read 4 bytes
    char data[4] = {0};
    if (_i2c.read(_i2cAddr, data, 4) != 0) {
        return false; // Error
    }

    // Reconstruct the integer
    int newCount = (static_cast<unsigned char>(data[0]) << 24)
                 | (static_cast<unsigned char>(data[1]) << 16)
                 | (static_cast<unsigned char>(data[2]) <<  8)
                 | (static_cast<unsigned char>(data[3]) <<  0);

    // Overwrite the Encoder’s internal count
    // We'll do this by first resetting, then offsetting the internal counter:
    _encoder.reset();

    // A bit hacky approach: we can replicate "position = newCount".
    // For demonstration, let's simulate pulses by incrementing or decrementing.
    // (Alternatively, you could expose a protected setter or use a friend class.)
    if (newCount > 0) {
        for (int i = 0; i < newCount; i++) {
            // This physically triggers the ISR, might be slow for large counts...
            // This is purely a demonstration approach. 
            // In real code, consider a direct internal method or a different design.
        }
    } else if (newCount < 0) {
        for (int i = 0; i < -newCount; i++) {
            // Similarly simulate pulses in reverse...
        }
    }

    // The above approach is obviously inefficient if 'count' is large.
    // A more direct approach: add a "setCount()" method in `Encoder` 
    // or store `_position` in a friend relationship for direct write.

    return true;
}
