#include "mbed.h"
#include "BNO055.h"
// main() runs in its own thread in the OS

DigitalOut test (PC_13);
int main()
{
    while (true) {
       test = 0;
       ThisThread::sleep_for(200ms);
       test = 1;
       ThisThread::sleep_for(200ms);
    }
}

