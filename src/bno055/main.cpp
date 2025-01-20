#include "mbed.h"
//#include "bno055.cpp" // todo: swap these to .h files
// main() runs in its own thread in the OS
#include "func.h"
DigitalOut test (PC_13);
int main()
{
    while (true) {

       test = 0;
       wait(100);
       test = 1;
       wait(100);
    }
}

