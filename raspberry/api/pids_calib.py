from base import (
    load_config_file, SerialMessage, save_config_file, OTHERPARAM_2CONTROL_MSG,
    PID_2CONTROL_MSG, PRESSENS_MSG, FLUXSENS_MSG, EXAVALV_MSG, AOPVALVS_MSG
)
from time import sleep
from write_serial import serial_send
from sqlalchemy import Table, Column, String, MetaData, update, select
from sqlalchemy import create_engine
from os import path, system
from re import match
from guesser import PressensParameters, FluxsensParameters, ExavalvParameters, AopvalvsParameters

ABS_CONFIG_PATH = "/home/pi/Documents/opus-anima/raspberry/api/config/"
STEP2CONTROL = SerialMessage(load_config_file("schema/step_2control.cfg"))

PIDS = ["ar", "o2", "ins", "pil", "exp"]
SETS = PIDS + ["cont", "deg"]

SCHEMA = {}
SCHEMA["cont"] = [item.label for item in OTHERPARAM_2CONTROL_MSG.PARAMETERS]
SCHEMA["deg"] = [item.label for item in STEP2CONTROL.PARAMETERS]
for pid in PIDS:
    SCHEMA[pid] = [item.label for item in PID_2CONTROL_MSG.PARAMETERS]

engine = create_engine('sqlite:///shaping.db', echo=False)
db = engine.connect()

metadata = MetaData()
state = Table('state', metadata,
    Column('id', String(10), primary_key=True),
    Column('value', String(1000))
)
metadata.create_all(engine)
intial_state = select([state])


def initialize_state():
    for sel in SETS:
        ins = state.insert().values(id=sel, value=str(dict()))
        db.execute(ins)


def set_state(parameters, sel):
    ins_pid = state.update().where(state.c.id == sel).values(value=str(parameters[sel]))
    db.execute(ins_pid)


def get_state(parameters):
    sel = select([state])
    for row in db.execute(sel):
        dict_values = eval(row[1])
        if dict_values and isinstance(dict_values, dict):
            parameters[row[0]] = dict_values
    clean_data(parameters)


def process_command(raw):
    itemized = [item.strip() for item in raw.split(" ") if item not in ['', ' ', '\n']]
    length = len(itemized)
    if length == 1:
        return itemized[0]
    elif length % 2 == 0:
        values = {}
        for i in range(0, length, 2):
            values[itemized[i][1:]] = itemized[i+1]
        return values
    return None


def save_data(parameters):
    global PAC_TYPE
    get_state(parameters)
    pids_data = [parameters[pid] for pid in PIDS]
    control_data = [parameters["cont"]]
    save_config(pids_data, "pid_" + PAC_TYPE + ".cfg")
    save_config(control_data, "general_" + PAC_TYPE + ".cfg")
    sync_twin_file(parameters)
    print("Salvo como arquivo de configuracao!")


def sync_twin_file(parameters):
    global MACHINE_ID
    global PAC_TYPE

    twin_pac_type = ""
    if PAC_TYPE == "adult":
        twin_pac_type = "child"
    else:
        twin_pac_type = "adult"
    twin_pids_data = []
    twin_pids_data.append(parameters["ar"])
    twin_pids_data.append(parameters["o2"])
    pid_ins = {}
    pid_exp = {}
    twin_file_path = "calibration/" + MACHINE_ID + "/pid_" + twin_pac_type + ".cfg"
    if not path.exists(ABS_CONFIG_PATH + twin_file_path):
        orig_file_path = "calibration/" + MACHINE_ID + "/pid_" + PAC_TYPE + ".cfg"
        system("cp " + ABS_CONFIG_PATH + orig_file_path + " " + ABS_CONFIG_PATH + twin_file_path)

    for item in load_config_file(twin_file_path):
        if item["pid"] == "ins":
            pid_ins = item
        if item["pid"] == "exp":
            pid_exp = item
    twin_pids_data.append(pid_ins)
    twin_pids_data.append(parameters["pil"])
    twin_pids_data.append(pid_exp)
    save_config(twin_pids_data, "pid_" + twin_pac_type + ".cfg")


def save_config(data, filename):
    global MACHINE_ID
    filepath = "calibration/" + MACHINE_ID + "/" + filename
    save_config_file(data, filepath)
    system("cp " + ABS_CONFIG_PATH + filepath + " " + ABS_CONFIG_PATH + "calibration/current/" + filename)


def load_data(parameters):
    global PAC_TYPE
    for item in load_config_file("calibration/current/pid_" + PAC_TYPE + ".cfg"):
        parameters[item["pid"]] = item
    parameters["deg"] = {"valv": "ar", "periodo": 1000, "valor1": 4, "valor2": 20}
    parameters["cont"] = load_config_file("calibration/current/general_" + PAC_TYPE + ".cfg")[0]
    clean_data(parameters)


def clean_data(parameters):
    for sel, data in parameters.items():
        for key in data:
            if key not in SCHEMA[sel]:
                del data[key]

def exit():
    raise KeyboardInterrupt


def send_last(parameters, sel):
    if sel == "deg":
        serial_send(parameters[sel], STEP2CONTROL, 45)
    elif sel == "cont":
        serial_send(parameters[sel], OTHERPARAM_2CONTROL_MSG, 43)
    else:
        serial_send(parameters[sel], PID_2CONTROL_MSG, 41)


def update(parameters, sel, command):
    selected = parameters[sel]
    updated = False
    for key, value in command.items():
        if key in SCHEMA[sel]:
            selected[key] = value
            updated = True
    if updated:
        set_state(parameters, sel)
    return updated


def check_command(command):
    for key, value in command.items():
        if not is_command_valid(key, value):
            return False
    return True


def is_command_valid(key, value):
    main_pid_params = ["Kp", "Ti", "Td", "N", "Tt"]
    pwm_based = ["I0", "lo", "hi"]
    autokp_point_based = ["pa0", "pa1", "pa2", "pa3", "pa4", "valor1", "valor2"]
    low_gain_based = ["ga0", "ga1", "ga2", "ga3", "ga4", "Kr", "minCCVolume", "maxCCVolume", "fatorCCVolume", "minCCPressao", "maxCCPressao", "fatorCCPressao"]
    boolean = ["autoKpPorDelta"]
    delta_estrang = ["deltaMin", "deltaMax"]
    time = ["periodo"]
    pid = ["valv"]
    # The user must not be able to change the pid parameter from a pid parameter set

    try:
        value = float(value)
        if key in main_pid_params:
            return 0 <= value and value <= 120
        elif key in pwm_based:
            return 0 <= value and value <= 4095
        elif key in autokp_point_based:
            return 0 <= value and value <= 100
        elif key in low_gain_based:
            return 0 <= value and value <= 3
        elif key in boolean:
            return value == 0 or value == 1
        elif key in delta_estrang:
            return 0 <= value and value <= 10
        elif key in time:
            return 0 <= value and value <= 1000
    except:
        if key in pid:
            return value in PIDS
    return False


def start(parameters):
    for sel in SETS:
        send_last(parameters, sel)
        sleep(0.2)


def help():
    print("COMANDOS")
    print()
    print("-sel <conj>")
    print("    seleciona um conjunto de parametros")
    print("    em que <conj> pode ser:")
    print("        ar    -> PID de fluxo de ar")
    print("        o2    -> PID de fluxo de o2")
    print("        ins   -> PID de pressao inspiratoria")
    print("                 (no PCV)")
    print("        pil   -> PID de pressao piloto")
    print("        exp   -> PID de pressao expiratoria")
    print("        cont  -> outros parametros do")
    print("                 controlador")
    print("        deg   -> degrau de calibracao dos")
    print("                 PIDs ar/O2/piloto")
    print()
    print("-<par> <val>")
    print("    atribui um valor a um parametro do")
    print("    conjunto selecionado")
    print("-salv")
    print("    salva os parametros configurados")
    print("-exib")
    print("    mostra os parametros do conjunto")
    print("    selecionado")
    print("-sair")
    print("    sai do programa")


def init(parameters):
    global MACHINE_ID
    global PAC_TYPE

    print()
    print("===================================")
    print("LINHA DE COMANDO PARA ALTERACAO")
    print("DE PARAMETROS DOS PIDS")
    print("-----------------------------------")
    print()
    print()
    ser_num = ""
    while True:
        ser_num = input("Insira o numero de serie da maquina: \n")
        if match("[A-Z0-9]+", ser_num):
            print("Numero de serie: " + ser_num)
            command = input("[insira 1 para confirmar]\n")
            if command == "1":
                MACHINE_ID = ser_num
                break
        else:
            print("[erro] Insira apenas letras maisculas e numeros!")
    print()

    tipo = ""
    while True:
        print("Insira o tipo de paciente:")
        print("1 - Adulto")
        print("2 - Infantil")
        tipo = input()
        if tipo == "1":
            tipo = "adulto"
            PAC_TYPE = "adult"
            break
        elif tipo == "2":
            tipo = "infantil"
            PAC_TYPE = "child"
            break
        else:
            print("[erro] Insira apenas 1 ou 2!")

    while True:
        print()
        print("Garanta que o programa ja esta")
        print("carregado no Atmel!")
        confirma = input("Confirme inserindo 1\n")
        if confirma == "1":
            break

    while True:
        print()
        print("Garanta que o Plotter da Arduino IDE")
        print("esta aberto!")
        confirma = input("Confirme inserindo 1\n")
        if confirma == "1":
            break

    print("-----------------------------------")
    print("Iniciando...")
    print("Numero de serie " + ser_num)
    print("Tipo de paciente " + tipo)
    print()

    sleep(0.5)
    serial_send(PressensParameters().get()[0], PRESSENS_MSG, 31)
    sleep(0.5)
    for i in range(3):
       serial_send(FluxsensParameters().get()[i], FLUXSENS_MSG, 33)
       sleep(0.5)
    serial_send(ExavalvParameters().get()[0], EXAVALV_MSG, 35)
    sleep(0.5)
    serial_send(AopvalvsParameters().get()[0], AOPVALVS_MSG, 37)
    sleep(0.5)

    calib_path = "calibration/" + MACHINE_ID
    if not path.exists(ABS_CONFIG_PATH + calib_path):
        system("mkdir " + ABS_CONFIG_PATH + calib_path)
    load_data(parameters)
    start(parameters)

    print()
    print("-----------------------------------")
    print()
    print()
    help()

def main():
    if not db.execute(intial_state).fetchone():
        initialize_state()

    parameters = {}
    init(parameters)

    sel = ''
    try:
        while True:
            raw_command = input("SEL[{}]>>".format(sel))
            command = process_command(raw_command)
            if str(command)[:6] == "-salv":
                save_data(parameters)
            elif command == "-exib":
                if sel in SETS:
                    print(parameters[sel])
                else:
                    print("[erro] Nenhum conjunto de parametros selecionado!")
            elif command == "-sair":
                exit()
            else:
                try:
                    if command and "sel" in command:
                        if command["sel"] in SETS:
                            sel = command["sel"]
                        else:
                            print("[erro] Conjunto de parametros invalido!")
                    elif check_command(command):
                        if update(parameters, sel, command):
                            print("ok")
                            send_last(parameters, sel)
                        else:
                            print("[erro] Parametro nao encontrado no conjunto selecionado!")
                    else:
                        print("[erro] Comando invalido!")
                except (AttributeError, KeyError):
                    if sel not in SETS:
                        print("[erro] Nenhum conjunto de parametros selecionado!")
                    print("[erro] Parametro nao existente no conjunto selecionado!")
    except KeyboardInterrupt:
        print("Saindo...")


if __name__ == "__main__":
    main()
