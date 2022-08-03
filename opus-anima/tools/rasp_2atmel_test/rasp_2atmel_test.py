import serial
import time

ser = serial.Serial('/dev/serial0', 115200, timeout=1)
ser.flush()

while True:
	ser.write(b"Hello from Raspberry Pi!\n")
	time.sleep(1)
