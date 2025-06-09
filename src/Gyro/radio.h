#include "mbed.h"
#include <cstdint>
#include <string>
#include "EUSBSerial.h"

void cobs(uint8_t* Vec, uint8_t start);
void writeUART(BufferedSerial* ser, const uint8_t* data, size_t len);
bool parse(const char* data, size_t length, std::string& result);