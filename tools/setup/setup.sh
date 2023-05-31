#!/bin/bash

# Setup directory

APP_DIR=$PWD
###
sudo apt-get update
sudo apt install chromium-browser -y

# Install python 3.8
sudo apt install software-properties-common -y
sudo add-apt-repository ppa:deadsnakes/ppa -y
sudo apt install python3.8 -y

# Create virtual env
cd raspberry
python3.8 -m pip install virtualenv
sudo apt install python3.8-venv -y
python3.8 -m venv animaenv
. animaenv/bin/activate
python3.8 -m pip install -r requirements.txt

# Install dependencies, setup the virtual environment and database
# Install database (remove if already exists)
cd api
sudo rm persist.db
sudo apt install sqlite3
sqlite3 persist.db < initialize_db.sql
cd ..

# Environment access key
KEY=$(date | md5sum | cut -d" " -f1)
echo "SECRET_KEY=$KEY" > environment
sudo mv environment /etc

cd $APP_DIR/raspberry/api
sudo chmod 666 persist.db

echo "setup.sh complete"
sudo reboot