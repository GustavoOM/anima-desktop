#include "_global.h"
#include <malloc.h>
#include <stdlib.h>

//#define VERBOSE

void setupRespirador() {
  #ifdef SIMULADOR
    setupSimulacao();
  #else // MAQUINA
    setupMaquina();
  #endif
  
  #ifdef ARDUINO_PLOTTER
    setupGraficoRespirador();
  #endif
  
  respirador.setup();
}

void loopRespirador() {
#ifdef SIMULADOR
  if (respirador.contIter == 2) { sim.loop(false); }
#endif

  respirador.loop();

#ifdef SIMULADOR
  if (respirador.contIter == 2) { sim.loop(true); }
#endif

#ifdef ARDUINO_PLOTTER
  if (respirador.contIter == 3) { loopGrafico(); }
#endif
}

void setupSimulacao() {
  sim.setup();
  statusSensores.valorPressao[1] = 35;
  statusSensores.valorPressao[2] = 35;
  statusSensores.tensaoBateria = 13;
  statusSensores.tensaoFonte = 13;
}

void setupGraficoRespirador() {
  memset(&grafico, 0, sizeof(ModoGrafico));

  grafico.curvasAtuais[0] = 10;
  grafico.curvasAtuais[1] = 11;

  grafico.pidRef = &estadoPidFluxoAr;
}
