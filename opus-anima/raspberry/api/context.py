from template import PatientTemplate
from logger import StateLogger
from guesser import (
    ParameterGuesser, BoundariesPanel, PidParameters,
    ControlParameters, FluxsensParameters, ExavalvParameters,
    AopvalvsParameters, PressensParameters
)
from strategies import MODES


class ContextManager():

    def __init__(self):
        self.current_mode = None
        self.current_mode_id = None
        self.next_mode = None
        self.logger = StateLogger()
        self.guesser = ParameterGuesser()
        self.boundaries = BoundariesPanel()
        self.pid = PidParameters()
        self.control = ControlParameters()
        self.patient = None
        self.shutdown_signal = False
        self.pressens = PressensParameters()
        self.fluxsens = FluxsensParameters()
        self.exavalv = ExavalvParameters()
        self.aopvalvs = AopvalvsParameters()

    @staticmethod
    def info():
        return [mode.info() for mode in MODES.values()]

    @staticmethod
    def available_modes():
    	return {"modes": [{"label": label, "id": mode.__name__} for label, mode in MODES.items()]}

    def new_patient(self, data):
        patient = None
        try:
            patient = PatientTemplate(data)
            self.patient = self.logger.save_patient(patient)
        except:
            print("Bad data for patient.")
            return False
        self.guesser.guess(self.patient)
        self.current_mode = None
        self.next_mode = None
        return self.patient.id != None

    def pid_parameters(self):
        return self.pid.guess(self.patient)

    def control_parameters(self):
        return self.control.guess(self.patient)

    def pressens_parameters(self):
        return self.pressens.get()

    def fluxsens_parameters(self):
        return self.fluxsens.get()

    def exavalv_parameters(self):
        return self.exavalv.get()

    def aopvalvs_parameters(self):
        return self.aopvalvs.get()

    def set_alarms(self, data):
        updated = self.boundaries.update(data)
        if updated:
            self.logger.save_boundaries(data, self.current_mode_id)
        return updated

    def change_mode(self, mode, config_info):
        if not self.next_mode or self.next_mode.nickname != mode:
            self.next_mode = MODES[mode]()
            if self.current_mode:
                self.next_mode.configure(self.current_mode.parameters)
        self.next_mode.configure(config_info)
        if self.next_mode.is_valid():
            return True
        return False

    def update(self, controler_data):
        if not self.next_mode and not self.current_mode:
            return False

        if self.next_mode and self.next_mode.owns(controler_data):
            self.current_mode = self.next_mode
            self.current_mode_id = self.logger.save_mode(self.current_mode, self.patient.id)
            self.next_mode = None
        return self.current_mode_id != None

    def is_mode_set(self):
        return self.current_mode_id != None

    def restore(self):
        patient, strategy, boundaries, shutdown, hourmeter = self.logger.restore()
        print("API correctly shut down? R: {}".format(shutdown))
        if shutdown:
            return {"restored": False, "hourmeter": hourmeter}
        mode_id, patient_id = self.logger.get_ids()
        patient["id"] = patient_id
        self.patient = PatientTemplate(patient)
        self.guesser.guess(self.patient)
        current_mode = MODES[strategy["mode"]]
        current_mode.parameters = strategy
        self.current_mode = current_mode
        self.current_mode_id = mode_id
        self.boundaries.update(boundaries)
        return {"restored": True, "patient": patient, "strategy": strategy, "boundaries": boundaries, "hourmeter": hourmeter}

    def close(self):
        self.current_mode = None
        self.next_mode = None
        self.logger.shutdown()


if __name__ == '__main__':
    # manager = ContextManager()
    # data = {'height': 178, 'name': 'andre', 'age_group': 'adult'}
    # print(manager.new_patient(data))
    # print(manager.available_modes())
    # last_mode = manager.load_last_mode()
    # print(last_mode.parameters)
    # print(last_mode.is_valid())
    print(ContextManager.info())
