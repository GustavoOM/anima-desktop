#!/bin/bash

xset s noblank
xset s off
xset -dpms

sudo apt-get install python3-pip
cd /home/gustavo/Documents/opus-anima/raspberry

sudo apt install libjpeg-dev zlib1g-dev -y

pip install -r requirements.txt


cd /home/gustavo/Documents/opus-anima/raspberry
python3 stay_alive.py
