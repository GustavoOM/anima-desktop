#!/bin/bash

xset s noblank
xset s off
xset -dpms

cd /home/$USER/Documents/opus-anima/raspberry
python3.8 stay_alive.py
