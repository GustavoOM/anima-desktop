from flask import Flask, jsonify, request
from flask_socketio import SocketIO
from flask_cors import CORS

from context import ContextManager
from controller import Controller
from checklist import Checklist
from template import AlarmHistoryTemplate, AlarmInfoTemplate
from base import (
	DEBUG_OUTPUT, DEBUG_ALARM, DEBUG_GRAPHIC, IP_ADDRESS, PORT
)
from multiprocessing.connection import Client
from os import system, kill, getpid
from signal import SIGINT

app = Flask(__name__)
CORS(app)
socketio = SocketIO(app, cors_allowed_origins="*")

manager = ContextManager()
controller = Controller(boundaries=manager.boundaries)
tester = Checklist()
writer_conn = None

@app.route("/modes", methods=['GET'])
def list_modes():
	"""
	Return a list of availabe ventilatory modes.
	"""
	return jsonify(manager.available_modes())


@app.route("/settings", methods=['GET'])
def retrieve_settings():
	"""
	Return every avaiable ventilator mode with configuration 
	parameters and output monitor parameters.
	"""
	modes = manager.info()
	output, graphics, alarms = controller.info()
	return jsonify({'modes': modes, 'output': output, 'graphics': graphics, 'alarms': alarms})


@app.route("/configure", methods=['POST'])
def configure():
	"""
	Set user's configuration. Send configuration to controller.
	"""
	mode_name = request.json['mode']
	configuration = request.json['configuration']
	#print("ENTREI NO /CONFIGURE!\n")
	if manager.change_mode(mode_name, configuration):
		#print("ENTREI NA CONDIÇÃO!\n")
		writer_conn.send({"type": 11, "data": manager.next_mode.parameters})
		configuration["mode"] = mode_name
		manager.update(configuration)
		return jsonify({"delivered": True})
	return jsonify({"delivered": False})


@app.route("/patient", methods=['POST'])
def add_patient():
	"""
	Set data of new pacient
	"""
	if manager.new_patient(request.json):
		for i in range(5):
			writer_conn.send({"type": 41, "data": manager.pid_parameters()[i]})
		writer_conn.send({"type": 43, "data": manager.control_parameters()[0]})
		return jsonify({"delivered": True})
	return jsonify({"delivered": False})


@app.route("/init-param", methods=['POST'])
def send_init_param():
	"""
	Send initial parameters to the controller
	"""
	return jsonify({"delivered": True})


@app.route("/boundaries", methods=['POST'])
def set_alarms():
	"""
	Set parameters bondaries to trigger alarms
	"""
	if manager.set_alarms(request.json['boundaries']):
		writer_conn.send({"type": 13, "data": request.json['boundaries']})
		return jsonify({"delivered": True})
	return jsonify({"delivered": False})


@app.route("/night-mode", methods=['POST'])
def set_night_mode():
	"""
	Toggle night mode which lowers the alarm volume
	"""
	controller.set_night_mode(request.json['nightmode'])
	return jsonify({"delivered": True})


@app.route("/silence", methods=['POST'])
def silence_alarm():
	"""
	Receive the id of an alarm and send a silence message to controller
	"""
	controller.set_silence(1)
	return jsonify({"delivered": True})


@app.route("/comm-alert", methods=['POST'])
def comm_alert():
	"""
	Notifies a communication undesired event
	"""
	if request.json["commActor"] == "writer":
		controller.notify_writer_fail()
	elif request.json["commActor"] == "reader":
		controller.notify_reader_idle()
	if controller.is_there_comm_error():
		controller.update({"communication": 1}, False)
		alert = controller.emit_alert()
		if alert:
			socketio.emit("alert", alert)
	return jsonify({"delivered": True})


@app.route("/stop", methods=['POST'])
def stop_controller():
	"""
	Stop mechanic respiration.
	"""
	if request.json["stop"]:
		writer_conn.send({"type": 15, "data": {"stop": 7}})

		controller.set_graphic_pause(True)
		controller.set_in_ventilation(False)

		return jsonify({"delivered": True})
	return jsonify({"delivered": False})


@app.route("/start", methods=['POST'])
def start_controller():
	"""
	Start mechanic respiration.
	"""
	if request.json["start"]:
		writer_conn.send({"type": 15, "data": {"stop": 0}})		
		controller.set_graphic_pause(False)
		controller.set_in_ventilation(True)
		controller.set_silence(0)
		return jsonify({"delivered": True})
	return jsonify({"delivered": False})


@app.route("/status", methods=['GET'])
def status():
	"""
	Answers if frontend should skip configuration and go to monitor or if it should
	set a new configuration.
	"""
	return jsonify(manager.restore())



@app.route("/test/reset", methods=['POST'])
def reset_autotests():
	"""
	Reset autotests workflow. 
	"""
	cycle = request.json["cycle"]
	tester.reset_cycle(cycle)
	return jsonify({"delivered": True})


@app.route("/test/start", methods=['POST'])
def start_autotest():
	
	"""
	Start a specific autotest. Send the test to be executed to controller. 
	"""
	writer_conn.send({"type": 17, "data": request.json})
	tester.start(request.json)
	return jsonify({"delivered": True})


@app.route("/test/audio-start", methods=['POST'])
def start_audio_autotest():
	
	"""
	Play the alarm audio to its autotest. 
	"""
	if request.json["audio"]:
		system("omxplayer -o local --vol -1000 api/resources/media_quadrada.wav > /dev/null")
	return jsonify({"delivered": True})


@app.route("/test/check-<group>", methods=['GET'])
def check_autotest(group):
	
	"""
	Check if an started autotest is currently running, finished corrected or exitted with failure.
	"""

	completed, failure, message = tester.check_progress(group)
	jsonRetorno = {"delivered": True, "id": group, "completed": completed, 
		"failure": failure, "message":message}
	#print("/test/check-<group>\n",jsonRetorno)
	return jsonify(jsonRetorno)


@app.route("/test/info", methods=['GET'])
def test_info():
	
	"""
	Get all the autotests available.
	"""
	return jsonify(tester.info())


@app.route("/test/report", methods=['POST'])
def report_autotest():
	
	"""
	Report the result of some autotest
	"""
	print(request.json)
	tester.check_response(request.json)
	return jsonify({"delivered": True})
	

@app.route("/turnoff", methods=['GET'])
def turnoff():
	"""
	Start the shutdown procedure.
	"""
	manager.shutdown_signal = True
	writer_conn.send({"type": 15, "data": {"turnOff": 7}})
	manager.close()
	kill(getpid(), SIGINT)
	return jsonify({"delivered": True})


@app.route("/log/list-<num_rows>")
def get_last_alarms(num_rows):
	"""
	Return a json structure with the data of the last <num_rows> alarms.
	"""
	raw_data = manager.logger.get_last_alarms(num_rows)
	temp = AlarmHistoryTemplate(raw_data, manager.boundaries.table, controller.alarms.table)
	return jsonify({"list": temp.data, "delivered": True})


@app.route("/log/info-<alarm_id>")
def get_alarm_info(alarm_id):
	"""
	Return the mode configuration that was executing when a given alarm started.
	"""
	raw_data = manager.logger.get_alarm_info(alarm_id)
	temp = AlarmInfoTemplate(raw_data, manager.guesser.data)
	return jsonify({"info": temp.data, "delivered": True})


@app.route("/ex-flow-calib/start", methods=['POST'])
def start_ex_flow_calib():
	"""
	Start the expiratory flow calibration procedure.
	"""
	if request.json["exFlowCalib"]:
		
		writer_conn.send({"type": 15, "data": {"exFlowCalib": 7}})
		controller.calibration.start()
		
		return jsonify({"delivered": True})
	return jsonify({"delivered": False})


@app.route("/ex-flow-calib/report", methods=['POST'])
def report_ex_flow_calib():
	"""
	Report progress of the expiratory flow calibration.
	"""
	controller.calibration.report(request.json)
	return jsonify({"delivered": True})


@app.route("/ex-flow-calib/check", methods=['GET'])
def check_ex_flow_calib():
	"""
	Check progress of the expiratory flow calibration.
	"""
	progress = controller.calibration.check()
	return jsonify({"delivered": True, "progress": progress})


@app.route("/output/graphics-alarms", methods=['POST'])
def report_graphics_alarms():
	"""
	Receive data from graphics and alarms, check those values and show them 
	to the operator
	"""
	data = request.json
	if controller.is_there_comm_error():
		data["communication"] = 1
	if manager.is_mode_set():
		controller.update(data, True)
		alert = controller.emit_alert()
		if alert:
			socketio.emit("alert", alert)
			if DEBUG_ALARM:
				print("ALERTS = {}".format(alert))
		if not controller.wait():
			drawing = controller.draw()
			socketio.emit("draw", drawing)
			if DEBUG_GRAPHIC:
				print("GRAPHICS = {}".format(drawing))
	return jsonify({"delivered": True})


@app.route("/output/indicators", methods=['POST'])
def report_indicators():
	"""
	Receive data from indicators, check those values and show them 
	to the operator
	"""
	if manager.is_mode_set():
		controller.update(request.json, False)
		if not controller.wait():
			output = controller.output()
			socketio.emit("report", output)
			if DEBUG_OUTPUT:
				print("OUTPUT = {}".format(output))
	should_emit, hourmeter = manager.logger.check_hourmeter()
	if should_emit:
		socketio.emit("update hourmeter", hourmeter)
	return jsonify({"delivered": True})


def start_api():
	"""
	Function to start api.
	"""
	global writer_conn
	writer_conn = Client(("localhost", 6000))
	writer_conn.send("api")
	manager.restore()
	socketio.run(app, host=IP_ADDRESS , port=PORT)
