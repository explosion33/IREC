#include "mbed.h"

// main() runs in its own thread in the OS

DigitalOut test (PC_13);
int main()
{
    while (true) {
       test = 0;
       wait_us(200);
       test=1;
       wait_us(200);
    }
}

