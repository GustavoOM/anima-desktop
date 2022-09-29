import datetime
from sqlalchemy import create_engine
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy import Column, Integer, String, Float, func, Boolean, DateTime, desc, ForeignKey
from sqlalchemy.orm import sessionmaker, relationship
from sqlalchemy.exc import IntegrityError
from contextlib import contextmanager
from base import DIR_PATH


engine = create_engine('sqlite:///{}/persist.db'.format(DIR_PATH), echo=False)
DB_Table = declarative_base()
Session = sessionmaker(bind=engine)


"""
TABLE OBJECTS
"""

class Mode(DB_Table):
	__tablename__ = 'mode'

	id = Column(Integer, primary_key=True)
	start_time = Column(DateTime, default=datetime.datetime.now)
	patient_id = Column(Integer)
	mode = Column(String)
	FIO2 = Column(Integer)
	shape = Column(Integer)
	riseTime = Column(Integer)
	tIns = Column(Integer)
	tInsMax = Column(Integer)
	Freq = Column(Integer)
	vTidal = Column(Integer)
	vMin = Column(Integer)
	flow = Column(Float)
	pCon = Column(Float)
	pSuport = Column(Float)
	tSup = Column(Float)
	pSup = Column(Float)
	tInf = Column(Float)
	pInf = Column(Float)
	PEEP = Column(Float)
	sensInsP = Column(Float)
	sensInpF = Column(Float)
	tPausaIns = Column(Integer)
	sensExpF = Column(Float)
	pInsLimit = Column(Float)
	bkMode = Column(Integer)
	bkTIns = Column(Integer)
	bkFreq = Column(Integer)
	bkVTidal = Column(Integer)
	bkPcon = Column(Float)

	def __repr__(self):
		return "<MODE=(" + "".join([" {}={}".format(key, value) for key, value in 
			self.__dict__.items() if value != None and key != '_sa_instance_state']) + ")>"


class Patient(DB_Table):
	__tablename__ = "patient"

	id = Column(Integer, primary_key=True)
	name = Column(String)
	sex = Column(String)
	height = Column(Integer)
	weight = Column(Integer)
	age_group = Column(String)
	prontuary = Column(String)


class Alarm(DB_Table):
	__tablename__ = "alarm"

	id = Column(Integer, primary_key=True)
	type = Column(String)
	mode_id = Column(Integer, ForeignKey("mode.id"))
	start_time = Column(DateTime, default=datetime.datetime.now)
	end_time = Column(DateTime, default=datetime.datetime.now)
	message = Column(String)
	value = Column(Float)
	num_silences = Column(Integer, default=0)
	state = Column(Integer, default=1)


class Boundaries(DB_Table):
	__tablename__ = "boundaries"

	id = Column(Integer, primary_key=True)
	mode_id = Column(Integer)
	start_time = Column(DateTime, default=datetime.datetime.now)
	notBreathing = Column(Float)
	fio2min = Column(Integer)
	fio2max = Column(Integer)
	vMinMin = Column(Integer)
	vMinMax = Column(Integer)
	pInsMin = Column(Float)
	pInsMax = Column(Float)
	pInsHardLimit = Column(Float)
	freqMax = Column(Integer)
	vTidalInsMin = Column(Integer)
	vTidalInsMax = Column(Integer)
	maxPEEP = Column(Float)


class State(DB_Table):
	__tablename__ = "state"

	id = Column(Integer, primary_key=True)
	shutdown = Column(Boolean)
	hourmeter = Column(Integer)
	strategy = Column(String)
	boundaries = Column(String)
	patient = Column(String)


"""
LOGGER CLASSES
"""

class StateLogger:
	def __init__(self):
		self.HOURMETER_UPDATE_FREQUENCY = 60	# in seconds
		self.HOURMETER_EMIT_FREQUENCY = 3600	# in seconds
		self.seconds_since_last_emit = self.get_hourmeter() % self.HOURMETER_EMIT_FREQUENCY
		self.updated_at = datetime.datetime.now()
		self.emited_at = datetime.datetime.now() - datetime.timedelta(seconds=self.seconds_since_last_emit)

	@contextmanager
	def session_scope(self):
		""" Isolates the scope handling from the rest of the cade. """
		session = Session()
		try:
			yield session
		except:
			session.rollback()
		finally:
			session.commit()
			session.close()

	def save_patient(self, patient):
		log_item = Patient()
		for key, value in patient.__dict__.items():
			log_item.__dict__[key] = value
		with self.session_scope() as session:
			session.add(log_item)
			session.commit()
			patient.id = log_item.id
			state = session.query(State).one()
			state.patient = str(patient.__dict__)
		return patient

	def save_mode(self, mode, patient_id):
		log_item = Mode(patient_id=patient_id)
		for key, value in mode.parameters.items():
			log_item.__dict__[key] = value
		mode_id = None
		with self.session_scope() as session:
			session.add(log_item)
			state = session.query(State).one()
			state.strategy = str(mode.parameters)
			state.shutdown = False
			session.commit()
			mode_id = log_item.id
		return mode_id

	def save_boundaries(self, boundaries, mode_id):
		log_item = Boundaries(mode_id=mode_id)
		for key, value in boundaries.items():
			log_item.__dict__[key] = value
		with self.session_scope() as session:
			session.add(log_item)
			state = session.query(State).one()
			state.boundaries = str(boundaries)

	def shutdown(self):
		with self.session_scope() as session:
			state = session.query(State).one()
			state.shutdown = True

	def restore(self):
		patient = None
		strategy = None
		boundaries = None
		shutdown = None
		hourmeter = None
		with self.session_scope() as session:
			self.interrupt_alarms(session)
			state = session.query(State).one()
			patient = eval(state.patient)
			strategy = eval(state.strategy)
			boundaries = eval(state.boundaries)
			shutdown = state.shutdown
			hourmeter = state.hourmeter // self.HOURMETER_EMIT_FREQUENCY # convert from seconds to hours
		return patient, strategy, boundaries, shutdown, hourmeter

	def interrupt_alarms(self, session):
		alarms = session.query(Alarm).filter(Alarm.state != 0).filter(Alarm.state != 3).all()
		end_time = datetime.datetime.now()
		for alarm in alarms:
			alarm.state = 3
			alarm.end_time = end_time

	def get_ids(self):
		mode_id = None
		patient_id = None
		with self.session_scope() as session:
			mode_id, patient_id, _ = session.query(Mode.id, Mode.patient_id, func.max(Mode.start_time)).one()
		return mode_id, patient_id

	def get_last_alarms(self, num_rows):
		data = None
		with self.session_scope() as session:
			data = [alarm.__dict__ for alarm in session.query(Alarm).order_by(desc(Alarm.start_time)).limit(num_rows).all()]
		return data

	def get_alarm_info(self, alarm_id):
		data = None
		with self.session_scope() as session:
			data = session.query(Mode).join(Alarm).filter(Alarm.id == int(alarm_id)).one().__dict__
		return data

	def get_hourmeter(self):
		hourmeter = None
		with self.session_scope() as session:
			state = session.query(State).one()
			hourmeter = state.hourmeter
		return hourmeter

	def update_hourmeter(self, seconds_passed):
		hourmeter = None
		with self.session_scope() as session:
			state = session.query(State).one()
			state.hourmeter += seconds_passed
			hourmeter = state.hourmeter
		return hourmeter
		

	def check_hourmeter(self):
		time_now = datetime.datetime.now()
		delta_updated = (time_now - self.updated_at).seconds
		delta_emited = (time_now - self.emited_at).seconds

		hourmeter = None
		should_emit = False

		if delta_updated >= self.HOURMETER_UPDATE_FREQUENCY:
			hourmeter = self.update_hourmeter(delta_updated)
			hourmeter =  hourmeter // self.HOURMETER_EMIT_FREQUENCY # convert from seconds to hours
			self.updated_at = datetime.datetime.now()

			if delta_emited >= self.HOURMETER_EMIT_FREQUENCY:
				should_emit = True
				self.emited_at = datetime.datetime.now()
		return should_emit, hourmeter
		
		
class AlarmLogger:

	def __init__(self):
		self._reset()

	def _open(self):
		self.session = Session()
		self.mode_id, _ = self.session.query(Mode.id, func.max(Mode.start_time)).one()
		self.current_alarms = {alarm.type: alarm for alarm in self.session.query(Alarm).filter(Alarm.state != 0).filter(Alarm.state != 3).all()}

	def _reset(self):
		self.mode_id = None
		self.current_alarms = {}
		self.session = None
		self.track = set()

	def save(self, alarm):

		if alarm.id not in self.track:
			return

		if alarm.state != None and alarm.id in self.current_alarms:
			record = self.current_alarms[alarm.id]
			if record.state != 0 and alarm.state == 0:
				record.end_time = datetime.datetime.now()
			elif record.state != 2 and alarm.state == 2:
				record.num_silences += 1
			record.state = alarm.state
			value = alarm.__dict__.get("watch_value", None)
			if value:
				record.value = value
		elif alarm.state == 1 or alarm.state == 2:
			self.session.add(Alarm(type=alarm.id, mode_id=self.mode_id, value=alarm.__dict__.get("watch_value", None),
					message=alarm.metadata.message, state=alarm.state))

		self.track.remove(alarm.id)
		if not self.track:
			self.session.commit()
			self.session.close()
			self._reset()

	def begin_track(self, alarm):
		if not self.session:
			self._open()
		self.track.add(alarm.id)


class FakeAlarmLogger:

	def begin_track(self, alarm):
		pass

	def save(self, alarm):
		pass

if __name__ == '__main__':
	logger = StateLogger()
	print(logger.restore())
