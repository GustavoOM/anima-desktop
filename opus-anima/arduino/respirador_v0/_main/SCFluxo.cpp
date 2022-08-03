#include "_global.h"


SCFluxo::SCFluxo() { }


void SCFluxo::setup() {
  memset(&paramPidFluxoAr, 0, sizeof(ParametrosPid));
  memset(&paramPidFluxoO2, 0, sizeof(ParametrosPid));
  memset(&estadoPidFluxoAr, 0, sizeof(EstadoPid));
  memset(&estadoPidFluxoO2, 0, sizeof(EstadoPid));

  _setDefaultParametrosPid(); // sobrescritos no caso de comunicação com a Rasp

  pidFluxoAr.setup(&paramPidFluxoAr, &estadoPidFluxoAr);
  pidFluxoO2.setup(&paramPidFluxoO2, &estadoPidFluxoO2); 
}


void SCFluxo::loop() {
	_calculaSetArO2();
  
  estadoPidFluxoAr.sensorAtual = medidas.fluxoAr;
  estadoPidFluxoO2.sensorAtual = medidas.fluxoO2;
  
  pidFluxoAr.loop();
	pidFluxoO2.loop();
  
  valvulaFluxoArVP1(round(estadoPidFluxoAr.saidaAtual));
  valvulaFluxoOxVP2(round(estadoPidFluxoO2.saidaAtual));
}


void SCFluxo::resetPid() {
  pidFluxoAr.resetEstadoPid();
  pidFluxoO2.resetEstadoPid();
}

/* Calcula setpoint de ar e de O2 a partir do setpoint de fluxo definido no módulo
superior e o FiO2 setado pelo usuário. */
void SCFluxo::_calculaSetArO2() {
  const float densidadeAr = 1.293;
  const float densidadeO2 = 1.429;

  float aux1 = (0.209 - paramCicloControle.fio2) * densidadeAr;
  float aux2 = (paramCicloControle.fio2 - 1.0) * densidadeO2  +  (0.209 - paramCicloControle.fio2) * densidadeAr;
  
  estadoPidFluxoAr.setpointAtual = (1.0 - aux1/aux2) * estadoControle.setFluxo;
  estadoPidFluxoO2.setpointAtual = (aux1/aux2) * estadoControle.setFluxo;
}


void SCFluxo::_setDefaultParametrosPid() {
#ifdef SIMULADOR
  paramPidFluxoAr.Kp = 17.8;
  paramPidFluxoAr.Ti = 0.029;
  paramPidFluxoAr.Td = 0.0;
  paramPidFluxoAr.N = 0.0;
  paramPidFluxoAr.Tt = 1.0;
  paramPidFluxoAr.I0 = 0.0;
  paramPidFluxoAr.minn = 0.0;
  paramPidFluxoAr.maxx = 4095.0;

  paramPidFluxoAr.pontosAutoKp[0] = 10.0;
  paramPidFluxoAr.ganhosAutoKp[0] = 1.0;
  paramPidFluxoAr.pontosAutoKp[1] = 20.0;
  paramPidFluxoAr.ganhosAutoKp[1] = 1.0;
  paramPidFluxoAr.pontosAutoKp[2] = 30.0;
  paramPidFluxoAr.ganhosAutoKp[2] = 1.0;
  paramPidFluxoAr.pontosAutoKp[3] = 40.0;
  paramPidFluxoAr.ganhosAutoKp[3] = 1.0;
  paramPidFluxoAr.pontosAutoKp[4] = 50.0;
  paramPidFluxoAr.ganhosAutoKp[4] = 1.0;
  
  paramPidFluxoO2.Kp = 17.8;
  paramPidFluxoO2.Ti = 0.029;
  paramPidFluxoO2.Td = 0.0;
  paramPidFluxoO2.N = 0.0;
  paramPidFluxoO2.Tt = 1.0;
  paramPidFluxoO2.I0 = 0.0;
  paramPidFluxoO2.minn = 0.0;
  paramPidFluxoO2.maxx = 4095.0;

  paramPidFluxoO2.pontosAutoKp[0] = 10.0;
  paramPidFluxoO2.ganhosAutoKp[0] = 1.0;
  paramPidFluxoO2.pontosAutoKp[1] = 20.0;
  paramPidFluxoO2.ganhosAutoKp[1] = 1.0;
  paramPidFluxoO2.pontosAutoKp[2] = 30.0;
  paramPidFluxoO2.ganhosAutoKp[2] = 1.0;
  paramPidFluxoO2.pontosAutoKp[3] = 40.0;
  paramPidFluxoO2.ganhosAutoKp[3] = 1.0;
  paramPidFluxoO2.pontosAutoKp[4] = 50.0;
  paramPidFluxoO2.ganhosAutoKp[4] = 1.0;

#else // MAQUINA
  paramPidFluxoAr.Kp = 2.5;
  paramPidFluxoAr.Ti = 0.004;
  paramPidFluxoAr.Td = 0.0;
  paramPidFluxoAr.N = 0.0;
  paramPidFluxoAr.Tt = 0.01;
  paramPidFluxoAr.I0 = 800.0;
  paramPidFluxoAr.minn = 800.0;
  paramPidFluxoAr.maxx = 3500.0;

  paramPidFluxoAr.pontosAutoKp[0] = 10.0;
  paramPidFluxoAr.ganhosAutoKp[0] = 1.0;
  paramPidFluxoAr.pontosAutoKp[1] = 20.0;
  paramPidFluxoAr.ganhosAutoKp[1] = 1.0;
  paramPidFluxoAr.pontosAutoKp[2] = 30.0;
  paramPidFluxoAr.ganhosAutoKp[2] = 1.0;
  paramPidFluxoAr.pontosAutoKp[3] = 40.0;
  paramPidFluxoAr.ganhosAutoKp[3] = 1.0;
  paramPidFluxoAr.pontosAutoKp[4] = 50.0;
  paramPidFluxoAr.ganhosAutoKp[4] = 1.0;

  paramPidFluxoO2.Kp = 8.0;
  paramPidFluxoO2.Ti = 0.01;
  paramPidFluxoO2.Td = 0.0;
  paramPidFluxoO2.N = 0.0;
  paramPidFluxoO2.Tt = 0.01;
  paramPidFluxoO2.I0 = 1500.0;
  paramPidFluxoO2.minn = 800.0;
  paramPidFluxoO2.maxx = 3500.0;

  paramPidFluxoO2.pontosAutoKp[0] = 10.0;
  paramPidFluxoO2.ganhosAutoKp[0] = 1.0;
  paramPidFluxoO2.pontosAutoKp[1] = 20.0;
  paramPidFluxoO2.ganhosAutoKp[1] = 1.0;
  paramPidFluxoO2.pontosAutoKp[2] = 30.0;
  paramPidFluxoO2.ganhosAutoKp[2] = 1.0;
  paramPidFluxoO2.pontosAutoKp[3] = 40.0;
  paramPidFluxoO2.ganhosAutoKp[3] = 1.0;
  paramPidFluxoO2.pontosAutoKp[4] = 50.0;
  paramPidFluxoO2.ganhosAutoKp[4] = 1.0;
         
#endif
}
