#!/bin/bash
cd /home/gustavo/Documents/opus-anima

APP_DIR=$PWD

# Check if raspberry has internet connection
ping -c 1 google.com
if [ $? -ne 0 ]
then
    echo "ERROR: Raspberry Pi is not connected to internet"
    exit 1
fi

sudo apt-get update

sudo apt-get install curl

# Install arduino-cli
cd /home/gustavo
sudo curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
PATH=$PATH:/home/gustavo/bin

# Setup arduino-cli
arduino-cli core update-index
arduino-cli core install arduino:sam

# Check if arduino is connected
ARDUINO_CHECK=$(arduino-cli board list | grep Arduino)
if [ -z "$ARDUINO_CHECK" ]
then
    echo "ERROR: Could not find any Arduino board connected."
    exit 1
fi

#Conexão com os jumpers
# Enable serial port and disable serial console
#if [ ! $(cat /boot/config.txt | grep enable_uart) ]
#then
#    echo enable_uart=1 | sudo tee -a /boot/config.txt
#fi
# já comentado antes # echo $(awk '{print substr($0,index($0,$2))}' /boot/cmdline.txt) | sudo tee /boot/cmdline.txt
#sudo sed -i 's/console=serial0,115200 //' /boot/cmdline.txt


# Compile and upload code to Arduino
cd $APP_DIR/arduino/respirador_v0/_main
# get serial port from board
PORT=$(arduino-cli board list | grep Arduino | awk '{print $1}')
# get FQBN from board
FQBN=$(arduino-cli board list | grep Arduino | awk '{print $(NF - 1)}')
# remove the BOM character from _global.cpp
sudo sed -i '1s/^\xEF\xBB\xBF//' _global.cpp
# compile and upload code
arduino-cli compile -b $FQBN
arduino-cli upload -p $PORT -b $FQBN

# Environment access key
KEY=$(date | md5sum | cut -d" " -f1)
echo "SECRET_KEY=$KEY" > environment
sudo mv environment /etc

cd $APP_DIR/raspberry/api
sudo chmod 666 persist.db








# Install dependencies, setup the virtual environment and database
cd $APP_DIR/setup
bash cleanup.sh

# Enable kiosk service
#cd $APP_DIR/setup
#sudo cp kiosk.service /lib/systemd/system/kiosk.service
#sudo systemctl enable kiosk.service

# Visual adjustments
#sudo cp panel /home/pi/.config/lxpanel/LXDE-pi/panels
# change wallpaper not working
#pcmanfm --set-wallpaper $APP_DIR/setup/wallpaper.png --wallpaper-mode center
#sudo mv /etc/xdg/lxsession/LXDE-pi/sshpwd.sh /etc/xdg/lxsession/LXDE-pi/sshpwd.sh.backup
#sudo mv /etc/xdg/autostart/piwiz.desktop /etc/xdg/autostart/piwiz.desktop.backup

# Environment access key
KEY=$(date | md5sum | cut -d" " -f1)
echo "SECRET_KEY=$KEY" > environment
sudo mv environment /etc

cd $APP_DIR/raspberry/api
sudo chmod 666 persist.db

sudo reboot
