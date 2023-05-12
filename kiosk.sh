#!/bin/bash

xset s noblank
xset s off
xset -dpms

echo $CONFIG_OPUS_ENV

if [ ! -f .setup ];
then
	touch .setup && echo "Running setup.sh" && sh tools/setup/setup.sh
fi

gnome-terminal --command="bash execArduino.sh"

sleep 10

cd raspberry
. animaenv/bin/activate
python3.8 stay_alive.py
