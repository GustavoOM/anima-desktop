#!/bin/bash

# Setup directory

APP_DIR=$PWD
###
sudo apt-get update
sudo apt-get install curl
sudo apt install chromium-browser

# Install python 3.8
sudo apt install software-properties-common
sudo add-apt-repository ppa:deadsnakes/ppa -y
sudo apt install python3.8

# Create virtual env
cd raspberry
python3.8 -m pip install virtualenv
sudo apt install python3.8-venv
python3.8 -m venv animaenv
. animaenv/bin/activate
python3.8 -m pip install -r requirements.txt

# Install arduino-cli
cd /home/$USER
sudo usermod -a -G dialout $USER
sudo curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
PATH=$PATH:/home/$USER/bin


# Setup arduino-cli
sudo /home/$USER/bin/arduino-cli core update-index
sudo /home/$USER/bin/arduino-cli core install arduino:sam

# Check if arduino is connected
ARDUINO_CHECK=$(arduino-cli board list | grep Arduino)
if [ -z "$ARDUINO_CHECK" ]
then
    echo "ERROR: Could not find any Arduino board connected."
    exit 1
fi

# Compile and upload code to Arduino
cd $APP_DIR/arduino/respirador_v0/_main
# get serial port from board
PORT=$(sudo /home/$USER/bin/arduino-cli board list | grep Arduino | awk '{print $1}')
# get FQBN from board
FQBN=$(sudo /home/$USER/bin/arduino-cli board list | grep Arduino | awk '{print $(NF - 1)}')
# remove the BOM character from _global.cpp
sudo sed -i '1s/^\xEF\xBB\xBF//' _global.cpp
# compile and upload code
sudo /home/$USER/bin/arduino-cli compile -b $FQBN
sudo /home/$USER/bin/arduino-cli upload -p $PORT -b $FQBN

# Install dependencies, setup the virtual environment and database
cd $APP_DIR/tools
bash cleanup.sh

# Environment access key
KEY=$(date | md5sum | cut -d" " -f1)
echo "SECRET_KEY=$KEY" > environment
sudo mv environment /etc

cd $APP_DIR/raspberry/api
sudo chmod 666 persist.db

echo "setup.sh complete"
%sudo reboot
