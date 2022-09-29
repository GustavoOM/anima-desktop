#include "_global.h"

//#define VERBOSE

Ensaio::Ensaio() { }

void Ensaio::setup() {
  _pwmFluxoBase = 0;
  _pwmPilotoBase = 0;
  _tLeitura = 500;
}

bool Ensaio::ensaiaSensorFluxo(SensFluxEnum sensFlux, ParametrosSensorFluxo *dados) {
#ifdef VERBOSE
  bool verb = true;
#else
  bool verb = false;
#endif

  int nLimFix = 10;
  float limiaresFixos[10] = { 3, 5, 7, 9, 12, 15, 18, 22, 26, 30 };
  float bufferAdc[60];
  float bufferFluxo[60];
  
  bool semFalha = true;
  if (sensFlux != SENS_FLUX_EX) {
    semFalha = _ajustaFluxoBase(true);
  } else {
    semFalha = _ajustaFluxoBase(false);
  }
  if (!semFalha) {
    return false;
  }
  semFalha = _ajustaPilotoBase();
  if (!semFalha) {
    return false;
  }
  _acionaValvParaSensFlux(sensFlux, 0);
  _leSensoresPorTempo(_tLeitura);
  dados->curvaAdc[0] = _medeAdcSensFlux(sensFlux);
  dados->curvaFluxo[0] = 0;
  
  if (verb) {
    Serial.println(" ");
    Serial.println("Buscando fluxo positivo");
    Serial.println("    [pwm]  [fluxo]");
  }
  int pwm = 0;
  do {
      pwm += 100;
      if (pwm > 4000) {
        return false;
      }
      _acionaValvParaSensFlux(sensFlux, pwm);
      _leSensoresPorTempo(_tLeitura);
      if (verb) {
        sprintf(_charBuf, "    %5d  %7.1f", pwm, _medeSensFlux(sensFlux));
        Serial.println(_charBuf);
      }
  } while (_medeSensFlux(sensFlux) < 4.0);
  pwm -= 200;
  _acionaValvParaSensFlux(sensFlux, 0);
  _leSensoresPorTempo(_tLeitura);
  
  int pwmInicial = pwm;
  
  if (verb) {
    Serial.println(" ");
    Serial.println("Coletando pontos da curva");
    Serial.println("    [pwm]  [fluxo]  [limiar]  [valorAdc]");
  }
  int passo = 10;
  if (sensFlux == SENS_FLUX_EX) {
      passo = 2;
  }
  int contaPasso = 0;
  int contaLimiaresFixos = 0;
  int contaLinha = 0;
  int contaBuffer = 0;
  float limiarAtual = limiaresFixos[contaLimiaresFixos];

  float fluxoAtual = 0;
  float adcAtual = dados->curvaAdc[0];
  float adcAnterior = dados->curvaAdc[0];

  if (verb) {
    int printPwm = 0;
    float printLimiar = 0;
    sprintf(_charBuf, "    %5d  %7.1f  %8.1f  %10.0f", printPwm, dados->curvaFluxo[0], printLimiar, adcAtual);
    Serial.println(_charBuf);
  }
  
  bool repetePwm = false;
  contaLinha++;
  while (pwm < 4000) {
    if (!repetePwm) {
      pwm += passo;
    }
    contaPasso++;
    if (contaPasso == 3) {
      if (_medeSensFlux(sensFlux) > 20.0) {
        if (passo < 40) {
          passo += 10;
        }
      }
      contaPasso = 0;
    }
    _acionaValvParaSensFlux(sensFlux, pwm);
    if (passo != 2) {
      _leSensoresPorTempo(_tLeitura);
    } else {
      _leSensoresPorTempo(200);
    }
    
    fluxoAtual = _medeSensFlux(sensFlux);
    adcAtual = _medeAdcSensFlux(sensFlux);
    if (verb) { 
      sprintf(_charBuf, "    %5d  %7.1f  %8.1f  %10.0f", pwm, fluxoAtual, limiarAtual, adcAtual);
      Serial.println(_charBuf);
    }
    contaLinha++;
    if (verb && contaLinha % 15 == 0) {
      Serial.println("    [pwm]  [fluxo]  [limiar]  [valorAdc]");
    }

    if (fluxoAtual >= limiarAtual) {
      contaPasso = 0;
      
      if (adcAtual > adcAnterior) {
        if (contaLimiaresFixos < nLimFix) {
          contaLimiaresFixos++;
          dados->curvaAdc[contaLimiaresFixos] = adcAtual;
          dados->curvaFluxo[contaLimiaresFixos] = _medeSensFlux(sensFlux);
        } else {
          bufferAdc[contaBuffer] = adcAtual;
          bufferFluxo[contaBuffer] = _medeSensFlux(sensFlux);
          if (contaBuffer < 59) {
              contaBuffer++;
          }
        }
        adcAnterior = adcAtual;

        if (contaLimiaresFixos < nLimFix) {
          limiarAtual = limiaresFixos[contaLimiaresFixos];
        } else {
          if (sensFlux != SENS_FLUX_EX) {
            limiarAtual += 2;  
          }
          else {
            limiarAtual += 4;
          }
        }
      }
      else {
        repetePwm = !repetePwm;
      }
    }
    else {
      repetePwm = false;  
    }
  }
  if (contaBuffer < 7) {
    return false;
  }

  dados->curvaAdc[17] = bufferAdc[contaBuffer-1];
  dados->curvaFluxo[17] = bufferFluxo[contaBuffer-1];
  int nLimAdi = 18 - nLimFix - 2;
  float fluxoMaximo = dados->curvaFluxo[17];
  float ultimoLimiarFixo = limiaresFixos[nLimFix-1];
  float incremento = (fluxoMaximo - ultimoLimiarFixo) / (nLimAdi + 1);
  float limiar = ultimoLimiarFixo + incremento;
  int indiceAtualCurva = 1 + nLimFix;
  for (int i=0; i<contaBuffer; i++) {
    bool bufferEsgotando = contaBuffer - i <= (18-1) - indiceAtualCurva;
    if (bufferEsgotando) {
      limiar = 0;
    }
    if (bufferFluxo[i] > limiar) {
      dados->curvaAdc[indiceAtualCurva] = bufferAdc[i];
      dados->curvaFluxo[indiceAtualCurva] = bufferFluxo[i];
      indiceAtualCurva++;
      if (!bufferEsgotando) {
        limiar += incremento;
      }
    }
    if (indiceAtualCurva == 17) {
      break;
    }
  }

  desligaValvulas();
  return true;
}

bool Ensaio::ensaiaValvulaAOP(ValvulasAOPEnum valv, float* dadosValv) {
#ifdef VERBOSE
  bool verb = true;
#else
  bool verb = false;
#endif

  int pwm;
  if (verb) {
    Serial.println(" ");
    Serial.println("Buscando pwm de abertura grosseiro");
    Serial.println("    [pwm]  [saida]");
  }
  pwm = 0;
  while (true) {
    _acionaValvulaAOP(valv, pwm);
    _leSensoresPorTempo(_tLeitura);
    if (verb) {
      sprintf(_charBuf, "    %5d  %7.1f", pwm, _medeValvulaAOP(valv));
      Serial.println(_charBuf);
    }
    if (_medeValvulaAOP(valv) > SAIDA_ZERO) {
      break;
    }
    pwm += 200;
    if (pwm > 4000) {
      return false;
    }
  }
  int pwmAberturaGrosseiro = saturaFloat(pwm - 200, 0, 4000);

  if (verb) {
    Serial.println(" ");
    Serial.println("Buscando pwm de fechamento grosseiro");
    Serial.println("    [pwm]  [saida]");
  }
  if (valv != VALV_PILOTO) {
    pwm = 4000;
  } else {
    pwm = 3000;
  }
  while (true) {
    _acionaValvulaAOP(valv, pwm);
    _leSensoresPorTempo(_tLeitura);
    if (verb) {
      sprintf(_charBuf, "    %5d  %7.1f", pwm, _medeValvulaAOP(valv));
      Serial.println(_charBuf);
    }
    if (0 <= _medeValvulaAOP(valv) && _medeValvulaAOP(valv) < SAIDA_ZERO) {
      break;
    }
    if (valv != VALV_PILOTO) {
      pwm -= 200;
    } else {
      pwm -= 40;
    }
    if (pwm < 0) {
      return false;
    }
  }
  int pwmFechamentoGrosseiro = saturaFloat(pwm + 200, 0, 4000);
  
  _acionaValvulaAOP(valv, 0);
  _leSensoresPorTempo(_tLeitura);

  if (verb) {
    Serial.println(" ");
    Serial.println("Coletando pwm de abertura");
    Serial.println("    [pwm]  [saida]");
  }
  pwm = pwmAberturaGrosseiro;
  while (true) {
    _acionaValvulaAOP(valv, pwm);
    _leSensoresPorTempo(_tLeitura);
    if (verb) {
      sprintf(_charBuf, "    %5d  %7.1f", pwm, _medeValvulaAOP(valv));
      Serial.println(_charBuf);
    }
    if (_medeValvulaAOP(valv) > SAIDA_ZERO) {
      break;
    }
    pwm += 40;
    if (pwm > 4000) {
      return false;
    }
  }
  dadosValv[0] = saturaFloat(pwm, 0, 4000);

  if (verb) {
    Serial.println(" ");
    Serial.println("Buscando saida maxima");
    Serial.println("    [pwm]  [saida]");
  }
  if (valv != VALV_PILOTO) {
    pwm = 4000;
  } else {
    pwm = 3000;
  }
  while (true) {
    _acionaValvulaAOP(valv, pwm);
    _leSensoresPorTempo(_tLeitura);
    if (verb) {
      sprintf(_charBuf, "    %5d  %7.1f", pwm, _medeValvulaAOP(valv));
      Serial.println(_charBuf);    
    }
    if (_medeValvulaAOP(valv) > 0) {
      break;
    }
    if (valv != VALV_PILOTO) {
      pwm -= 200;
    } else {
      pwm -= 40;
    }
    if (pwm < 0) {
      return false;
    }
  }
  dadosValv[2] = _medeValvulaAOP(valv);

  if (verb) {
    Serial.println(" ");
    Serial.println("Buscando pwm de fechamento");
    Serial.println("    [pwm]  [saida]");
  }
  pwm = pwmFechamentoGrosseiro;
  while (true) {
    _acionaValvulaAOP(valv, pwm);
    _leSensoresPorTempo(_tLeitura);
    if (verb) {
      sprintf(_charBuf, "    %5d  %7.1f", pwm, _medeValvulaAOP(valv));
      Serial.println(_charBuf);
    }
    if (0 <= _medeValvulaAOP(valv) && _medeValvulaAOP(valv) < SAIDA_ZERO) {
      break;
    }
    if (valv != VALV_PILOTO) {
      pwm -= 40;
    } else {
      pwm -= 20;
    }
    if (pwm < 0) {
      return false;
    }
  }
  dadosValv[1] = pwm;
  desligaValvulas();
  return true;
}

bool Ensaio::ensaiaValvulaExalacao(float* curvaExalacaoPiloto, float* curvaExalacaoSistema, float* limiares) {
#ifdef VERBOSE
  bool verb = true;
#else
  bool verb = false;
#endif

  int pwm = 0;
  curvaExalacaoPiloto[0] = 0.0;
  curvaExalacaoSistema[0] = 0.0;

  if (_ajustaFluxoBase(false)) {
    if (verb) {
      Serial.println(" ");
      Serial.println("Buscando inicio da curva");
      Serial.println("    [pwm]  [presPil]");
    }
    pwm = 0;
    do {
      pwm += 80;
      if (pwm > 4000) {
        return false;
      }
      valvulaOxExalacaoVP3(pwm);
      _leSensoresPorTempo(_tLeitura);
      if (verb) {
        sprintf(_charBuf, "    %5d %9.1f", pwm, statusSensores.valorPressao[5]);
        Serial.println(_charBuf);
      }
    } while (statusSensores.valorPressao[5] < SAIDA_ZERO);
    
    pwm -= 80;
    valvulaOxExalacaoVP3(0);
    _leSensoresPorTempo(_tLeitura);
    if (verb) {
      Serial.println(" ");
      Serial.println("Coletando pontos");
      Serial.println("    [pwm]  [presPil]  [limiar]  [presSis]");
    }
    
    float pressSisAnterior = 0.0;
    float pressSisAtual = 0.0;
    int contaLimiares = 0;
    int contaPontos = 1;
    int contaPasso = 0;
    int passo = 3;
    int contaLinha = 0;

    do {
      pressSisAnterior = pressSisAtual;
      pwm += passo;
      contaPasso++;
      if (contaPasso == 3 && contaPontos > 5) {
        if (passo < 30) {
          passo += 3;
        }   
        contaPasso = 0;
      }
      if (pwm > 4000) {
        return false;
      }
      valvulaOxExalacaoVP3(pwm);
      _leSensoresPorTempo(_tLeitura);
      pressSisAtual = (statusSensores.valorPressao[4] + statusSensores.valorPressao[3]) / 2.0;
      if (verb) {
        sprintf(
          _charBuf, "    %5d  %9.1f  %8.1f  %9.1f",
          pwm,
          statusSensores.valorPressao[5],
          limiares[contaLimiares],
          pressSisAtual
        );
        Serial.println(_charBuf);
        contaLinha++;

        if (contaLinha % 15 == 0) {
          Serial.println("    [pwm]  [presPil]  [limiar]  [presSis]");
        }
      }

      if (pressSisAtual > limiares[contaLimiares] && contaLimiares < 9) {
        contaLimiares++;
        curvaExalacaoPiloto[contaPontos] = statusSensores.valorPressao[5];
        curvaExalacaoSistema[contaPontos] = pressSisAtual;
        contaPontos++;
        contaPasso = 0;
      }

    } while (pressSisAtual - pressSisAnterior > 0.5 || pressSisAtual < limiares[8]);
    curvaExalacaoPiloto[contaPontos] = statusSensores.valorPressao[5];
    curvaExalacaoSistema[contaPontos] = pressSisAtual;
    contaPontos++;

    desligaValvulas();
    return true;
  }
  else {
    desligaValvulas();
    return false;
  }
}

bool Ensaio::ensaiaValvulaSeguranca(int* tResposta) {
  simulaSensores();
  *tResposta = ensaioTempoValvulaSeg;
  return true;
}

bool Ensaio::ensaiaCircuitoPaciente(float* volumeVazado) {
  simulaSensores();
  *volumeVazado = ensaioVazamentoCircuitoPac;
  return true;
}

bool Ensaio::_ajustaFluxoBase(bool usarSensorFluxoCalib) {
#ifdef VERBOSE
  bool verb = true;
#else
  bool verb = false;
#endif

  int pwm = 0;
  float fluxo = 0; 

  if (verb) {
    Serial.println(" ");
    Serial.println("Buscando fluxo positivo");
    Serial.println("    [pwm]  [fluxo]");
  }
  do {
    pwm += 80;
    if (pwm > 4000) {
      return false;
    }
    valvulaFluxoArVP1(pwm);
    
    _leSensoresPorTempo(200);
    if (usarSensorFluxoCalib) {
      fluxo = statusSensores.fluxoCalibracao;
    } else {
      fluxo = statusSensores.valorFluxo[1];
    }
    if (verb) {
      sprintf(_charBuf, "    %5d  %7.1f", pwm, fluxo);
      Serial.println(_charBuf);
    }
  } while (fluxo < 3.0);

  if (verb) {
    Serial.println(" ");
    Serial.println("Ajustando pwm para fluxo base");
    Serial.println("    [pwm]  [fluxo]");
  }
  do {
    pwm += 10;
    if (pwm > 4000) {
      return false;
    }
    valvulaFluxoArVP1(pwm);
    _leSensoresPorTempo(_tLeitura);
    if (usarSensorFluxoCalib) {
      fluxo = statusSensores.fluxoCalibracao;
    } else {
      fluxo = statusSensores.valorFluxo[1];
    }
    if (verb) {
      sprintf(_charBuf, "    %5d  %7.1f", pwm, fluxo);
      Serial.println(_charBuf);
    }
  } while (fluxo < FLUXO_BASE);

  _pwmFluxoBase = pwm;

  return true;
}

bool Ensaio::_ajustaPilotoBase() {
#ifdef VERBOSE
  bool verb = true;
#else
  bool verb = false;
#endif

  int pwm = 0;

  if (verb) {
    Serial.println(" ");
    Serial.println("Buscando pressurizacao positiva do sistema");
    Serial.println("    [pwmPil]  [presPil]  [presSis]");
  }
  do {
    pwm += 80;
    if (pwm > 4000) {
      return false;
    }
    valvulaOxExalacaoVP3(pwm);
    _leSensoresPorTempo(_tLeitura);
    if (verb) {
      sprintf(_charBuf, "    %5d  %9.1f  %9.1f", pwm, statusSensores.valorPressao[5], statusSensores.valorPressao[4]);
      Serial.println(_charBuf);
    }
  } while (statusSensores.valorPressao[4] < 1.0);

  if (verb) {
    Serial.println(" ");
    Serial.println("Ajustando piloto para pressurizar sistema");
    Serial.println("    [pwmPil]  [presPil]  [presSis]");
  }
  do {
    pwm += 5;
    if (pwm > 4000) {
      return false;
    }
    valvulaOxExalacaoVP3(pwm);
    _leSensoresPorTempo(_tLeitura);
    if (verb) {
      sprintf(_charBuf, "    %5d  %9.1f  %9.1f", pwm, statusSensores.valorPressao[5], statusSensores.valorPressao[4]);
      Serial.println(_charBuf);
    }
  } while (statusSensores.valorPressao[4] < 5.0);

  _pwmPilotoBase = pwm;

  valvulaFluxoArVP1(0);
  return true;
}

void Ensaio::_leSensoresPorTempo(int periodo) {
  simulaSensores();
}

void Ensaio::_acionaValvulaAOP(ValvulasAOPEnum valv, int pwm) {
  switch (valv) {
    case VALV_AR:
      valvulaFluxoArVP1(pwm); break;
    case VALV_O2:
      valvulaFluxoOxVP2(pwm); break;
    case VALV_PILOTO:
      valvulaOxExalacaoVP3(pwm); break;
  }
}

float Ensaio::_medeValvulaAOP(ValvulasAOPEnum valv) {
  switch (valv) {
    case VALV_AR: return statusSensores.valorFluxo[1];
    case VALV_O2: return statusSensores.valorFluxo[2];
    case VALV_PILOTO: return statusSensores.valorPressao[5];
  }
}

void Ensaio::_acionaValvParaSensFlux(SensFluxEnum sensFlux, int pwm) {
  switch(sensFlux) {
    case SENS_FLUX_AR:
      valvulaFluxoArVP1(pwm);
      break;
    case SENS_FLUX_O2:
      valvulaFluxoOxVP2(pwm);
      break;
    case SENS_FLUX_EX:
      valvulaFluxoArVP1(pwm);
      valvulaFluxoOxVP2(pwm);
      break;
  }
}

float Ensaio::_medeAdcSensFlux(SensFluxEnum sensFlux) {
  float adc = 0;
  switch (sensFlux) {
    case SENS_FLUX_AR:
      adc = statusSensores.adcFluxo[1]; break;
    case SENS_FLUX_O2:
      adc = statusSensores.adcFluxo[2]; break;
    case SENS_FLUX_EX:
      adc = statusSensores.adcFluxo[4]; break;
  }
  return adc;
}

float Ensaio::_medeSensFlux(SensFluxEnum sensFlux) {
  if (sensFlux != SENS_FLUX_EX) {
    return statusSensores.fluxoCalibracao;
  } else {
    return statusSensores.valorFluxo[1] + statusSensores.valorFluxo[2];
  }
}

void Ensaio::simulaSensores() {
  statusSensores.tensaoFonte = dadosTeste.tensaoFonte;
  statusSensores.tensaoBateria = dadosTeste.tensaoBateria;

  statusSensores.valorPressao[1] = dadosTeste.pressaoAr;
  statusSensores.valorPressao[2] = dadosTeste.pressaoO2;

  for (int i=3; i<=5; i++) {
    statusSensores.erroTimeoutPressao[i] = dadosTeste.erroTimeoutPressao[i-3];
    statusSensores.erroOffsetPressao[i] = dadosTeste.erroOffsetPressao[i-3];
  }
  for (int i=1; i<=4; i++) {
    statusSensores.erroTimeoutFluxo[i] = dadosTeste.erroTimeoutFluxo[i-1];
    statusSensores.erroOffsetFluxo[i] = dadosTeste.erroOffsetFluxo[i-1];
  }

  statusSensores.valorFluxo[1] = mapFloat(
    pwmValvAr,
    dadosTeste.minPwmValvulasAOP[0],
    dadosTeste.maxPwmValvulasAOP[0],
    dadosTeste.minSaidaValvulasAOP[0],
    dadosTeste.maxSaidaValvulasAOP[0]
  );
  statusSensores.valorFluxo[2] = mapFloat(
    pwmValvO2,
    dadosTeste.minPwmValvulasAOP[1],
    dadosTeste.maxPwmValvulasAOP[1],
    dadosTeste.minSaidaValvulasAOP[1],
    dadosTeste.maxSaidaValvulasAOP[1]
  );
  statusSensores.valorPressao[5] = mapFloat(
    pwmValvPil,
    dadosTeste.minPwmValvulasAOP[2],
    dadosTeste.maxPwmValvulasAOP[2],
    dadosTeste.minSaidaValvulasAOP[2],
    dadosTeste.maxSaidaValvulasAOP[2]
  );
  
  statusSensores.valorFluxo[4] = statusSensores.valorFluxo[1] + statusSensores.valorFluxo[2];
  statusSensores.fluxoCalibracao = statusSensores.valorFluxo[4]; 

  statusSensores.adcFluxo[1] = mapFloat(
    statusSensores.valorFluxo[1],
    dadosTeste.minSaidaValvulasAOP[0],
    dadosTeste.maxSaidaValvulasAOP[0],
    dadosTeste.minAdcSensoresFluxo[0],
    dadosTeste.maxAdcSensoresFluxo[0]
  );
  statusSensores.adcFluxo[2] = mapFloat(
    statusSensores.valorFluxo[2],
    dadosTeste.minSaidaValvulasAOP[1],
    dadosTeste.maxSaidaValvulasAOP[1],
    dadosTeste.minAdcSensoresFluxo[1],
    dadosTeste.maxAdcSensoresFluxo[1]
  );
  statusSensores.adcFluxo[4] = mapFloat(
    statusSensores.valorFluxo[4],
    dadosTeste.minSaidaValvulasAOP[0] + dadosTeste.minSaidaValvulasAOP[1],
    dadosTeste.maxSaidaValvulasAOP[0] + dadosTeste.minSaidaValvulasAOP[1],
    dadosTeste.minAdcSensoresFluxo[2],
    dadosTeste.maxAdcSensoresFluxo[2]
  );
  
  statusSensores.valorPressao[3] = mapFloat(
    statusSensores.valorPressao[5],
    dadosTeste.minPilotoCurvaSis,
    dadosTeste.maxPilotoCurvaSis,
    dadosTeste.minPressaoSis,
    dadosTeste.maxPressaoSis
  );
  statusSensores.valorPressao[4] = mapFloat(
    statusSensores.valorPressao[5],
    dadosTeste.minPilotoCurvaSis,
    dadosTeste.maxPilotoCurvaSis,
    dadosTeste.minPressaoSis,
    dadosTeste.maxPressaoSis
  );
  
  ensaioTempoValvulaSeg = dadosTeste.tempoValvulaSeg;
  ensaioVazamentoCircuitoPac = dadosTeste.vazamentoCircuitPac;
}
