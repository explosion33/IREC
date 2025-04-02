import serial
import time
import pandas as pd
import numpy as np


# A short script to read sensor data from the serial bus and store it into a csv file

COM_PORT = 'COM7'
BAUD_RATE = 9600
CSV_FILE = 'bno055_data.csv'

ser = serial.Serial(COM_PORT, BAUD_RATE, timeout=1)

columns = ['GYR'] # column names
df = pd.DataFrame(columns = columns)

try:
    print("Reading from Serial Port")
    while True:
        line = ser.readline().decode('utf-8').strip()
        if line:
            values = line.split(' ')
            df.loc[len(df)] = values

except KeyboardInterrupt:
    print("\nStopping data collection.")
finally:
    ser.close()
    df.to_csv(CSV_FILENAME, index=False, header=False)
    print(f"Data saved to {CSV_FILENAME}")
    