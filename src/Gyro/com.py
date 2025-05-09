import serial
import time
import pandas as pd
import numpy as np


# A short script to read sensor data from the serial bus and store it into a csv file

COM_PORT = 'COM4'
BAUD_RATE = 9600
CSV_FILE = 'bno055_data.csv'
print(serial.__file__)
ser = serial.Serial(COM_PORT, BAUD_RATE, timeout=1)

columns = [
    'ACC_X', 'ACC_Y', 'ACC_Z',
    'MAG_X', 'MAG_Y', 'MAG_Z',
    'GYR_X', 'GYR_Y', 'GYR_Z',
    'EUL_X', 'EUL_Y', 'EUL_Z',
    'LINACC_X', 'LINACC_Y', 'LINACC_Z',
    'GRAV_X', 'GRAV_Y', 'GRAV_Z',
    'QUAT_X', 'QUAT_Y', 'QUAT_Z'
] # column names
df = pd.DataFrame(columns = columns)
try:    
    print("Reading from Serial Port")
    while True:
        line = ser.readline().decode('utf-8').strip()
        print(line)
        if line:
            values = line.split(' ')
            if len(values) == len(columns):
                try:
                    float_values = [float(v) for v in values]
                    df.loc[len(df)] = float_values
                    print(f"Logged row: {float_values}")
                except ValueError:
                    print(f"Skipping invalid line: {line}")

except KeyboardInterrupt:
    print("\nStopping data collection.")
finally:
    ser.close()
    df.to_csv(CSV_FILE, index=False)
    print(f"Data saved to {CSV_FILE}")
    