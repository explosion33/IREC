#include "encoder.h"

// Lookup table for transitions
// Index = (oldState << 2) | (newState)
static const int8_t _transitionTable[16] = {
 //   old\new  00, 01, 10, 11
 /*     00 */  0,  +1,  -1,  0,
 /*     01 */ -1,   0,   0, +1,
 /*     10 */ +1,   0,   0, -1,
 /*     11 */  0,  -1,  +1,  0
};

encoder::encoder(PinName channelA, PinName channelB, int pulsesPerRev)
    : _chanA(channelA),
      _chanB(channelB),
      _position(0),
      _pulsesPerRev(pulsesPerRev),
      _prevState(0),
      _direction(0)
{
    uint8_t A = _chanA.read();
    uint8_t B = _chanB.read();
    _prevState = (A << 1) | B;

    _chanA.rise(callback(this, &encoder::encodeISR));
    _chanA.fall(callback(this, &encoder::encodeISR));
    _chanB.rise(callback(this, &encoder::encodeISR));
    _chanB.fall(callback(this, &encoder::encodeISR));
}

int encoder::getCount() const {
    return _position;
}

float encoder::getOrientationDegrees() const {
    return (static_cast<float>(_position) / _pulsesPerRev) * 360.0f;
}

float encoder::getOrientationRadians() const { 
    static const float PI = 3.14159265358979f;
    return (static_cast<float>(_position) / _pulsesPerRev) * (2.0f * PI);
}

int encoder::getDirection() const {
    return _direction;
}

void encoder::reset() {
    _position = 0;
    _direction = 0;
}

void encoder::encodeISR() {
    uint8_t A = _chanA.read();
    uint8_t B = _chanB.read();
    uint8_t newState = (A << 1) | B;

    uint8_t index = (_prevState << 2) | newState;
    int8_t step = _transitionTable[index];

    _position += step;

    if (step > 0) {
        _direction = 1;
    } else if (step < 0) {
        _direction = -1;
    } else {
        _direction = 0;
    }

    _prevState = newState;
}
