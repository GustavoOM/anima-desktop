#include "_global.h"


SCPressaoExp::SCPressaoExp() { }


void SCPressaoExp::setup() {
  memset(&paramPidPressaoExp, 0, sizeof(ParametrosPid)); 
  memset(&paramPidPressaoPil, 0, sizeof(ParametrosPid));
  memset(&estadoPidPressaoExp, 0, sizeof(EstadoPid)); 
  memset(&estadoPidPressaoPil, 0, sizeof(EstadoPid));

  _setDefaultParametrosPid(); // sobrescritos no caso de comunicação com a Rasp

  pidPressaoExp.setup(&paramPidPressaoExp, &estadoPidPressaoExp);
  pidPressaoPil.setup(&paramPidPressaoPil, &estadoPidPressaoPil);
}


void SCPressaoExp::loop() {
  if (!estadoControle.isSensorPilotoFalho) {
    // recebe setpoint de pressão expiratória definido no módulo superior
    estadoPidPressaoExp.setpointAtual = estadoControle.setPressaoExp;

    estadoPidPressaoExp.sensorAtual = medidas.pressaoSis;
    estadoPidPressaoPil.sensorAtual = medidas.pressaoPil;
  }
  else {
    estadoPidPressaoExp.setpointAtual = 0;
    estadoPidPressaoExp.sensorAtual = 0;
    estadoPidPressaoPil.sensorAtual = medidas.pressaoSis;
  }

#ifdef SIMULADOR
  // a simulação não considerava um conjunto de duas válvulas para a exalação, por isso
  // possui um comportamento diferente
  if (estadoControle.faseAtual == EM_EXPIRACAO) {
    pidPressaoExp.loop();
  } else {
    pidPressaoExp.moverIntegral();
    estadoPidPressaoExp.saidaAtual = 4095.0;
  }
  valvulaOxExalacaoVP3(round(estadoPidPressaoExp.saidaAtual));
#else // MAQUINA

  if (!estadoControle.isSensorPilotoFalho) {

    if (_isTransicaoFase()) {
      if (estadoControle.faseAtual != EM_INSPIRACAO) {
        _ajusteEstrangulamento();
      } else {
        paramPidPressaoExp.minn = 0.0;
        paramPidPressaoExp.maxx = 100.0;
        paramPidPressaoExp.I0 = 70.0;
        pidPressaoExp.moverIntegral();
      }  
    }
    pidPressaoExp.loop();
    estadoPidPressaoPil.setpointAtual = estadoPidPressaoExp.saidaAtual;
    
  }
  else {
    if (_isTransicaoFase()) {
      if (estadoControle.faseAtual != EM_INSPIRACAO) {
        float faixaPwm = paramPidPressaoPil.maxx - paramPidPressaoPil.minn;
        paramPidPressaoPil.I0 = 0.5 * faixaPwm + paramPidPressaoPil.minn;   
      } else {
        paramPidPressaoPil.I0 = paramPidPressaoPil.maxx;
      }  
      pidPressaoPil.moverIntegral();
    }
    
    estadoPidPressaoPil.setpointAtual = estadoControle.setPressaoExp;
  }

  pidPressaoPil.loop();
  valvulaOxExalacaoVP3(round(estadoPidPressaoPil.saidaAtual));

#endif
}


void SCPressaoExp::resetPid() {
  pidPressaoExp.resetEstadoPid();
  pidPressaoPil.resetEstadoPid();
}

/* Verifica transição ins/exp ou exp/ins. */
bool SCPressaoExp::_isTransicaoFase() {
  return (estadoControle.faseAtual != estadoControle.faseAnterior) &&
          (estadoControle.faseAtual != EM_PAUSA_INSPIRACAO);
          // nesse caso, a pausa inspiratória é considerada parte da inspiração
}


/* Define o valor de integral inicial para o PID de pressão expiratória de acordo com a
curva pressão piloto/sistema e define os valores de mínimo e máximo desse PID de modo a
estrangular o setpoint de pressão piloto dentro dessa faixa. */
void SCPressaoExp::_ajusteEstrangulamento() { 
  float curva = mapCurvaFloat(
    estadoPidPressaoExp.setpointAtual,
    paramValvulaExalacao.curvaSistema,
    paramValvulaExalacao.curvaPiloto,
    11);

  paramPidPressaoExp.I0 = curva;
  paramPidPressaoExp.minn = curva - paramAjusteControle.deltaEstrangMin;
  paramPidPressaoExp.maxx = curva + paramAjusteControle.deltaEstrangMax;
  pidPressaoExp.moverIntegral();
}


void SCPressaoExp::_setDefaultParametrosPid() {
#ifdef SIMULADOR
  paramPidPressaoExp.Kp = 100.0;
  paramPidPressaoExp.Ti = 0.5;
  paramPidPressaoExp.Td = 0.0;
  paramPidPressaoExp.N = 0.0;
  paramPidPressaoExp.Tt = 1.0;
  paramPidPressaoExp.I0 = 1500.0;
  paramPidPressaoExp.minn = 0.0;
  paramPidPressaoExp.maxx = 4095.0;

  paramPidPressaoExp.pontosAutoKp[0] = 10.0;
  paramPidPressaoExp.ganhosAutoKp[0] = 1.0;
  paramPidPressaoExp.pontosAutoKp[1] = 20.0;
  paramPidPressaoExp.ganhosAutoKp[1] = 1.0;
  paramPidPressaoExp.pontosAutoKp[2] = 30.0;
  paramPidPressaoExp.ganhosAutoKp[2] = 1.0;
  paramPidPressaoExp.pontosAutoKp[3] = 40.0;
  paramPidPressaoExp.ganhosAutoKp[3] = 1.0;
  paramPidPressaoExp.pontosAutoKp[4] = 50.0;
  paramPidPressaoExp.ganhosAutoKp[4] = 1.0;

  paramPidPressaoPil.Kp = 0.0;
  paramPidPressaoPil.Ti = 0.0;
  paramPidPressaoPil.Td = 0.0;
  paramPidPressaoPil.N = 0.0;
  paramPidPressaoPil.Tt = 1.0;
  paramPidPressaoPil.I0 = 0.0;
  paramPidPressaoPil.minn = 0.0;
  paramPidPressaoPil.maxx = 4095.0;

  paramPidPressaoPil.pontosAutoKp[0] = 10.0;
  paramPidPressaoPil.ganhosAutoKp[0] = 1.0;
  paramPidPressaoPil.pontosAutoKp[1] = 20.0;
  paramPidPressaoPil.ganhosAutoKp[1] = 1.0;
  paramPidPressaoPil.pontosAutoKp[2] = 30.0;
  paramPidPressaoPil.ganhosAutoKp[2] = 1.0;
  paramPidPressaoPil.pontosAutoKp[3] = 40.0;
  paramPidPressaoPil.ganhosAutoKp[3] = 1.0;
  paramPidPressaoPil.pontosAutoKp[4] = 50.0;
  paramPidPressaoPil.ganhosAutoKp[4] = 1.0;
  
#else // MAQUINA
  paramPidPressaoExp.Kp = 10.0;
  paramPidPressaoExp.Ti = 2.0;
  paramPidPressaoExp.Td = 0.010;
  paramPidPressaoExp.N = 0.0;
  paramPidPressaoExp.Tt = 0.01;
  paramPidPressaoExp.I0 = 65.0;
  paramPidPressaoExp.minn = 10.0;
  paramPidPressaoExp.maxx = 65.0;

  paramPidPressaoExp.pontosAutoKp[0] = 10.0;
  paramPidPressaoExp.ganhosAutoKp[0] = 1.0;
  paramPidPressaoExp.pontosAutoKp[1] = 20.0;
  paramPidPressaoExp.ganhosAutoKp[1] = 1.0;
  paramPidPressaoExp.pontosAutoKp[2] = 30.0;
  paramPidPressaoExp.ganhosAutoKp[2] = 1.0;
  paramPidPressaoExp.pontosAutoKp[3] = 40.0;
  paramPidPressaoExp.ganhosAutoKp[3] = 1.0;
  paramPidPressaoExp.pontosAutoKp[4] = 50.0;
  paramPidPressaoExp.ganhosAutoKp[4] = 1.0;

  paramPidPressaoPil.Kp = 30.0;
  paramPidPressaoPil.Ti = 0.100;
  paramPidPressaoPil.Td = 0.0;
  paramPidPressaoPil.N = 0.0;
  paramPidPressaoPil.Tt = 0.01;
  paramPidPressaoPil.I0 = 500.0;
  paramPidPressaoPil.minn = 0.0;
  paramPidPressaoPil.maxx = 4095.0;

  paramPidPressaoPil.pontosAutoKp[0] = 10.0;
  paramPidPressaoPil.ganhosAutoKp[0] = 1.0;
  paramPidPressaoPil.pontosAutoKp[1] = 20.0;
  paramPidPressaoPil.ganhosAutoKp[1] = 1.0;
  paramPidPressaoPil.pontosAutoKp[2] = 30.0;
  paramPidPressaoPil.ganhosAutoKp[2] = 1.0;
  paramPidPressaoPil.pontosAutoKp[3] = 40.0;
  paramPidPressaoPil.ganhosAutoKp[3] = 1.0;
  paramPidPressaoPil.pontosAutoKp[4] = 50.0;
  paramPidPressaoPil.ganhosAutoKp[4] = 1.0;

#endif
}
