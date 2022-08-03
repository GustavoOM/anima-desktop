from write_serial import serial_send
from guesser import PressensParameters, FluxsensParameters, ExavalvParameters, AopvalvsParameters
from time import sleep
from base import PRESSENS_MSG, FLUXSENS_MSG, EXAVALV_MSG, AOPVALVS_MSG

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
