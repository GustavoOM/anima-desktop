import serial
import time

ser = serial.Serial('/dev/serial/by-id/usb-Arduino_LLC_Arduino_Due-if00', 115200, timeout=1)
ser.flush()

while True:
	ser.write(b"Hello from Raspberry Pi!\n")
	time.sleep(1)
