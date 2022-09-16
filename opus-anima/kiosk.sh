#!/bin/bash

xset s noblank
xset s off
xset -dpms

echo $CONFIG_OPUS_ENV

if [ ! -f ~/Documents/opus-anima/.setup ];
then
	touch .setup && echo "Running setup.sh" && sh tools/setup/setup.sh
fi


cd ~/Documents/opus-anima/raspberry
. animaenv/bin/activate
python3.8 stay_alive.py
