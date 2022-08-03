#include "_global.h"


SCPressaoIns::SCPressaoIns() { }


void SCPressaoIns::setup() {
  memset(&paramPidPressaoIns, 0, sizeof(ParametrosPid));
  memset(&estadoPidPressaoIns, 0, sizeof(EstadoPid));

  _setDefaultParametrosPid();
  pidPressaoIns.setup(&paramPidPressaoIns, &estadoPidPressaoIns);
}


float SCPressaoIns::loop() {
	// recebe setpoint de pressão inspiratória definido no módulo superior
  estadoPidPressaoIns.setpointAtual = estadoControle.setPressaoIns;
  float fatorSensorFluxo = paramAjusteControle.Kr * medidas.fluxoIns;
  estadoPidPressaoIns.sensorAtual = medidas.pressaoSis + fatorSensorFluxo;
  // fator proporcional ao fluxo funciona como um segundo derivativo para o PID

  if (_isInicioInspiracao()) { _ajustePid(); }  
  
  pidPressaoIns.loop();
  return estadoPidPressaoIns.saidaAtual;
}


void SCPressaoIns::resetPid() {
  pidPressaoIns.resetEstadoPid();
}


bool SCPressaoIns::_isInicioInspiracao() {
  return (estadoControle.faseAtual == EM_INSPIRACAO) &&
          (estadoControle.faseAnterior == EM_EXPIRACAO);
}


void SCPressaoIns::_ajustePid() {
  // para saída do PID (que é o setpoint de fluxo no PCV) não começar zerado
  paramPidPressaoIns.I0 = paramCicloControle.fluxoBase;
  pidPressaoIns.moverIntegral();
  
  // medida da pressão no fim da expiração (PEEP) para ser usada como referência para o
  // ajuste de autoKp por delta (o outro termo do delta é a pressão de controle do PCV)
  estadoPidPressaoIns.refDelta = medidas.pressaoSis;
}


void SCPressaoIns::_setDefaultParametrosPid() {
#ifdef SIMULADOR
  paramPidPressaoIns.Kp = 4.0;
  paramPidPressaoIns.Ti = 0.08;
  paramPidPressaoIns.Td = 0.0;
  paramPidPressaoIns.N = 0.0;
  paramPidPressaoIns.Tt = 0.01;
  paramPidPressaoIns.I0 = 0.0;
  paramPidPressaoIns.minn = 0.0;
  paramPidPressaoIns.maxx = 150.0;

  paramPidPressaoIns.pontosAutoKp[0] = 10.0;
  paramPidPressaoIns.ganhosAutoKp[0] = 1.0;
  paramPidPressaoIns.pontosAutoKp[1] = 20.0;
  paramPidPressaoIns.ganhosAutoKp[1] = 1.0;
  paramPidPressaoIns.pontosAutoKp[2] = 30.0;
  paramPidPressaoIns.ganhosAutoKp[2] = 1.0;
  paramPidPressaoIns.pontosAutoKp[3] = 40.0;
  paramPidPressaoIns.ganhosAutoKp[3] = 1.0;
  paramPidPressaoIns.pontosAutoKp[4] = 50.0;
  paramPidPressaoIns.ganhosAutoKp[4] = 1.0;
  
#else // MAQUINA
  paramPidPressaoIns.Kp = 0.8;
  paramPidPressaoIns.Ti = 0.08;
  paramPidPressaoIns.Td = 0.0;
  paramPidPressaoIns.N = 0.0;
  paramPidPressaoIns.Tt = 0.01;
  paramPidPressaoIns.I0 = 0.0;
  paramPidPressaoIns.minn = 0.0;
  paramPidPressaoIns.maxx = 150.0;

  paramPidPressaoIns.pontosAutoKp[0] = 10.0;
  paramPidPressaoIns.ganhosAutoKp[0] = 1.0;
  paramPidPressaoIns.pontosAutoKp[1] = 20.0;
  paramPidPressaoIns.ganhosAutoKp[1] = 1.0;
  paramPidPressaoIns.pontosAutoKp[2] = 30.0;
  paramPidPressaoIns.ganhosAutoKp[2] = 1.0;
  paramPidPressaoIns.pontosAutoKp[3] = 40.0;
  paramPidPressaoIns.ganhosAutoKp[3] = 1.0;
  paramPidPressaoIns.pontosAutoKp[4] = 50.0;
  paramPidPressaoIns.ganhosAutoKp[4] = 1.0;
  
#endif
}
