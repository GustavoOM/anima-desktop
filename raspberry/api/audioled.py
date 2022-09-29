import RPi.GPIO as gpio
from os import system
from threading import Thread
from time import sleep, time

RED_LIGHT_PIN = 32
GREEN_LIGHT_PIN = 26

def player(signarr):
	mute_before = 0
	mute_time = 0

	while True:
		if signarr[0] == 0 and signarr[2] == 1:
			if signarr[3] == 0:
				system("omxplayer -o local --vol -500 api/resources/alta_quadrada.wav > /dev/null")
			else:
				system("omxplayer -o local --vol -2000 api/resources/alta_quadrada.wav > /dev/null")
		elif signarr[0] == 0 and signarr[1] == 1:
			if signarr[3] == 0:
				system("omxplayer -o local --vol -1000 api/resources/media_quadrada.wav > /dev/null")
			else:
				system("omxplayer -o local --vol -2500 api/resources/media_quadrada.wav > /dev/null")
		else:
			if signarr[0] == 1:
				if mute_before == 0:
					mute_time = time()
					mute_before = 1
				else:
					if time() - mute_time > 120:
						signarr[0] = 0
						mute_before = 0
			sleep(0.1)

def blinker(signarr):
	while True:
		if signarr[2] == 1:
			gpio.output(RED_LIGHT_PIN, gpio.HIGH)
			sleep(0.25)
			gpio.output(RED_LIGHT_PIN, gpio.LOW)
			sleep(0.25)
		elif signarr[1] == 1:
			gpio.output(RED_LIGHT_PIN, gpio.HIGH)
			gpio.output(GREEN_LIGHT_PIN, gpio.HIGH)
			sleep(0.25)
			gpio.output(RED_LIGHT_PIN, gpio.LOW)
			gpio.output(GREEN_LIGHT_PIN, gpio.LOW)
			sleep(0.25)
		else:
			sleep(0.1)

def setup_audioled(signarr):
	gpio.setmode(gpio.BOARD)
	gpio.setwarnings(False)
	gpio.setup(RED_LIGHT_PIN, gpio.OUT)
	gpio.setup(GREEN_LIGHT_PIN, gpio.OUT)
	gpio.output(RED_LIGHT_PIN, gpio.LOW)
	gpio.output(GREEN_LIGHT_PIN, gpio.LOW)

	t1 = Thread(target=player, args=(signarr,))
	t1.start()
	t2 = Thread(target=blinker, args=(signarr,))
	t2.start()
