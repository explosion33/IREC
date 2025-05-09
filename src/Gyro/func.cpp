#include "mbed.h"
#include "USBSerial.h"

void wait (int time_ms){
    ThisThread::sleep_for(time_ms);
}

