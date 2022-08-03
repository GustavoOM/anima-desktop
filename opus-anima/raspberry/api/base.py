from collections import namedtuple
from math import floor
import serial
import os

"""
ENABLERS
"""

SHOW_READ_SERIAL = False
SHOW_WRITE_SERIAL = False
VENTILATOR_MODE = True
DEBUG_ALARM = False
DEBUG_OUTPUT = False
DEBUG_GRAPHIC = False
SHOW_READ_SERIAL_TIMING = False

"""
OTHERS
"""

DIR_PATH = os.path.dirname(os.path.realpath(__file__))


"""
LOAD CONFIGURATION FILE
"""

BASE_CONVERTIONS = {} 
DECODERS = {}

class FDict:

    def __init__(self, std_dict):
        self.standard = std_dict

    def __call__(self, x):
        return self.standard[x]


class IntegerDecoder:

    def __init__(self, config):
        self.convert = {item["bit"]: item["label"] for item in config}

    def __call__(self, data):
        alarms = {}
        stringify = '{:0{}b}'.format(int(data), len(self.convert))
        reverse_data = stringify[::-1]
        for i, bit in enumerate(reverse_data):
            if i in self.convert:
                alarms[self.convert[i]] = int(bit)
        return alarms


def try_convert(value):
    precedence = [int, float, eval, str]
    for convert in precedence:
        try:
            convertion = convert(value)
            if isinstance(convertion, str) and convertion in BASE_CONVERTIONS:
                convertion = BASE_CONVERTIONS[convertion]
            return convertion
        except (ValueError, NameError, SyntaxError):
            continue
    print("Could not convert {} to any of the possible values.".format(value))
    raise ValueError


def load_config_file(filename):
    fp = open(DIR_PATH + '/config/' + filename, 'r')
    lines = fp.read().split('\n')
    data = []
    try:
        labels = [item.strip() for item in lines[0].split(';')]
        for line in lines[1:]:
            line_array = line.split(";")
            if len(line_array) == len(labels):
                data.append({labels[i]: try_convert(line_array[i].strip()) for i in range(len(labels))})
    except BaseException as error:
        print("File {} is not in the correct format.".format(filename))
        raise error
    finally:
        fp.close()
        return data


def load_vert_config_file(filename):
    fp = open(DIR_PATH + '/config/' + filename, 'r')
    lines = fp.read().split('\n')
    if lines[len(lines)-1] == "":
        lines = lines[:len(lines)-1]
    first_line_array = lines[0].split(';')
    ndata = len(first_line_array) - 1
    if first_line_array[len(first_line_array)-1] == "":
        ndata = ndata - 1
    data = [{} for _ in range(ndata)]
    try:
        for line in lines:
            line_array = line.split(';')
            if line_array[len(line_array)-1] == "":
                line_array = line_array[:len(line_array)-1]
            ndata_in_line = len(line_array) - 1
            if ndata_in_line < ndata:
                for _ in range(ndata - ndata_in_line):
                    data.pop(ndata-1)
                    ndata = ndata - 1
                    if ndata == 0:
                        return []
            label = line_array[0].strip()
            for i in range(ndata):
                data[i][label] = try_convert(line_array[i+1].strip())
    except BaseException as error:
        print("File {} is not in the correct format.".format(filename))
        raise error
    finally:
        fp.close()
        return data


def save_config_file(data, filename):
    with open(DIR_PATH + '/config/' + filename, 'w+') as file:
        try:
            labels = list(data[0].keys())
            file.write(';'.join(labels) + '\n')
            for line in data:
                if len(line) == len(labels):    
                    file.write(';'.join([str(line[key]) for key in labels]) +'\n')
                else: 
                    raise KeyError
        except (KeyError, AttributeError):
            print("Data not in the correct format to be saved as {} config file.".format(filename))


def save_vert_config_file(data, filename):
    with open(DIR_PATH + '/config/' + filename, 'w+') as file:
        try:
            labels = list(data[0].keys())
            for label in labels:
                line = label + ";"
                for i in range(len(data)):
                    line = line + str(data[i][label]) + ";"
                file.write(line + '\n')
        except (AttributeError):
            print("Data not in the correct format to be saved as {} config file.".format(filename))


def load_convertions():
    config = load_config_file("other/convert_measures.cfg")
    try:
        for item in config:
            function = item["convert"]
            BASE_CONVERTIONS[item["id"]] = function
            if isinstance(function, dict):
                BASE_CONVERTIONS[item["id"]] = FDict(function)
    except (ValueError, NameError, SyntaxError) as error:
         print("Could not convert {} to a function.".format(item.id))
         raise error

load_convertions()
_ALARMS = load_config_file("other/alarm_signal_decoding.cfg")
DECODERS["int2alarms"] = IntegerDecoder(_ALARMS[:32])
ALARM_LABELS = [item["label"] for item in _ALARMS]


"""
Useful methods
"""

def get_all_subclasses(cls):
    """
    Extracted from https://stackoverflow.com/questions/3862310/how-to-find-all-the-subclasses-of-a-class-given-its-name/3862957
    """
    all_subclasses = []

    for subclass in cls.__subclasses__():
        all_subclasses.append(subclass)
        all_subclasses.extend(get_all_subclasses(subclass))

    return all_subclasses


"""
LOAD CONTROLLER MESSAGE FORMAT 
"""

Item = namedtuple("item", "label convert")

class SerialMessage:

    def __init__(self, file, header=False):
        self.include_header = header
        self.order = []
        self.mount(file)

    def mount(self, file):
        for item in file:
            if item["group"] not in self.order:
                self.order.append(item["group"])
            self.__dict__.setdefault(item["group"], []).append(Item(label=item["label"], 
                convert=item["convert"]))

    def __iter__(self):
        include = self.order
        if not self.include_header:
            include = self.order[1:]
        for group in include:  
            for item in self.__dict__[group]:
                yield item

    def __len__(self):
        total = 0
        for item in self:
            total += 1
        return total

VENTILATION_2CONTROL_MSG = SerialMessage(load_config_file("schema/ventilation_2control.cfg"))
BOUNDARIES_2CONTROL_MSG = SerialMessage(load_config_file("schema/boundaries_2control.cfg"))
OPERATION_2CONTROL_MSG = SerialMessage(load_config_file("schema/operation_2control.cfg"))
AUTOTESTS_2CONTROL_MSG = SerialMessage(load_config_file("schema/autotests_2control.cfg"))
PID_2CONTROL_MSG = SerialMessage(load_config_file("schema/pid_2control.cfg"))
OTHERPARAM_2CONTROL_MSG = SerialMessage(load_config_file("schema/otherparam_2control.cfg"))

RESPONSE_2API_MSG = SerialMessage(load_config_file("schema/response_2api.cfg"), header=True)
GRAPHICS_ALARMS_2API_MSG = SerialMessage(load_config_file("schema/graphics_alarms_2api.cfg"), header=True)
INDICATORS_2API_MSG = SerialMessage(load_config_file("schema/indicators_2api.cfg"), header=True)
AUTOTESTS_2API_MSG = SerialMessage(load_config_file("schema/autotests_2api.cfg"), header=True)
EXFLOW_CALIB_2API_MSG = SerialMessage(load_config_file("schema/exflow_calib_2api.cfg"), header=True)

PRESSENS_MSG = SerialMessage(load_config_file("schema/pressens.cfg"))
FLUXSENS_MSG = SerialMessage(load_config_file("schema/fluxsens.cfg"))
EXAVALV_MSG = SerialMessage(load_config_file("schema/exavalv.cfg"))
AOPVALVS_MSG = SerialMessage(load_config_file("schema/aopvalvs.cfg"))


"""
URL of API
"""

IP_ADDRESS = '0.0.0.0'
PORT = '5000'
API_URL = 'http://' + IP_ADDRESS + ':' + PORT 


ser = None
if VENTILATOR_MODE:
    ser = serial.Serial("/dev/serial0", baudrate=115200, timeout=2)


"""
Time Handlers
"""

class MessageTime:

    counter = 1

    @classmethod
    def get(cls):
        cls.counter += 1
        return cls.counter


"""
SERIAL MESSAGE SIZE CHECKS
"""
if __name__ == "__main__":
	print(br_timestamp())


"""
CHECKSUM CALCULATION
"""

def checksum_calc(msg):
    msg_bytes = msg.encode("ascii")
    sum = 0
    block = 0
    for i in range(len(msg_bytes)):
        j = i % 4
        if j == 0:
            block = 0
        block = block | msg_bytes[i] << 8*(3-j)
        if j == 3:
            sum = sum + block
        if i == (len(msg_bytes) - 1) and j != 3:
            block = block >> 8*(3-j)
            sum = sum + block
    return sum & int("0xFFFFFFFF", 16)
