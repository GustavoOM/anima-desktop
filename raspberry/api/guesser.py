from collections import namedtuple
from base import load_config_file, load_vert_config_file
from metadata import ParameterMetadata
from bound import BOUNDS
from sensor import SENSORS
from alarm import ALARMS
from graphics import GRAPHICS


PARAM_ADULT_FILENAME = "other/strategy_parameters_adult.cfg"
PARAM_CHILD_FILENAME = "other/strategy_parameters_child.cfg"

class ParameterGuesser:

	configuration = {"adult": load_config_file(PARAM_ADULT_FILENAME),
					 "child": load_config_file(PARAM_CHILD_FILENAME)}
	data = {item['id']: ParameterMetadata.empty() for item in 
		configuration["adult"]}

	def mount(self, option):
		for metadata in self.configuration[option]:
			self.data[metadata['id']].update(**metadata)

	def guess(self, patient):
		self.mount(patient.age_group)


CONTROL_ADULT_FILENAME = "calibration/current/general_adult.cfg"
CONTROL_CHILD_FILENAME = "calibration/current/general_child.cfg"
PID_ADULT_FILENAME = "calibration/current/pid_adult.cfg"
PID_CHILD_FILENAME = "calibration/current/pid_child.cfg"

class PidParameters:
	configuration = {"adult": load_config_file(PID_ADULT_FILENAME),
				"child": load_config_file(PID_CHILD_FILENAME)}
	def guess(self, patient):
		return self.configuration[patient.age_group]

class ControlParameters:
	configuration = {"adult": load_config_file(CONTROL_ADULT_FILENAME),
				"child": load_config_file(CONTROL_CHILD_FILENAME)}
	def guess(self, patient):
		return self.configuration[patient.age_group]


PRESSENS_FILENAME = "calibration/current/pressens.cfg"
FLUXSENS_FILENAME = "calibration/current/fluxsens.cfg"
EXAVALV_FILENAME = "calibration/current/exavalv.cfg"
AOPVALVS_FILENAME = "calibration/current/aopvalvs.cfg"

class PressensParameters:

	parameters = load_vert_config_file(PRESSENS_FILENAME)

	def get(self):
		return self.parameters

class FluxsensParameters:

	parameters = load_vert_config_file(FLUXSENS_FILENAME)

	def get(self):
		return self.parameters

class ExavalvParameters:

	parameters = load_vert_config_file(EXAVALV_FILENAME)

	def get(self):
		return self.parameters

class AopvalvsParameters:

	parameters = load_vert_config_file(AOPVALVS_FILENAME)

	def get(self):
		return self.parameters


BOUND_METADATA = "metadata/bound.cfg"
SENSOR_METADATA = "metadata/sensors.cfg"
ALARM_METADATA = "metadata/alarm.cfg"
GRAPHIC_METADATA = "metadata/graphics.cfg"


class Panel:

	def __init__(self):
		self.mount()

	def mount(self):
		for metadata in self.configuration:
			self.table[metadata['id']].metadata.update(**metadata)


class BoundariesPanel(Panel):

	configuration = load_config_file(BOUND_METADATA)
	table = {id: bound() for id, bound in BOUNDS.items()}

	def mount(self):
		super().mount()
		for bound in self.table.values():
			bound.limit = bound.metadata.default

	def bound(self, sensors):
		for bound in self.table.values():
			sensor_id = bound.metadata.bound_id 
			if sensor_id:
				sensors.table[sensor_id].constrains.append(bound)

	def update(self, data):
		updated_all = True
		for key, value in data.items():
			if key in self.table:
				self.table[key].limit = value
			else:
				updated_all = False
		return updated_all


class SensorsPanel(Panel):

	configuration = load_config_file(SENSOR_METADATA)
	table = {id: sensor() for id, sensor in SENSORS.items()}


class AlarmsPanel(Panel):

	configuration = load_config_file(ALARM_METADATA)
	table = {id: alarm() for id, alarm in ALARMS.items()}


class GraphicsPanel(Panel):

	configuration = load_config_file(GRAPHIC_METADATA)
	table = {id: graphic() for id, graphic in GRAPHICS.items()}
	
	def __init__(self, update_frequency):
		super().__init__()
		for graphic in self.table.values():
			graphic.update_frequency = update_frequency


if __name__ == "__main__":
	pass
