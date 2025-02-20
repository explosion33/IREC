#include "mbed.h"

void wait(int time_ms) {
    wait_us(time_ms *1000);
}

// void wait (int time_ms){
//     ThisThread::sleep_for(time_ms);
// }