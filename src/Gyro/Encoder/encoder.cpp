#include "encoder.h"

// Lookup table for quadrature transitions: oldState (2 bits) -> newState (2 bits).
// Index = (oldState << 2) | (newState). Each entry is -1, 0, or +1.
static const int8_t _transitionTable[16] = {
 // new = 00, 01, 10, 11
 /* old=00 */  0,  +1,  -1,  0,
 /* old=01 */ -1,   0,   0, +1,
 /* old=10 */ +1,   0,   0, -1,
 /* old=11 */  0,  -1,  +1,  0
};

encoder::encoder(PinName channelA, PinName channelB, int pulsesPerRev)
    : _chanA(channelA),
      _chanB(channelB),
      _position(0),
      _pulsesPerRev(pulsesPerRev),
      _prevState(0),
      _direction(0)
{
    // Read the initial state of A and B
    uint8_t A = _chanA.read();
    uint8_t B = _chanB.read();
    _prevState = (A << 1) | B;

    // Attach the same ISR to all edges on both channels
    _chanA.rise(callback(this, &encoder::encodeISR));
    _chanA.fall(callback(this, &encoder::encodeISR));
    _chanB.rise(callback(this, &encoder::encodeISR));
    _chanB.fall(callback(this, &encoder::encodeISR));
}

int encoder::getCount() const {
    return _position;
}

float encoder::getOrientationDegrees() const {
    // Convert count to degrees
    return (static_cast<float>(_position) / _pulsesPerRev) * 360.0f;
}

float encoder::getOrientationRadians() const {
    // Convert count to radians
    // 2 * PI * (position / pulsesPerRev)
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
    // Read current A,B state
    uint8_t A = _chanA.read();
    uint8_t B = _chanB.read();
    uint8_t newState = (A << 1) | B;

    // Determine how the position changes
    uint8_t index = (_prevState << 2) | newState;
    int8_t step = _transitionTable[index];

    // Update the count
    _position += step;

    // Update direction based on step
    // step > 0 => forward, step < 0 => backward, step = 0 => no valid change
    if (step > 0) {
        _direction = 1;
    } else if (step < 0) {
        _direction = -1;
    } else {
        _direction = 0;
    }

    // Save new state
    _prevState = newState;
}
