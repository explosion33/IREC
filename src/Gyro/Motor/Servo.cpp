#include "Servo.h"
#include "mbed.h"

static float clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

Servo::Servo(PinName pin) : _pwm(pin) {
    calibrate();     // Use default calibration
    _pwm.period_ms(20);
}

void Servo::write(float percent) {
    float offset = _range_us * 2.0f * (percent - 0.5f);
    _pwm.pulsewidth_us(1500 + (int)clamp(offset, -_range_us, _range_us));
    _p = clamp(percent, 0.0f, 1.0f);
}

void Servo::position(float degrees) {
    float offset = _range_us * (degrees / _degrees);
    _pwm.pulsewidth_us(1500 + (int)clamp(offset, -_range_us, _range_us));
}

void Servo::calibrate(float range_us, float degrees) {
    _range_us = range_us;
    _degrees = degrees;
}

float Servo::read() {
    return _p;
}

Servo& Servo::operator= (float percent) {
    write(percent);
    return *this;
}

Servo& Servo::operator= (Servo& rhs) {
    write(rhs.read());
    return *this;
}

Servo::operator float() {
    return read();
}
