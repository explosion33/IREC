#include "encoder.h"
#include "platform/CriticalSectionLock.h"

// Lookup table for transitions between encoder states
// Index = (oldState << 2) | newState
// Each value indicates movement: +1, -1, or 0
static const int8_t _transitionTable[16] = {
    0,  +1,  -1,  0,
   -1,   0,   0, +1,
   +1,   0,   0, -1,
    0,  -1,  +1,  0
};

/**
 * Constructor: initializes encoder with interrupt handlers and starting state.
 */
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

    // Attach ISR to both rising and falling edges on each channel
    _chanA.rise(callback(this, &encoder::encodeISR));
    _chanA.fall(callback(this, &encoder::encodeISR));
    _chanB.rise(callback(this, &encoder::encodeISR));
    _chanB.fall(callback(this, &encoder::encodeISR));
}

/**
 * Returns the current encoder count.
 */
int encoder::getCount() const {
    CriticalSectionLock lock;
    return _position;
}

/**
 * Returns orientation in degrees (0–360).
 */
float encoder::getOrientationDegrees() const {
    CriticalSectionLock lock;
    return (static_cast<float>(_position) / _pulsesPerRev) * 360.0f;
}

/**
 * Returns orientation in radians (0–2π).
 */
float encoder::getOrientationRadians() const {
    static const float PI = 3.14159265358979f;
    CriticalSectionLock lock;
    return (static_cast<float>(_position) / _pulsesPerRev) * (2.0f * PI);
}

/**
 * Returns number of revolutions (can be fractional).
 */
float encoder::getRevolutions() const {
    CriticalSectionLock lock;
    return static_cast<float>(_position) / _pulsesPerRev;
}

/**
 * Returns last movement direction.
 */
int encoder::getDirection() const {
    CriticalSectionLock lock;
    return _direction;
}

/**
 * Resets encoder count and direction.
 */
void encoder::reset() {
    CriticalSectionLock lock;
    _position = 0;
    _direction = 0;
}

/**
 * ISR that handles changes on encoder input pins.
 * Determines new state, calculates movement direction,
 * updates pulse count and stores new state.
 */
void encoder::encodeISR() {
    uint8_t A = _chanA.read();
    uint8_t B = _chanB.read();
    uint8_t newState = (A << 1) | B;

    uint8_t index = (_prevState << 2) | newState;
    int8_t step = _transitionTable[index];

    _position += step;
    _direction = (step > 0) ? 1 : (step < 0 ? -1 : 0);
    _prevState = newState;
}
