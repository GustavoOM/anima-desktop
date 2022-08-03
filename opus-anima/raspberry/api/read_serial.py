import requests
from requests.exceptions import ConnectionError
from base import ( AUTOTESTS_2API_MSG, EXFLOW_CALIB_2API_MSG, FLUXSENS_MSG,
    PRESSENS_MSG, EXAVALV_MSG, AOPVALVS_MSG, DECODERS, API_URL, ser, SHOW_READ_SERIAL,
    VENTILATOR_MODE, GRAPHICS_ALARMS_2API_MSG, INDICATORS_2API_MSG, RESPONSE_2API_MSG,
    SHOW_READ_SERIAL_TIMING, checksum_calc )
from time import time
from multiprocessing.connection import Client

def process_message(msg_raw):
    try:
        it = time() * 1000
        msg = msg_raw.decode("ascii")
        et = time() * 1000
        if msg is None:
            return None
        if msg[0] == "^" and msg[-3] == ';':
            if verify_checksum(msg):
                if SHOW_READ_SERIAL_TIMING:
                    print("[READER] Decoding msg id[" + msg[1:3] + "]: " + "{:.0f}".format(et - it))
                return msg[1:-3]
            else:
                print("[READER] Message discarded by invalid checksum", msg[-2])
                return None
    except UnicodeDecodeError:
        print("[READER] Could not decode into an ascii character. Dropping message:", msg_raw)

def verify_checksum(msg):
    chunks = msg.split(',')
    try:
        recv_checksum_str = (chunks.pop(-1))[:-3]
        recv_checksum = int(recv_checksum_str)
    except:
        print("[READER] Could not convert received checksum to int", recv_checksum_str)
        return False
    real_msg = ','.join(chunks)
    calc_checksum = checksum_calc(real_msg)
    return recv_checksum == calc_checksum

def forward_message(data):
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
        if SHOW_READ_SERIAL:
            print("[READER] Message received: ", message)
    except ConnectionError:
        print("[READER] Could not reach API. Dropping Message.")

def terminal_receive_text():
    while ser.in_waiting <= 0:
        pass
    msg = ser.readline().decode("utf-8")
    if msg[:3] == "^00":
        return msg[6:-3].split(',')[1]
    else:
        return None

def terminal_receive_data():
    while ser.in_waiting <= 0:
        pass
    msg = ser.readline().decode("utf-8")
    if msg[:2] == "^3":
        schema = None
        datatype = None
        if msg[2] == "2":
            schema = PRESSENS_MSG
            datatype = "pressens"
        elif msg[2] == "4":
            schema = FLUXSENS_MSG
            datatype = "fluxsens"
        elif msg[2] == "6":
            schema = EXAVALV_MSG
            datatype = "exavalv"
        elif msg[2] == "8":
            schema = AOPVALVS_MSG
            datatype = "aopvalvs"
        data = (msg[1:-3].split(','))[3:]
        if schema is not None:
            return datatype, decode_message(data, schema)

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

    idlt = time() * 1000
    try:
        while True:
            it = time() * 1000
            msg_raw = ser.readline()
            if len(msg_raw) > 5:
                if SHOW_READ_SERIAL_TIMING:
                    print("[READER] Idle: " + "{:.0f}".format(it - idlt))
                if ser.in_waiting > 200:
                    ser.reset_input_buffer()
                    print("[READER] Serial buffer flushed")
                msg = process_message(msg_raw)
                if msg is not None:
                    forward_message(msg)
                    et = time() * 1000
                    if SHOW_READ_SERIAL_TIMING:
                        print("[READER] Total msg id[" + msg[0:2] + "]: " + "{:.0f}".format(et - it))
                        idlt = et
            else:
                notify_idle()
    finally:
        ser.close()
        writer_conn.close()

if __name__ == '__main__':
    read_serial()
