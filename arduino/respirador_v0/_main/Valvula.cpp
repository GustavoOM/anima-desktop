#include "_global.h"

Valvula::Valvula() { }

void Valvula::setup() {
  _it = 0;

  _setDefaultParametrosPid();
  objetosCal.pid->setup(objetosCal.paramPid, objetosCal.estadoPid);
}

void Valvula::loop() {
  objetosCal.estadoPid->setpointAtual = _calcSetpoint();
  
  if (objetosCal.paramPid->autoKpPorDeltaSet) {
    if (objetosCal.estadoPid->setpointAtual == paramCal.valor1Deg) {
      objetosCal.estadoPid->refDelta = paramCal.valor2Deg;
    }
    if (objetosCal.estadoPid->setpointAtual == paramCal.valor2Deg) {
      objetosCal.estadoPid->refDelta = paramCal.valor1Deg;
    }
  }

  objetosCal.pid->loop();
}

void Valvula::resetPid() {
  objetosCal.pid->resetEstadoPid();
}

float Valvula::_calcSetpoint() {
  float tAtual = (_it+0.5) * (TAMS_PID/1000.0);
  
  float set;
  if (tAtual < paramCal.periodoDeg/1000.0) {
    set = paramCal.valor1Deg;
  } else {
    set = paramCal.valor2Deg;
  }
  
  _it++;
  if (tAtual > (2.0 * paramCal.periodoDeg/1000.0)) {
    _it = 0;
  }
  
  return (set);
}

void Valvula::_setDefaultParametrosPid() {
  objetosCal.paramPid->Kp = 1.0;
  objetosCal.paramPid->Ti = 0.1;
  objetosCal.paramPid->Td = 0.0;
  objetosCal.paramPid->N = 0.0;
  objetosCal.paramPid->Tt = 0.01;
  objetosCal.paramPid->I0 = 0.0;
  objetosCal.paramPid->minn = 0.0;
  objetosCal.paramPid->maxx = 4095.0;

  objetosCal.paramPid->pontosAutoKp[0] = 10.0;
  objetosCal.paramPid->ganhosAutoKp[0] = 1.0;
  objetosCal.paramPid->pontosAutoKp[1] = 20.0;
  objetosCal.paramPid->ganhosAutoKp[1] = 1.0;
  objetosCal.paramPid->pontosAutoKp[2] = 30.0;
  objetosCal.paramPid->ganhosAutoKp[2] = 1.0;
  objetosCal.paramPid->pontosAutoKp[3] = 40.0;
  objetosCal.paramPid->ganhosAutoKp[3] = 1.0;
  objetosCal.paramPid->pontosAutoKp[4] = 50.0;
  objetosCal.paramPid->ganhosAutoKp[4] = 1.0;
}
