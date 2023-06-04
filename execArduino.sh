# TODO: colocar esses comandos no setup, ja que faz parte da instalacao

cd arduino/respirador_v0/_main/

if [ ! -f anima ];
then
	make all
fi

clear && ./anima