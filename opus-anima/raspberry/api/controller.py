from collections import deque
from base import INDICATORS_2API_MSG, GRAPHICS_ALARMS_2API_MSG, ALARM_LABELS
from guesser import BoundariesPanel, SensorsPanel, AlarmsPanel, GraphicsPanel
from itertools import chain

class Controller:

	HISTORY_SIZE = 1000
	parameters = ALARM_LABELS + [p.label for p in INDICATORS_2API_MSG.OUTPUT + INDICATORS_2API_MSG.SENSORS +
																								GRAPHICS_ALARMS_2API_MSG.OUTPUT + GRAPHICS_ALARMS_2API_MSG.SENSORS ]
	frequency_filter = 1

	def __init__(self, alarms=AlarmsPanel(), sensors=SensorsPanel(), 
				 boundaries=BoundariesPanel(), graphics=GraphicsPanel(frequency_filter)):
		self.data = {}
		self.frequency_step = 0
		self.graphics = graphics
		self.alarms = alarms
		self.sensors = sensors
		self.boundaries = boundaries
		self.boundaries.bound(self.sensors)
		self.triggers = {}
		self.audioled_signarr = None
		self.in_ventilation = False
		self.night_mode = False
		self.calibration = Calibration()
		self.comm_error = False
		for measure in chain(self.alarms.table.values(), self.sensors.table.values(), 
							 self.boundaries.table.values(), self.graphics.table.values()):
			for trigger in measure.triggers:
				self.triggers.setdefault(trigger, []).append(measure.id)

	def info(self):
		output = [sensor.info() for sensor in self.sensors.table.values()]
		graphics = [graphic.info() for graphic in self.graphics.table.values()]
		bounds = {}
		for bound in self.boundaries.table.values():
			bounds.setdefault(bound.bound_type, []).append(bound.info())
		return output, graphics, bounds

	def wait(self):
		if self.frequency_step == 0:
			return False
		return True

	def update(self, data, update_freq):
		if update_freq:
			self.frequency_step = (self.frequency_step + 1) % self.frequency_filter
		for key in data: 
			if key in self.parameters:
				self.data.setdefault(key, deque(maxlen=self.HISTORY_SIZE)).append(data[key])
				for triggered in self.triggers.get(key, []):
					for table in (self.sensors.table, self.alarms.table, 
								  self.boundaries.table, self.graphics.table):
						if triggered in table:
							table[triggered].activate(key, self)

	def output(self):
		output = []
		pressures = [{"id": "PAr", "value": self.data["PAr"][-1]}, {"id": "PO2", "value": self.data["PO2"][-1]}]
		for id, sensor in self.sensors.table.items():
			value = sensor.extract(self)
			if value:
				output.append({"name": sensor.metadata.name,"id": id, "value": value, "range": sensor.range()})
		return {"output": output, "pressures": pressures}

	def draw(self):
		graphics = []
		phase = self.data["phase"][-1]
		for graphic in self.graphics.table.values():
			timestamp, lines = graphic.extract(self)
			for line in lines:
				graphics.append({"name": graphic.metadata.y_name, "timestamp": timestamp, "value":line})
		return {"graphics": graphics, "phase": phase}

	def emit_alert(self):
		alerts = {}
		max_severity = 0
		for alert in chain(self.alarms.table.values(), self.boundaries.table.values()):
			data = alert.extract(self)
			if data:
				alerts.setdefault(data["severity"], []).append(data)
				if data["severity"] > max_severity:
					max_severity = data["severity"]
		self.set_alert_audio_level(max_severity)
		if alerts:
			if self.in_ventilation:
				return {"alerts": alerts[max(alerts.keys())]}
			else:
				alerts_array = []
				for sev in alerts:
					alerts_array.extend(alerts[sev])
				return {"alerts": alerts_array}
		return None

	def set_alert_audio_level(self, max_severity):
		if self.audioled_signarr is not None:
			if max_severity == 2:
				self.audioled_signarr[2] = 1
			elif max_severity == 1:
				self.audioled_signarr[1] = 1
			else:
				self.audioled_signarr[2] = 0
				self.audioled_signarr[1] = 0

	def set_silence(self, value):
		if self.in_ventilation:
			self.audioled_signarr[0] = value

	def set_graphic_pause(self, pause):
		for graphic in self.graphics.table.values():
			graphic.set_pause(pause)

	def set_in_ventilation(self, is_in_ventilation):
		self.in_ventilation = is_in_ventilation
		# Disable all audio while not on ventilation
		if not self.in_ventilation:
			self.audioled_signarr[0] = 2

	def set_night_mode(self, night_mode):
		self.night_mode = night_mode
		if self.night_mode:
			self.audioled_signarr[3] = 1
		else:
			self.audioled_signarr[3] = 0

	def notify_writer_fail(self):
		self.comm_error = True

	def notify_reader_idle(self):
		if self.in_ventilation:
			self.comm_error = True

	def is_there_comm_error(self):
		return self.comm_error

	def __len__(self):
		total = 0
		for history in self.data.values():
			total += len(history)
		return total


class Calibration():
	def __init__(self):
		self.message_type = 0
		self.version = 0
		self.timestamp = 0
		self.progress = 0
		self.activated = False

	def start(self):
		self.message_type = 0
		self.version = 0
		self.timestamp = 0
		self.progress = 0

		self.activated = True

	def report(self, data):
		if self.activated:
			self.message_type = data['MessageType']
			self.version = data['version']
			self.timestamp = data['timestamp']
			self.progress = data['progress']

			if self.progress == 10:
				self.activated = False

	def check(self):
		return self.progress

if __name__ == '__main__':
	from random import randrange
	monitor = Controller()
	print(monitor.info())
