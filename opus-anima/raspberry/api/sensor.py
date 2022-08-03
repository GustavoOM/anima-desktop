from base import get_all_subclasses
from metadata import SensorMetadata
from validation import CheckSensor


class Sensor(metaclass=CheckSensor):

	id = "generic"

	def __init__(self):
		self.metadata = SensorMetadata.empty()
		self.activated = False
		self.constrains = []
		self.triggers = [self.id]

	def activate(self, trigger, controller):
		if trigger in controller.data:
			if trigger in self.triggers:
				self.activated = True

	def extract(self, controller):
		if not self.activated:
			return None
		self.activated = False
		return controller.data[self.triggers[0]][-1]

	def range(self):
		min_value = self.metadata.min
		max_value = self.metadata.max
		if self.constrains:
			for constrain in self.constrains:
				if constrain.bound_type == 'max':
					max_value = constrain.limit
				elif constrain.bound_type == 'min':
					min_value = constrain.limit
		return (min_value, max_value)

	def info(self):
		return {"name": self.metadata.name, "range": self.range(), "id": self.id, "unit": self.metadata.unit}


class PlateauPressure(Sensor):

	id = "plateau50ms"

class FractionOfOxygen(Sensor):

	id = "mFIO2"


class VolumeMinute(Sensor):

	id = "vMinIns"


class PeekPressure(Sensor):

	id = "pPeek"


class AveragePressure(Sensor):

	id = "pAverage"


class InspirationTime(Sensor):

	id = "mTIns"


class Frequency(Sensor):

	id = "mFreq"


class TidalVolume(Sensor):

	id = "vTidalIns"


class PositiveExpiratoryPressure(Sensor):

	id = "peep50ms"

class StaticCompliance(Sensor):

	id = "cStat"

class DynamicCompliance(Sensor):

	id = "cDyn"



SENSORS = {sensor.id: sensor for sensor in get_all_subclasses(Sensor) if sensor.id != "generic"}


if __name__ == '__main__':
	pass
