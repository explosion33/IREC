#include "motor.h"
#include "func.h"

/**
 * @brief Construct a new Motor object.
 * Initializes the internal Servo and sets it to neutral (stop).
 */
Motor::Motor(PinName pin)
    : _servo(pin), _speed(0.0f) {
}

void Motor::arm() {
    _servo.write(1.0);
    ThisThread::sleep_for(1000ms);
    _servo.write(0.0);
    ThisThread::sleep_for(1000ms);
}

/**
 * @brief Set the motor speed.
 * 
 * @param value A float from 0.0 (stopped) to 1.0 (full forward).
 */
void Motor::setSpeed(float value) {
    if (value > 1.0f) value = 1.0f;
    if (value < 0.0f) value = 0.0f;

    _speed = value;

    _servo.write(_speed);
}

/**
 * @brief Get the last speed value set.
 * 
 * @return float Speed in range [0.0, 1.0]
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
