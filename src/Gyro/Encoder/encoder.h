#ifndef ENCODER_H
#define ENCODER_H

#include "mbed.h"

/**
 * @brief A quadrature encoder class that uses interrupts and a lookup table
 *        to track position and direction.
 *
 * Provides thread-safe access to encoder pulse count, orientation in degrees/radians,
 * number of revolutions, and last movement direction.
 */
class encoder {
public:
    /**
     * @brief Construct a new encoder object with two input channels.
     * 
     * @param channelA      PinName for encoder channel A.
     * @param channelB      PinName for encoder channel B.
     * @param pulsesPerRev  Number of pulses per full revolution (PPR) for one channel.
     */
    encoder(PinName channelA, PinName channelB, int pulsesPerRev);

    /**
     * @brief Get the current pulse count relative to the starting/reset position.
     * 
     * @return int Current encoder count.
     */
    int getCount() const;

    /**
     * @brief Get the absolute orientation in degrees.
     * 
     * @return float Orientation in degrees [0, 360) relative to reset position.
     */
    float getOrientationDegrees() const;

    /**
     * @brief Get the absolute orientation in radians.
     * 
     * @return float Orientation in radians [0, 2π) relative to reset position.
     */
    float getOrientationRadians() const;

    /**
     * @brief Get the total number of revolutions since reset.
     * 
     * @return float Relative revolutions (can be fractional).
     */
    float getRevolutions() const;

    /**
     * @brief Get the last detected direction of movement.
     * 
     * @return int +1 (forward), -1 (backward), or 0 (no movement or invalid step).
     */
    int getDirection() const;

    /**
     * @brief Reset encoder count and direction to zero.
     */
    void reset();

private:
    /**
     * @brief Interrupt handler called on any edge of either channel.
     *        Computes new state and updates position and direction.
     */
    void encodeISR();

    InterruptIn _chanA;         ///< Encoder input channel A
    InterruptIn _chanB;         ///< Encoder input channel B

    
    volatile int _position;     ///< Current encoder position (pulse count)
    const int _pulsesPerRev;    ///< Pulses per full revolution
    volatile uint8_t _prevState;///< Previous 2-bit state of (A, B)
    volatile int _direction;    ///< Last detected direction of rotation
};

#endif // ENCODER_H
