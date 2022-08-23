#!/bin/bash

cd /home/gustavo/Documents/opus-anima/raspberry

cd api
sudo rm persist.db
cd ..


sudo apt install sqlite3
cd api
sqlite3 persist.db < initialize_db.sql
