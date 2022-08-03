#!/bin/bash

bash upbuffer.sh
if [ "$?" -ne "0" ]
then
	echo "[ERROR] arduino buffer update could not be done"
	exit 1
fi

cd /home/pi/Documents/opus-anima/raspberry
sudo rm -r animaenv
cd api
sudo rm persist.db

cd ..
sudo apt-get install python3-venv
python3 -m venv animaenv
source animaenv/bin/activate
pip install -r requirements.txt

sudo apt install sqlite3=3.27.2-3+deb10u1 -y
cd api
sqlite3 persist.db < initialize_db.sql
