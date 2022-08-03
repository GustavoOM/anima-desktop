import serial

ser = serial.Serial('/dev/serial0', 115200, timeout=1)
ser.flush()
while True:
	if ser.in_waiting > 0:
		line = ser.read()
		print(line)
