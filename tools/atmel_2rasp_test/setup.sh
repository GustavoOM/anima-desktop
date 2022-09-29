PORT=$(sudo /home/$USER/bin/arduino-cli board list | grep Arduino | awk '{print $1}')
# get FQBN from board
FQBN=$(sudo /home/$USER/bin/arduino-cli board list | grep Arduino | awk '{print $(NF - 1)}')

# compile and upload code
sudo /home/$USER/bin/arduino-cli compile -b $FQBN
sudo /home/$USER/bin/arduino-cli upload -p $PORT -b $FQBN
