#include "mbed.h"
#include "encoder.h"

encoder::encoder(PinName A, PinName B){
    pinA = A;
    pinB = B;
    position = 0;
    lastState = 0;
    direction = 0;
}

void encoder::setup(){
    pinA.mode(PullUp);
    pinB.mode(PullUp);
    lastState = (static_cast<uint8_t>(pinA.read()) << 1) | pinB.read();

    pinA.fall(callback(this, &encoder::interruptA()));
    pinA.rise(callback(this, &encoder::interruptA()));
    pinB.fall(callback(this, &encoder::interruptB()));
    pinB.rise(callback(this, &encoder::interruptB()));
}
int16_t encoder::getPos(){
    __disable_irq();
    int32_t pos = position;
    __enable_irq();
    return pos;
}

int encoder::getDirection(){
    __disable_irq();
    int dir = direction;
    __enable_irq();
    return dir;
}

void encoder::reset(){
    __disable_irq();
    position = 0;
    direction = 0;
    __enable_irq();
}

float encoder::getAngleDeg(){
    float revolutions = static_cast<float>(position) / (ppR * 4); // 4 counts per pulse
    revolutions /= gearRatio;
    return revolutions * 360.0f;
}

float encoder::getAngleRad(){
    float revolutions = static_cast<float>(position) / (ppR * 4); // 4 counts per pulse
    revolutions /= gearRatio;
    return revolutions * 2.0f * 3.14;
}

void encoder::interruptA(){
    uint8_t currentA = pinA.read();
    uint8_t currentB = pinB.read();
    uint8_t state = (currentA << 1) | currentB;
    processState(state);
}

void encoder::interruptB(){
    uint8_t currentA = pinA.read();
    uint8_t currentB = pinB.read();
    uint8_t state = (currentA << 1) | currentB;
    processState(state);
}

void encoder::processState(uint8_t state){
    uint8_t prev = lastState;
    lastState = state;

    int8_t diff = (prev << 2) | state;
    static const int8_t lookup[16] = {
        0,  -1, 1,  0,
        1,  0,  0, -1,
        -1, 0,  0, 1,
        0,  1, -1, 0
    };

    int8_t movement = lookup[diff & 0x0F];

    if (movement != 0) {
        position += movement;
        direction = movement;
    }
}