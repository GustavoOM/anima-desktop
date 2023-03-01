MAIN=_main.cpp
DEPENDENCE=_global.cpp Pid.cpp Sensores.cpp Alarmes.cpp Autotestes.cpp Controle.cpp EntradaRasp.cpp EstrategiaAC.cpp EstrategiaPSV.cpp EstrategiaTopLayer.cpp Filtro.cpp HistBuffer.cpp Metricas.cpp ModoPCV.cpp ModoVCV.cpp Respirador.cpp SaidaRasp.cpp SCFluxo.cpp SCPressaoExp.cpp SCPressaoIns.cpp SimuladorLogico.cpp Estrategia.cpp 
PROGRAMNAME=anima

all:
	clear && g++ ${DEPENDENCE} $(MAIN) -o $(PROGRAMNAME) -lm

run:
	clear && ./$(PROGRAMNAME)

dev:
	clear && g++ ${DEPENDENCE} $(MAIN) -o $(PROGRAMNAME) -lm && ./$(PROGRAMNAME)