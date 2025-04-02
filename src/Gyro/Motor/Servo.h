#ifndef MBED_SERVO_H
#define MBED_SERVO_H

#include "mbed.h"

/** Servo control class, based on a PwmOut
 *
 * Example:
 * @code
 * // Continuously sweep the servo through it's full range
 * #include "mbed.h"
 * #include "Servo.h"
 * 
 * Servo myservo(p21);
 * 
 * int main() {
 *     while(1) {
 *         for(int i=0; i<100; i++) {
 *             myservo = i/100.0;
 *             wait(0.01);
 *         }
 *         for(int i=100; i>0; i--) {
 *             myservo = i/100.0;
 *             wait(0.01);
 *         }
 *     }
 * }
 * @endcode
 */
class Servo {

public:
    /** Create a servo object connected to the specified PwmOut pin
     *
     * @param pin PwmOut pin to connect to 
     */
    Servo(PinName pin);
    
    /** Set the servo position, normalised to it's full range
     *
     * @param percent A normalised number 0.0-1.0 to represent the full range.
     */
    void write(float percent);
    
    /**  Read the servo motors current position
     *
     * @param returns A normalised number 0.0-1.0  representing the full range.
     */
    float read();
    
    /** Set the servo position
     *
     * @param degrees Servo position in degrees
     */
    void position(float degrees);
    
    /**  Allows calibration of the range and angles for a particular servo
     *
     * @param range Pulsewidth range from center (1.5ms) to maximum/minimum position in seconds
     * @param degrees Angle from centre to maximum/minimum position in degrees
     */
    void calibrate(float range = 0.0005, float degrees = 45.0); 
        
    /**  Shorthand for the write and read functions */
    Servo& operator= (float percent);
    Servo& operator= (Servo& rhs);
    operator float();

protected:
    PwmOut _pwm;
    float _range;
    float _degrees;
    float _p;
};

#endif
