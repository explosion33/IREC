#include "mbed.h"
#include <cstdint>
#include <cstring>
#include <vector>
#include "EUSBSerial.h"
#include <string>

// USB Serial to PC
EUSBSerial pc;

// UART connection to target device
BufferedSerial uart(PA_2, PA_3, 115200);

// ===== COBS ENCODING FUNCTION =====
void cobs(std::vector<uint8_t>& buf, uint8_t start) {
    size_t count = 0;
    size_t last_i = 2;

    for (size_t i = 3; i < buf.size(); ++i) {
        count += 1;
        if (buf[i] == start) {
            buf[last_i] = static_cast<uint8_t>(count);
            last_i = i;
            count = 0;
        }
    }

    buf[last_i] = 0;
}

// ===== UART WRITE FUNCTION WITH COBS =====
void write(BufferedSerial* ser, const uint8_t* data, size_t len) {
    if (!ser || len + 4 > 256) return;

    std::vector<uint8_t> buf(len + 3, 0); // len + 4 to include \n terminator

    for (size_t i = 0; i < len; ++i) {
        buf[i + 3] = data[i];
    }

    buf[0] = 234;
    buf[1] = static_cast<uint8_t>(len + 3);
    buf[2] = 0;

    cobs(buf, 234);

    // buf[len + 3] = '\n'; // end of frame

    ser->write(buf.data(), buf.size());
}

bool parse(const char* data, size_t length, std::string& result) {
    static std::string buffer;
    buffer.append(data, length);

    size_t start = buffer.find('"');
    while (start != std::string::npos) {
        size_t end = buffer.find('"', start + 1);
        if (end == std::string::npos) {
            // Incomplete: keep only from opening quote onward
            if (start > 0) buffer.erase(0, start);
            return false;
        }

        // Extract and filter printable ASCII chars only
        std::string raw = buffer.substr(start + 1, end - start - 1);
        std::string cleaned;
        for (char c : raw) {
            if (c >= 32 && c <= 126) {
                cleaned += c;
            }
        }

        result = cleaned;
        buffer.erase(0, end + 1);  // Trim processed portion
        return true;
    }

    // No quote found at all
    buffer.clear();
    return false;
}

int main() {
    uart.set_blocking(true);
    pc.printf("Bidirectional UART <-> PC ready. Type and press Enter:\r\n");

    char input_buf[128];
    char uart_buf[256];
    uint8_t empty_buf[1];


    while (true) {
        // === PC to UART ===
        if (pc.readline(input_buf, sizeof(input_buf))) {
            write(&uart, reinterpret_cast<uint8_t*>(input_buf), strlen(input_buf)); 
            pc.printf("Sent: %s\r\n", input_buf);
        }

        // === UART to PC (raw passthrough) ===
        while (uart.readable()) {
            char uart_buf[128];
            std::string message;

            ssize_t n = uart.read(uart_buf, sizeof(uart_buf));
            if (n > 0) {
                // if (parse(uart_buf, n, message)) {
                //     pc.printf("Received message: %s\n", message.c_str());
                // }
                pc.printf("%s", uart_buf.c_str());
            }
        }
    }
}
