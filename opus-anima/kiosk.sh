#!/bin/bash

xset s noblank
xset s off
xset -dpms

cd ~/Documents/opus-anima/raspberry
source animaenv/bin/activate
python3 stay_alive.py
