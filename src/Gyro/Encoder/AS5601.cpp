#include "AS5601.h"

AS5601::AS5601(PinName channelA, PinName channelB, int pulsesPerRev,
                       PinName sda, PinName scl, int i2cFreq, char deviceAddr)
    : _encoder(channelA, channelB, pulsesPerRev),
      _addr(deviceAddr)
{
    _i2c = new I2C(sda, scl);
}

AS5601::~AS5601(){
    delete _i2c;
}

int AS5601::getCount() const {
    return _encoder.getCount();
}

float AS5601::getOrientationDegrees() const {
    return _encoder.getOrientationDegrees();
}

float AS5601::getOrientationRadians() const {
    return _encoder.getOrientationRadians();
}

int AS5601::getDirection() const {
    return _encoder.getDirection();
}

void AS5601::reset() {
    _encoder.reset();
}

// todo: implement relevant i2c methods