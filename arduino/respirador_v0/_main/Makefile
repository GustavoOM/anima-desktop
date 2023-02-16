MAIN=_main.cpp
DEPENDENCE=_global.cpp Alarmes.cpp Autotestes.cpp Controle.cpp EntradaRasp.cpp Estrategia.cpp EstrategiaAC.cpp EstrategiaPSV.cpp EstrategiaTopLayer.cpp Filtro.cpp HistBuffer.cpp Metricas.cpp ModoPCV.cpp ModoVCV.cpp Respirador.cpp SaidaRasp.cpp SCFluxo.cpp SCPressaoExp.cpp SCPressaoIns.cpp Sensores.cpp SimuladorLogico.cpp 
PROGRAMNAME=anima

all:
	clear && g++ ${DEPENDENCE} $(MAIN) -o $(PROGRAMNAME) -lm

run:
	clear && ./$(PROGRAMNAME)

dev:
	clear && g++ ${DEPENDENCE} $(MAIN) -o $(PROGRAMNAME) -lm && ./$(PROGRAMNAME)