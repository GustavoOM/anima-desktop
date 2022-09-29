import os, sys
sys.path.append(os.path.abspath(os.path.join('api')))
from signal import signal, SIGTERM
from multiprocessing import Process, Array
from multiprocessing.connection import wait
from subprocess import Popen
import webbrowser

from sqlalchemy.sql import text

from logger import engine
from write_serial import write_serial
from application import start_api
from read_serial import read_serial

db = engine.connect()
was_shutdown = text("SELECT shutdown FROM STATE;")

def kill_all(processes):
	for p in processes:
		p.terminate()

def run_front():
	try:
		sub = Popen(["chromium-browser", "--kiosk", "--disable-features=TranslateUI", "--noerrdialogs", "--disable-infobars", "frontend/test-cycle.html"])
		#sub = Popen(["chromium-browser", "frontend/test-cycle.html"])
		def suicide(signum, frame):
			sub.kill()
			sys.exit()
		signal(SIGTERM, suicide)
		sub.wait()
	except SystemExit:
		pass

def add_process(name, metadata, sentinels, shared=None):
	if shared is None:
		new = Process(name=name, target=metadata[name], daemon=False)
	else:
		new = Process(name=name, target=metadata[name], daemon=False, args=(shared,))
	new.start()
	print("Process started: " + new.name + " " + str(new.pid))
	sentinels[new.sentinel] = new

def main():
	metadata = {"write_messages": write_serial,
				"api": start_api,
				"read_messages": read_serial,
				"front": run_front}
	sentinels = {}

	audioled_signarr = Array("i", range(4))
	audioled_signarr[0] = 2
	audioled_signarr[1] = 0
	audioled_signarr[2] = 0

	# Night Mode
	audioled_signarr[3] = 0

	add_process("write_messages", metadata, sentinels)
	add_process("api", metadata, sentinels, shared=audioled_signarr)
	add_process("read_messages", metadata, sentinels)
	add_process("front", metadata, sentinels)

	try:
		while True:
			response = wait(sentinels.keys())
			for item in response:
				name = sentinels[item].name
				del sentinels[item]
				print(bool(db.execute(was_shutdown).fetchone()[0]))
				if name == "api" and bool(db.execute(was_shutdown).fetchone()[0]):
					kill_all(sentinels.values())
					os.system("sudo shutdown -h now")
					return
				add_process(name, metadata, sentinels)
	except KeyboardInterrupt:
		kill_all(sentinels.values())


if __name__ == "__main__":
	main()
