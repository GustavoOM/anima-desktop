#include "_global.h"


Pid::Pid() { }


void Pid::setup(ParametrosPid *param, EstadoPid *estado) {
  _param = param;
  _estado = estado;
}


void Pid::loop() {
  _checaTt(); // coeficiente de windup
  _autoKp();

  if (_estado->setpointAnterior == 0.0) { moverIntegral(); }

  if (_estado->setpointAtual == 0.0) { _zeraSaida(); }
  else {
    // cálculo das constantes
    float P, I1, I2, D1, D2;
    
    P = _estado->ganho * _param->Kp;

    if (_param->Ti > 0.0) {
      I1 = (_estado->ganho * _param->Kp * (TAMS_PID/1000.0)) / _param->Ti;
      I2 = (TAMS_PID/1000.0) / _param->Tt;
    } else {
      I1 = 0.0;
      I2 = 0.0;
    }

    if (_param->Td > 0.0) {
      D1 = _param->Td / (_param->Td  +  _param->N * (TAMS_PID/1000.0));
      D2 = _estado->ganho * _param->Kp * _param->N * D1;
    } else {
      D1 = 0.0;
      D2 = 0.0;
    }

    // cálculo das saídas
    float erro = _estado->setpointAtual - _estado->sensorAtual;
    float deltaSensor = _estado->sensorAtual - _estado->sensorAnterior;

    _estado->saidaProp = P * erro;
    _estado->saidaInte = _estado->integral;
    _estado->saidaDeri = D1 * _estado->derivativoAnterior  -  D2 * deltaSensor;
    _estado->saidaCalc = _estado->saidaProp + _estado->saidaInte + _estado->saidaDeri;

    // saturação
    if (_estado->saidaCalc > _param->maxx) {
      _estado->saidaAtual = _param->maxx;
    } else if (_estado->saidaCalc < _param->minn) {
      _estado->saidaAtual = _param->minn;
    } else {
      _estado->saidaAtual = _estado->saidaCalc;
    }


    if (_estado->saidaAtual < 0) {
      _estado->saidaAtual = 0;
    }
    if (_estado->saidaAtual > 4095) {
      _estado->saidaAtual = 4095;
    }

    // histerese
    _estado->saidaSemCompHist = _estado->saidaAtual;
    if (_estado->saidaAnterior > _estado->saidaAtual && _param->histerese > 0.0) {
      _estado->saidaAtual = _estado->saidaAtual - _param->histerese;
    }

    // atualização da integral
    float deltaSaida = _estado->saidaAtual - _estado->saidaCalc;
    _estado->integral += I1 * erro  +  I2 * deltaSaida;
  }

  // atualização do estado
  _estado->setpointAnterior = _estado->setpointAtual;
  _estado->sensorAnterior = _estado->sensorAtual;
  _estado->derivativoAnterior = _estado->saidaDeri;
  _estado->saidaAnterior = _estado->saidaSemCompHist;
}


void Pid::moverIntegral() {
	_estado->integral = _param->I0;
}


void Pid::resetEstadoPid() {
  _estado->ganho = 1.0;
  _estado->integral = 0.0;
  _estado->setpointAtual = 0.0;
  _estado->sensorAtual = 0.0;
  _estado->setpointAnterior = 0.0;
  _estado->sensorAnterior = 0.0;
  _estado->derivativoAnterior = 0.0;
  _estado->saidaAnterior = 0.0;
  _estado->refDelta = 0.0;

  _zeraSaida();
}


void Pid::_zeraSaida() {
  _estado->saidaProp = 0.0;
  _estado->saidaInte = 0.0;
  _estado->saidaDeri = 0.0;
  _estado->saidaCalc = 0.0;
  _estado->saidaAtual = 0.0;
}


void Pid::_checaTt() {
  if (_param->Ti > 0.0 && _param->Td > 0.0) {
    _param->Tt = sqrt(_param->Ti * _param->Td);
  } else {
    _param->Tt = TAMS_PID/1000.0;
  }
  
  if (_param->Tt < TAMS_PID/1000.0) {
    _param->Tt = TAMS_PID/1000.0;
  }
}


void Pid::_autoKp() {

  float entrada;
  if (_param->autoKpPorDeltaSet) {
    entrada = abs(_estado->refDelta - _estado->setpointAtual);
  } else { //autoKpPorSetpoint
    entrada = _estado->setpointAtual;
  }
  
  // saturação mínima
  if (entrada < _param->pontosAutoKp[0]) {
    _estado->ganho = _param->ganhosAutoKp[0];
  }

  // interpolação linear
  for (int i=0; i<4; i++) {
    if ((entrada >= _param->pontosAutoKp[i]) && (entrada < _param->pontosAutoKp[i+1])) {
      _estado->ganho = mapFloat(entrada,
                                _param->pontosAutoKp[i],
                                _param->pontosAutoKp[i+1],
                                _param->ganhosAutoKp[i],
                                _param->ganhosAutoKp[i+1]);         
    }
  }

  // saturação máxima
  if (entrada >= _param->pontosAutoKp[4]) {
    _estado->ganho = _param->ganhosAutoKp[4];
  }
}
