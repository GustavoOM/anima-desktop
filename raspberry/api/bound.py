from alarm import Alarm
from base import get_all_subclasses
from metadata import BoundMetadata
from validation import CheckBound


class Bound(Alarm, metaclass=CheckBound):

	bound_type = "set"
	id = "generic"

	def __init__(self):
		super().__init__()
		self.metadata = BoundMetadata.empty()
		self.limit = self.metadata.default
		self.watch_value = None

	def check(self, value, limit=None):
		if not limit:
			return self.limit
		return limit

	def activate(self, trigger, controller):
		super().activate(trigger, controller)
		if self.metadata.bound_id in controller.data:
			sensor_value = controller.data[self.metadata.bound_id][-1]
			if self.watch_value == None or not self.check(sensor_value, limit=self.watch_value):
				self.watch_value = sensor_value

	def extract(self, controller):
		message = super().extract(controller)
		if message:
			message["bound_id"] = self.metadata.bound_id
			message["value"] = self.watch_value
		else:
			self.watch_value = None
		return message

	def info(self):
		return {"name": self.metadata.name, "bound_id": self.metadata.bound_id, "id": self.id, "unit": self.metadata.unit, 
				"default": self.metadata.default, "step": self.metadata.step, "range":self.range()}


class UpperBound(Bound):

	bound_type = "max"
	id = "generic"

	def check(self, value, limit=None):
		limit = super().check(value, limit=limit)
		return value <= limit


class LowerBound(Bound):

	bound_type = "min"
	id = "generic"

	def check(self, value, limit=None):
		limit = super().check(value, limit=limit)
		return value >= limit


class MaxFractionOfOxygen(UpperBound):

	id = "fio2max"


class MinFractionOfOxygen(LowerBound):

	id = "fio2min"


class MaxVolumeMinute(UpperBound):

	id = "vMinMax"


class MinVolumeMinute(LowerBound):

	id = "vMinMin"


class MaxPeekPressure(UpperBound):

	id = "pInsMax"


class MinPeekPressure(LowerBound):

	id = "pInsMin"


class MaxFrequency(UpperBound):

	id ="freqMax"


class MinTidalVolume(LowerBound):

	id = "vTidalInsMin"


class MaxTidalVolume(UpperBound):

	id = "vTidalInsMax"


class MaxPositiveExpiratoryPressure(UpperBound):

	id = "maxPEEP"
	

BOUNDS = {bound.id: bound for bound in get_all_subclasses(Bound) if bound.id != "generic"}


if __name__ == '__main__':
	pass
