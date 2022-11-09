#!/bin/bash

RANGE=$((8100-8000+1))
R=$(($(($RANDOM%$RANGE))+8000))
echo $R

readonly SERVER_SCRIPT="python3 servidor.py $R"; # O script a ser executado
readonly CLIENT_SCRIPT="python3 cliente.py $R"; # O script a ser executado

gnome-terminal -- bash -c "$SERVER_SCRIPT; exec $SHELL";

gnome-terminal -- bash -c "$CLIENT_SCRIPT; exec $SHELL";