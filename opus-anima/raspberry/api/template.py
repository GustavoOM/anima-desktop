

class PatientTemplate:

    def __init__(self, data):
        self.id = data.get("id", None)
        self.age_group = data["age_group"]
        self.height = data.get("height", None)
        self.weight = data.get("weight", None)
        self.name = data.get("name", None)
        self.prontuary = data.get("prontuary", None)
        self.sex = data.get("sex", None)


class AlarmHistoryTemplate:

	def __init__(self, data, specs, alarm_specs):
		self.data = []
		for item in data:
			unit = specs.get(item["type"], None)
			if unit:
				unit = unit.metadata.unit
			else:
				unit = alarm_specs.get(item["type"], None)
				if unit:
					unit = unit.metadata.unit

			datetime = item["start_time"].strftime("%d/%m/%Y %H:%M")
			datetime = datetime.split(" ")
			time_elapsed = "ongoing"
			if item["end_time"]:
				time_elapsed = str(item["end_time"] - item["start_time"])
			self.data.append({"value": item["value"], "message": item["message"], "unit": unit, 
				"date": datetime[0], "time": datetime[1], "duration": time_elapsed, "id": item["id"]})

class AlarmInfoTemplate:

	def __init__(self, data, specs):
		self.data = []
		ignore = ["_sa_instance_state", "start_time", "patient_id", "id", "mode"]
		self.data.append({"name": "Modo", "value": data["mode"], "unit":""})		
		for id, value in data.items():
			if value != None and id not in ignore:
				self.data.append({"name": specs[id].name, "value": value, "unit": specs[id].unit})
