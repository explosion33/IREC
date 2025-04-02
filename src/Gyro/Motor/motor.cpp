#include "motor.h"
#include "func.h"

/**
 * @brief Construct a new Motor object.
 * Initializes the internal Servo and sets it to neutral (stop).
 */
Motor::Motor(PinName pin, float center, float range)
    : _servo(pin), _center(center), _range(range), _speed(0.0f) {
    _servo.write(_center); // Set to neutral on init
}

void Motor::arm() {
    _servo.write(0.0f);                     // Minimum throttle
    wait(500);

    _servo.write(1.0f);                     // Maximum throttle
    wait(8000);

    _servo.write(0.0f);                     // Back to minimum
    wait(8000);
}

/**
 * @brief Set the motor speed.
 * 
 * @param value A float from -1.0 (full reverse) to 1.0 (full forward).
 * Internally mapped to the servo's expected normalized range [0.0, 1.0].
 */
void Motor::setSpeed(float value) {
    // Clamp input to valid range
    if (value > 1.0f) value = 1.0f;
    if (value < -1.0f) value = -1.0f;

    _speed = value;

    // Map -1.0 → center - range, 0.0 → center, 1.0 → center + range
    float servoValue = _center + (_range * value);

    // Clamp servo output just in case
    if (servoValue > 1.0f) servoValue = 1.0f;
    if (servoValue < 0.0f) servoValue = 0.0f;

    _servo.write(servoValue);
}

/**
 * @brief Get the last speed value set.
 * 
 * @return float Speed in range [-1.0, 1.0]
 */
float Motor::getSpeed() const {
    return _speed;
}

/**
 * @brief Stop the motor by setting speed to 0 (neutral).
 */
void Motor::stop() {
    setSpeed(0.0f);
}
