#include "mbed.h"

class encoder {
public:
    encoder(PinName A, PinName B);

    int16_t getPos();
    float getAngleDeg();
    int getDirection();
    float getAngleRad();

    void reset();
    void setup();

private:
    InterruptIn pinA;
    InterruptIn pinB;

    volatile int direction;
    volatile int32_t position;
    volatile int8_t lastState;

    float gearRatio;
    int16_t ppR;

    void interruptA();
    void interruptB();
    void processState(uint8_t state);
};