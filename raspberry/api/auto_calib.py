from read_serial import terminal_receive_text, terminal_receive_data
from write_serial import terminal_send_text
from base import ser, save_vert_config_file
from re import match
from os import system, path

ABS_CONFIG_PATH = "/home/pi/Documents/opus-anima/raspberry/api/config/"

def wait_confirm_command(instruction):
    command = ""
    while command != "1":
        print(instruction)
        command = input("[insira 1 para confirmar]\n")

if __name__ == "__main__":
    print()
    print()
    print("====================================================================")
    print("CALIBRACAO AUTOMATICA")
    print("--------------------------------------------------------------------")
    print()

    global MACHINE_ID
    ser_num = ""
    while True:
        ser_num = input("Insira o numero de serie da maquina: \n")
        if match("[A-Z0-9]+", ser_num):
            print()
            print("Numero de serie: " + ser_num)
            command = input("[insira 1 para confirmar]\n")
            if command == "1":
                MACHINE_ID = ser_num
                break
        else:
            print("[erro] Insira apenas letras maisculas e numeros!")
    calib_path = "calibration/" + MACHINE_ID
    if not path.exists(ABS_CONFIG_PATH + calib_path):
        system("mkdir " + ABS_CONFIG_PATH + calib_path)

    wait_confirm_command("Carregue o codigo no Atmel")
    wait_confirm_command("Conecte diretamente a inalacao do respirador na sua exalacao")
    wait_confirm_command("Alimente a maquina com ar e O2")
    print()

    try:
        response = ""
        while response != "!(inicia)!":
            terminal_send_text("!(inicia)!")
            response = terminal_receive_text()
            if response is None:
                print("ERRO inicializacao")
                exit()

        while True:
            text_output = terminal_receive_text()
            if text_output is not None:
                if text_output == "!(entrada)!":
                    text_input = input()
                    terminal_send_text(text_input)
                elif text_output == "!(finaliza)!":
                    break
                else:
                    print(text_output)
            else:
                print("ERRO loop")
                exit()

        dataset = {}
        for _ in range(6):
            datatype, data = terminal_receive_data()
            if datatype not in dataset:
                dataset[datatype] = []
            data.pop("CHKSUM", None)
            dataset[datatype].append(data)

        invalid_param = False
        param_set = dataset["pressens"][0]
        for param in param_set:
            if param_set[param] < -80000 or 80000 < param_set[param]:
                print("Parametro invalido", param)
                invalid_param = True
        for i in range(3):
            param_set = dataset["fluxsens"][i]
            for param in param_set:
                if "flux" in param:
                    if param_set[param] < 0 or 200 < param_set[param]:
                        print("Parametro invalido", param)
                        invalid_param = True
                else:
                    if param_set[param] < -80000 or 80000 < param_set[param]:
                        print("Parametro invalido", param)
                        invalid_param = True
        param_set = dataset["exavalv"][0]
        for param in param_set:
            if param_set[param] < 0 or 150 < param_set[param]:
                print("Parametro invalido", param)
                invalid_param = True
        param_set = dataset["aopvalvs"][0]
        for param in param_set:
            if "Pwm" in param:
                if param_set[param] < 0 or 4095 < param_set[param]:
                    print("Parametro invalido", param)
                    invalid_param = True
            elif "Out" in param:
                if param_set[param] < 0 or 200 < param_set[param]:
                    print("Parametro invalido", param)
                    invalid_param = True
            else:
                if param_set[param] < 0 or 1000 < param_set[param]:
                    print("Parametro invalido", param)
                    invalid_param = True

        if invalid_param:
            print("********************************************************************")
            print("ERRO PARAMETROS INVALIDOS")
            print("********************************************************************")
            ser.close()
            exit()

        for datatype in dataset.keys():
            file_path = "calibration/" + MACHINE_ID + "/" + datatype + ".cfg"
            save_vert_config_file(dataset[datatype], file_path)
            system("cp " + ABS_CONFIG_PATH + file_path + " " + ABS_CONFIG_PATH + "calibration/current/" + datatype + ".cfg")

        print("====================================================================")
        print("Dados de calibracao salvos como arquivos de configuracao")
        print("====================================================================")
        ser.close()
        exit()

    finally:
        ser.close()
