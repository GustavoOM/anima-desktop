from sensor import Sensor 
from base import get_all_subclasses
from metadata import AlarmMetadata
from logger import AlarmLogger, FakeAlarmLogger
from validation import CheckAlarm, CheckNothing


class Alarm(Sensor, metaclass=CheckAlarm):
	"""
	Alarm has five values in metadata: 
	 - id.
	 - severity: (int) higher more severe, -1 not appear on screen.
	 - message: (str) error message to be shown on screen and on DB.
	 - watch_value_id: (str) value to show when alarm is triggered, can be None.
	 - unit: (str) unit of watched value.
	"""

	logger = AlarmLogger()
	id = "generic"

	def __init__(self):
		super().__init__()
		self.metadata = AlarmMetadata.empty()
		self.state = 0
		self.watch_value = None

	def activate(self, trigger, controller):
		super().activate(trigger, controller)
		if self.activated:
			self.logger.begin_track(self)

	def extract(self, controller):
		self.state = super().extract(controller)
		return_message = None
		if self.state:
			return_message = {'id': self.id, 'message': self.metadata.message, 
					'severity': self.metadata.severity}
			if self.metadata.watch_value_id:
					self.watch_value = controller.data[self.metadata.watch_value_id][-1]
		self.logger.save(self)
		return return_message


class AlarmBatch(Alarm, metaclass=CheckNothing):

	logger = FakeAlarmLogger()


class PressureSensorsFailure(AlarmBatch):

	id = "pressureSensors"

	def __init__(self):
		super().__init__()
		self.triggers = ["insPressureSensor", "expPressureSensor"]


class notBreathing(Alarm):
	id = "notBreathing"

class backupOn(Alarm):
	id = "backupOn"

class battery75(Alarm):
	id = "battery75"

class battery50(Alarm):
	id = "battery50"

class battery25(Alarm):
	id = "battery25"

class unplugged(Alarm):
	id = "unplugged"

class leaking(Alarm):
	id = "leaking"

class communication(Alarm):
	id = "communication"

class expObs1(Alarm):
	id = "expObs1"

class expObs2(Alarm):
	id = "expObs2"

class expObs3(Alarm):
	id = "expObs3"

class maxPEEP(Alarm):
	id = "maxPEEP"

class autoPEEP(Alarm):
	id = "autoPEEP"

class pInsMax(Alarm):
	id = "pInsMax"

class VTidalInsMax(Alarm):
 	id = "vTidalInsMax"

class pAirMin(Alarm):
	id = "pAirMin"

class pAirMinWarning(Alarm):
	id = "pAirMinWarning"

class pO2Min(Alarm):
	id = "pO2Min"

class pO2MinWarning(Alarm):
	id = "pO2MinWarning"

class fio2max(Alarm):
	id = "fio2max"

class fio2min(Alarm):
	id = "fio2min"

class noPower(Alarm):
	id = "noPower"

class freqMax(Alarm):
	id = "freqMax"

class pInsMin(Alarm):
	id = "pInsMin"

class vTidalInsMin(Alarm):
	id = "vTidalInsMin"

class vMinMax(Alarm):
	id = "vMinMax"

class vMinMin(Alarm):
	id = "vMinMin"

class invIE(Alarm):
	id = "invIE"

class converters(Alarm):
	id = "converters"

class invalidCommand(Alarm):
	id = "invalidCommand"

class pilotPress(Alarm):
	id = "pilotPress"


ALARMS = {alarm.id: alarm for alarm in get_all_subclasses(Alarm) if alarm.id != "generic"}


if __name__ == '__main__':
	pass
