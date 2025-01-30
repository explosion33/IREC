# BNO055 Sensor Driver

## Overview

The **BNO055 Sensor Driver** is a C++ library designed for stm32 based systems running mbedOS.

## Key Features

- **Initialization & Configuration**
  - Initialize the BNO055 sensor with customizable I2C settings.
  - Configure various power modes (Normal, Low Power, Suspend).
  - Set operation modes tailored to specific application needs.

- **Data Acquisition**
  - Retrieve comprehensive sensor data including:
    - Accelerometer
    - Gyroscope
    - Magnetometer
    - Euler angles
    - Linear acceleration
    - Gravity
    - Quaternion vectors

- **Calibration Management**
  - Check calibration status for system, gyroscope, accelerometer, and magnetometer.
  - Perform and validate self-tests to ensure sensor integrity.

- **Advanced Configuration**
  - Customize sensor ranges, bandwidths, and axis mappings.
  - Manage sensor offsets and radius values for enhanced accuracy.
  - Set unit preferences for measurements.

- **Diagnostics & Error Handling**
  - Access system error and status codes for troubleshooting.
  - Perform hardware and software resets to maintain optimal performance.

## Usage

**Sample Code**

   Ensure that the `BNO055.h` header file is included in your project.

   ```cpp
    #include "mbed.h"
    #include "BNO055.h"

    // Define generic I2C pin names
    #define SDA_PIN P0_0
    #define SCL_PIN P0_1
    #define addr = 0x28 << 1 // 8 bit address
    BNO055 sensor(SDA_PIN, SCL_PIN, addr);

    int main() {
        // Initialize the sensor
        if (sensor.setup() != BNO055Result::Ok) {
            printf("Sensor initialization failed.\n");
            return -1;
        }

        // Main loop
        while (1) {
            // Retrieve accelerometer data
            bno055_vector_t accel = sensor.getAccelerometer();
            printf("Accel: X=%.2f Y=%.2f Z=%.2f\n", accel.x, accel.y, accel.z);
            wait(1);
        }
    }