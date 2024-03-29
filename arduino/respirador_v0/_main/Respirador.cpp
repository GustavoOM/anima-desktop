#include "_global.h"

//#define VERBOSE

Respirador::Respirador() {}

void Respirador::setup() {
  memset(&dadosTempo, 0, sizeof(DadosTemporizacao));
  _deveCalibrarFluxoEx = false;
  _deveLoopar = true;

  estrategiaTop.setup();
  metricas.setup();
  alarmes.setup();
  entradaRasp.setup();
  saidaRasp.setup();
  controle.setup();
  //entradaRasp.recebeParametrosInicializacao();
  autotestes.realizaAutotestes();
}

void Respirador::loop() {

  _tInicioIteracao = micros();
  if (contIter == 1) { _tInicioCiclo = micros(); }

  #ifndef SIMULADOR
    sensores.leSensores();
  #endif
    
  switch (contIter) {
    case 1:
      entradaRasp.loop();
      _verificaOperacao();
      estrategiaTop.loop();
      break;

    case 2:
      controle.loop();
      break;

    case 3:
      metricas.loop();
      alarmes.loop();
      break;
    
    case 4:
      saidaRasp.loop();
      break;
  } 

  dadosTempo.tUtilIter[contIter-1] = micros() - _tInicioIteracao;
  if (contIter == NUM_ITER) { dadosTempo.tUtilCiclo = micros() - _tInicioCiclo; }

  while (!passouTempoMicros(_tInicioIteracao, TAMS*1000.0));
  dadosTempo.tTotalIter[contIter-1] = micros() - _tInicioIteracao;
  if (contIter == NUM_ITER) { dadosTempo.tTotalCiclo = micros() - _tInicioCiclo; }

  contIter++;
  if (contIter == NUM_ITER + 1) { contIter = 1; }

}

void Respirador::_verificaOperacao() {
  ComandosOperacao *comandos = &entradaRasp.dadosRecebidosLoop.operacao;

  if (comandos->stop == 7) {
    estrategiaTop.notifyStopVentilacao(false);
  } else {
    estrategiaTop.notifyRestartVentilacao();
  }

  if (comandos->desliga == 7) {
    estrategiaTop.notifyStopVentilacao(false);
    comandos->desliga = 0;
  }
}

void Respirador::setLoop(bool deveLoopar) {
  _deveLoopar = deveLoopar;
}
