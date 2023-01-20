import requests
from requests.exceptions import ConnectionError
from base import ( AUTOTESTS_2API_MSG, EXFLOW_CALIB_2API_MSG, FLUXSENS_MSG,
    PRESSENS_MSG, EXAVALV_MSG, AOPVALVS_MSG, DECODERS, API_URL, ser, SHOW_READ_SERIAL,
    VENTILATOR_MODE, GRAPHICS_ALARMS_2API_MSG, INDICATORS_2API_MSG, RESPONSE_2API_MSG,
    SHOW_READ_SERIAL_TIMING, checksum_calc )
from time import time
from multiprocessing.connection import Client
import socket
import sys
portSocket = 8084

def forward_message(data):
    if SHOW_READ_SERIAL:
            print("[READER] Message received: ", data)
    data = data.split(',')
    if data[0] == "10" and len(RESPONSE_2API_MSG) == len(data):
        msg = decode_message(data, RESPONSE_2API_MSG)
        writer_conn.send(msg)
    if data[0] == "20" and len(GRAPHICS_ALARMS_2API_MSG) == len(data):
        decode_post_message(data, GRAPHICS_ALARMS_2API_MSG, "/output/graphics-alarms")
    if data[0] == "22" and len(INDICATORS_2API_MSG) == len(data):
        decode_post_message(data, INDICATORS_2API_MSG, "/output/indicators")
    if data[0] == "24" and len(AUTOTESTS_2API_MSG) == len(data):
        decode_post_message(data, AUTOTESTS_2API_MSG, "/test/report")
    if data[0] == "26" and len(EXFLOW_CALIB_2API_MSG) == len(data):
        decode_post_message(data, EXFLOW_CALIB_2API_MSG, "/ex-flow-calib/report")

def decode_message(data, schema):
    message = {}
    for i, item in enumerate(schema):
        if data[i] != '':
            try:
                if item.convert in DECODERS:
                    message.update(DECODERS[item.convert](data[i]))
                else:
                    message[item.label] = item.convert(data[i])
            except:
                print("[READER] Could not convert {} field. Ignoring field.".format(item.label))
                continue
    return message

def decode_post_message(data, schema, endpoint):
    it = time() * 1000
    message = decode_message(data, schema)
    et = time() * 1000
    if SHOW_READ_SERIAL_TIMING:
        print("[READER] Decoding msg id[" + str(message["MessageType"]) + "]: " + "{:.0f}".format(et - it))
    try:
        if VENTILATOR_MODE:
            it = time() * 1000
            requests.post(API_URL + endpoint, json=message)
            et = time() * 1000
            if SHOW_READ_SERIAL_TIMING:
                print("[READER] Posting msg id[" + str(message["MessageType"]) + "]: " + "{:.0f}".format(et - it))
        
    except ConnectionError:
        print("[READER] Could not reach API. Dropping Message.")

def notify_idle():
    try:
        requests.post(API_URL + "/comm-alert", json={"commActor": "reader"})
    except ConnectionError:
        print("[READER] Could not reach API. Dropping Message.")
    if SHOW_READ_SERIAL:
        print("[READER] Idle for a long time")

def read_serial():
    global writer_conn
    writer_conn = Client(("localhost", 6000))
    writer_conn.send("reader")

    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind(('127.0.0.1', portSocket))

    print("TENTANDO ME CONECTAR!!!!!!")
    server_socket.listen(1)
    conn,address = server_socket.accept()
    print ("o cliente = ", address, " se conectou")

    idlt = time() * 1000
    try:
        while True:
            it = time() * 1000
            #TCP AQUI
            while True:
                msg_raw = conn.recv(1024).decode()
                if msg_raw:
                    break

            if len(msg_raw) > 5:
                if SHOW_READ_SERIAL_TIMING:
                    print("[READER] Idle: " + "{:.0f}".format(it - idlt))
                if msg_raw is not None:
                    forward_message(msg_raw)
                    et = time() * 1000
                    if SHOW_READ_SERIAL_TIMING:
                        print("[READER] Total msg id[" + msg_raw[0:2] + "]: " + "{:.0f}".format(et - it))
                        idlt = et
            else:
                notify_idle()

    except KeyboardInterrupt:
        print ('KeyboardInterrupt exception is caught')

    finally:
        writer_conn.close()
        conn.close()

if __name__ == '__main__':
    read_serial()