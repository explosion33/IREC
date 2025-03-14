#ifndef ENCODER_H
#define ENCODER_H

#include "mbed.h"

/**
 * @brief A quadrature encoder that operates using a look up table
 *
 * Provides getters for count, orientation (degrees, radians), 
 * and last direction of movement.
 */
class encoder {
public:
    /**
     * @param channelA      PinName for Encoder Channel A
     * @param channelB      PinName for Encoder Channel B
     * @param pulsesPerRev  PPR (pulses per revolution) for one channel.
     */
    encoder(PinName channelA, PinName channelB, int pulsesPerRev);

    /**
     * @return Current encoder count (pulses) relative to the starting position.
     */
    int getCount() const;

    /**
     * @return Absolute orientation in degrees, from the starting position.
     */
    float getOrientationDegrees() const;

    /**
     * @return Absolute orientation in radians, from the starting position.
     */
    float getOrientationRadians() const;

    /**
     * @return Relative number of revolutions, from the starting position
     */
    float getRevolutions() const;

    /**
     * @return Last movement direction:
     *          +1 = forward, -1 = backward, 0 = no movement or invalid step
     */
    int getDirection() const;

    /**
     * @brief Reset the encoder count to zero (and direction to 0).
     */
    void reset();

private:
    /**
     * @brief Common interrupt service routine for any change on Channel A or B.
     */
    void encodeISR();

private:
    InterruptIn _chanA;
    InterruptIn _chanB;

    volatile int    _position;      ///< Current pulse count
    const int       _pulsesPerRev;  ///< Nominal pulses per revolution (one channel)
    volatile uint8_t _prevState;    ///< Previous 2-bit state of (A,B)

    // We'll store the last direction of movement:
    // +1 = forward, -1 = backward, 0 = no valid change
    volatile int    _direction;
};

#endif // ENCODER_H
