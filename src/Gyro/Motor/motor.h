#ifndef MOTOR_H
#define MOTOR_H

#include "mbed.h"
#include "Servo.h"

/**
 * @brief A class for controlling a servo-based motor using the Servo class.
 * 
 * Provides high-level control over a servo motor by abstracting angle or speed settings.
 */
class Motor {
public:
    /**
     * @brief Construct a new Motor object.
     * 
     * @param pin The PWM pin connected to the servo motor.
     */
    Motor(PinName pin);

    /**
     * @brief Set motor power/speed.
     * 
     * @param value A value from -1.0 to 1.0, where 0 is center/stop,
     *              -1.0 is full reverse, and 1.0 is full forward.
     */
    void setSpeed(float value);

    /**
     * @brief Get the current motor speed setting.
     * 
     * @return float Current speed in range [-1.0, 1.0].
     */
    float getSpeed() const;

    /**
     * @brief Stop the motor (sets speed to 0).
     */
    void stop();

    /**
    * @brief Performs a startup calibration routine for an ESC.
    * Sends the necessary PWM signals to initialize most brushless ESCs.
    */
    void arm();

private:
    Servo _servo;
    float _speed;
};

#endif // MOTOR_H
