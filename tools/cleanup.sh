#!/bin/bash

cd /home/$USER/Documents/opus-anima/raspberry

# Install database (remove if already exists)
cd api
sudo rm persist.db
sudo apt install sqlite3
sqlite3 persist.db < initialize_db.sql
cd ..

