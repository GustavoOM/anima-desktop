from validation import CheckStrategy
from guesser import ParameterGuesser


class Strategy(metaclass=CheckStrategy):

	nickname = 'generic'

	@classmethod
	def info(cls):
		return {'mode': cls.nickname, "id": cls.__name__, 'configuration': [{"name": meta.name, "id": id, "range": (meta.min, meta.max), "unit": meta.unit,
	                "step": meta.step, "default": meta.default} for id, meta in cls.configuration.items()]}

	def __init__(self):
		self.parameters = {"mode": self.nickname}

	def __str__(self):
		name = self.nickname + ":"
		for key in self.configuration:
			name += "|{}={:.2f}".format(key, self.parameters[key])
		name += "|"
		return name

	def configure(self, data):
		updated = False
		for key, value in data.items(): 
			if key in self.configuration:
				self.parameters[key] = value
				updated = True		
		return updated

	def owns(self, data):
		if data["mode"] != self.nickname:
			return False
		for key in self.configuration:
			if key not in data:
				return False
			if self.parameters[key] != data[key]:
				return False
		return True

	def is_valid(self):
		for key, meta in self.configuration.items():
			if key not in self.parameters:
				return False
			if self.parameters[key] < meta.min or self.parameters[key] > meta.max:
				return False
		return True


class VolumeController(Strategy):

	identifiers = ["vTidal", "FIO2", "shape", "tIns", "Freq", "PEEP", 
		"sensInsP", "sensInpF", "tPausaIns", "pInsLimit"] 
	configuration = {id: ParameterGuesser.data[id] for id in identifiers}
	nickname = 'VCV'

class PressureController(Strategy):

	identifiers = ["pCon", "FIO2", "riseTime", "tIns", "Freq", "PEEP", 
		"sensInsP", "sensInpF"] 
	configuration = {id: ParameterGuesser.data[id] for id in identifiers}
	nickname = 'PCV'


class PressureSupport(Strategy):

	identifiers = ["FIO2", "riseTime", "tInsMax", "pSupport", "PEEP", 
		"sensInsP", "sensInpF", "sensExpF", "bkMode", "bkTIns", "bkFreq",
		"bkVTidal", "bkPcon", "notBreathing"] 
	configuration = {id: ParameterGuesser.data[id] for id in identifiers}
	nickname = 'PSV'


# This line of code automatically creates a dictionary with all the
# strategies avaiable, i.e. all the classes that inherit from Strategy.
# The only thing that the application needs to import is this dict.
MODES = {mode.nickname: mode
			for mode in Strategy.__subclasses__()}


if __name__ == '__main__':
	pass
