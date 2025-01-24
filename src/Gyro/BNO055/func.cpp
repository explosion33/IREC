#include "mbed.h"
void wait(int time_ms) {
    ThisThread::sleep_for(time_ms);
}