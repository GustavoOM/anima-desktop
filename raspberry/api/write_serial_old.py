from time import time, sleep
from multiprocessing.connection import Listener
from base import (
    OTHERPARAM_2CONTROL_MSG, ser, VENTILATOR_MODE, SHOW_WRITE_SERIAL, MessageTime,
    PID_2CONTROL_MSG, FLUXSENS_MSG, EXAVALV_MSG, AOPVALVS_MSG, PRESSENS_MSG, VENTILATION_2CONTROL_MSG,
    BOUNDARIES_2CONTROL_MSG, OPERATION_2CONTROL_MSG, AUTOTESTS_2CONTROL_MSG, checksum_calc,
    API_URL
)
import requests
from requests.exceptions import ConnectionError

class Writer:
    def __init__(self):
        self.api_conn = None
        self.reader_conn = None
        self.listener = Listener(("localhost", 6000))
        self.wait_queue = []

    def setup_socket_server(self):
        while self.api_conn is None or self.reader_conn is None:
            conn = self.listener.accept()
            msg = conn.recv()
            if msg == "api":
                self.api_conn = conn
                if SHOW_WRITE_SERIAL:
                    print("[WRITER] Connected to the API")
            elif msg == "reader":
                self.reader_conn = conn
                if SHOW_WRITE_SERIAL:
                    print("[WRITER] Connected to the Reader")
            else:
                conn.close()

    def loop(self):
        try:
            while True:
                if len(self.wait_queue) > 0:
                    self.manage_wait_queue()
                is_there_data = self.api_conn.poll(timeout=0.1)
                if is_there_data:
                    self.transmit_message()
                is_there_data = self.reader_conn.poll(timeout=0.01)
                while is_there_data:
                    self.check_response()
                    is_there_data = self.reader_conn.poll(timeout=0.01)
        finally:
            ser.close()
            self.listener.close()

    def manage_wait_queue(self):
        for item in self.wait_queue:
            if time() - item["tSent"] > 1:
                item["nTimout"] += 1
                item["tSent"] = time()
                if item["nTimout"] < 5:
                    print("[WRITER] Timeout warning, sending again:", item)
                    for _ in range(3):
                        send_message(item["msg"])
                        sleep(0.05)
                else:
                    self.wait_queue.remove(item)
                    try:
                        requests.post(API_URL + "/comm-alert", json={"commActor": "writer"})
                    except ConnectionError:
                        print("[WRITER] Could not reach API. Dropping Message.")
                    print("[WRITER] TIMEOUT ERROR:", item)

    def transmit_message(self):
        msg_obj = self.api_conn.recv()
        schema = self.get_schema_from_type(msg_obj["type"])
        if schema is not None:
            msg, id = build_message(msg_obj["data"], schema, msg_obj["type"])
            if SHOW_WRITE_SERIAL:
                print("[WRITER] Message sent:", msg)
            self.wait_queue.append({"id": id, "tSent": time(), "nTimout": 0, "msg": msg})
            for _ in range(3):
                send_message(msg)
                sleep(0.05)

    def check_response(self):
        response = self.reader_conn.recv()
        #if SHOW_WRITE_SERIAL:
            #print("[WRITER] Response received from the reader", response)
        if len(self.wait_queue) > 0:
            for item in self.wait_queue:
                if response["idMsgRecv"] == item["id"]:
                    self.wait_queue.remove(item)
                    break

    def get_schema_from_type(self, type):
        if type == 11:
            return VENTILATION_2CONTROL_MSG
        elif type == 13:
            return BOUNDARIES_2CONTROL_MSG
        elif type == 15:
            return OPERATION_2CONTROL_MSG
        elif type == 17:
            return AUTOTESTS_2CONTROL_MSG
        elif type == 31:
            return PRESSENS_MSG
        elif type == 33:
            return FLUXSENS_MSG
        elif type == 35:
            return EXAVALV_MSG
        elif type == 37:
            return AOPVALVS_MSG
        elif type == 41:
            return PID_2CONTROL_MSG
        elif type == 43:
            return OTHERPARAM_2CONTROL_MSG
        else:
            return None


def serial_send(data, schema, type):
    msg, _ = build_message(data, schema, type)
    for _ in range(3):
        send_message(msg)
        sleep(0.05)

def send_message(message):
    if VENTILATOR_MODE:
        #TCP AQUI
        ser.write(message)

def build_message(data, schema, type):
    id = MessageTime.get()
    message = '^' + str(type) + ',1,' + str(id)
    for item in schema:
        value = data.get(item.label, '')
        if value != '':
            value = str(item.convert(value))
        message += ',' + value
    checksum = checksum_calc(message[:-1])
    message += str(checksum) + ';'
    return message.encode('ascii'), id

def terminal_send_text(text):
    message = "^00,1," + text + ";"
    ser.write(bytes(message, "utf-8"))

def write_serial():
    writer = Writer()
    writer.setup_socket_server()
    writer.loop()


if __name__ == '__main__':
    write_serial()