#!/bin/bash

xset s noblank
xset s off
xset -dpms

echo $CONFIG_OPUS_ENV

if [ ! -f .setup ];
then
	touch .setup && echo "Running setup.sh" && sh tools/setup/setup.sh
fi


cd raspberry
. animaenv/bin/activate
python3.8 stay_alive.py
