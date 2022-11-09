from base import load_config_file
from time import time

AUTOTESTS_LABELS = "other/autotests_labels.cfg"

class Checklist():

	labels = load_config_file(AUTOTESTS_LABELS)

	def __init__(self):
		self.grouped_tests = {}
		self.activated_groups = {}
		self.group_values = {}
		for item in self.labels:
			self.group_values[item["value"]] = item["group"]
			self.grouped_tests[item["group"]] = {"completed": False}
			self.activated_groups[item["group"]] = False
		self.comm_test_init = 0

	def info(self):
		return {"tests": [{"id": item["group"], "description": item["description"], "cycle": item["cycle"], "value": item["value"]} for item in self.labels]}

	def start(self, data):
		test_group = self.group_values[data["test"]]
		self.activated_groups[test_group] = True
		if test_group == "communication":
			self.comm_test_init = time()

	def reset_cycle(self, cycle):
		for item in self.labels:
			if item["cycle"] == cycle:
				self.grouped_tests[item["group"]] = {"completed": False}
				self.activated_groups[item["group"]] = False

	def check_response(self, data):
		test_group = self.group_values[data["test"]]
		if self.activated_groups[test_group]:
			self.grouped_tests[test_group]["completed"] = True
			self.grouped_tests[test_group]["failure"] = data["fail"]
			self.activated_groups[test_group] = False

	def check_progress(self, group):

		not_implemented = []
		completed = True
		failure = False
		message = ''
		if group in not_implemented:
			return completed, failure, message
		completed = self.grouped_tests[group]["completed"]
		if completed:
			failure = self.grouped_tests[group]["failure"]
		return completed, failure, message

