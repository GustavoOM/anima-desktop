from base import VENTILATION_2CONTROL_MSG, BOUNDARIES_2CONTROL_MSG, INDICATORS_2API_MSG, ALARM_LABELS

def get_label(parameter_list):
	for p in parameter_list:
		yield p.label


class CheckStrategy(type):

	def __new__(meta, name, bases, class_dict):
		if bases:
			for label in class_dict['identifiers']:
				if label not in get_label(VENTILATION_2CONTROL_MSG.PARAMETERS):
					raise ValueError('Strategy {} not well defined. Parameter "{}" not expected.'.format(name, label))
		return type.__new__(meta, name, bases, class_dict)


class CheckSensor(type):

	def __new__(meta, name, bases, class_dict):
		if class_dict['id'] != "generic":
			if class_dict['id'] not in get_label(INDICATORS_2API_MSG.OUTPUT):
				raise ValueError('Measure {} not well defined. Identifier "{}" not expected.'.format(name, class_dict['id']))
		return type.__new__(meta, name, bases, class_dict)


class CheckAlarm(CheckSensor, type):

	def __new__(meta, name, bases, class_dict):
		if class_dict['id'] != "generic":
			if class_dict['id'] not in ALARM_LABELS:
				raise ValueError('Alarm {} not well defined. Identifier "{}" not expected.'.format(name, class_dict['id']))
		return type.__new__(meta, name, bases, class_dict)


class CheckBound(CheckAlarm, type):

	def __new__(meta, name, bases, class_dict):
		super().__new__(meta, name, bases, class_dict)
		if class_dict['id'] != "generic":
			if class_dict['id'] not in get_label(BOUNDARIES_2CONTROL_MSG.BOUNDARIES):
				raise ValueError('Bound {} not well defined. Identifier "{}" not expected.'.format(name, class_dict['id']))
		return type.__new__(meta, name, bases, class_dict)


class CheckNothing(CheckAlarm, type):

	def __new__(meta, name, bases, class_dict):
		return type.__new__(meta, name, bases, class_dict)
