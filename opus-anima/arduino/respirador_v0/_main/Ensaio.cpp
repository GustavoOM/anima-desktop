#include "_global.h"

//#define VERBOSE

Ensaio::Ensaio() { }

void Ensaio::setup() {
  _pwmFluxoBase = 0;
  _pwmPilotoBase = 0;
  _tLeitura = 500;
}

bool Ensaio::ensaiaSensorFluxo(SensFluxEnum sensFlux, ParametrosSensorFluxo *dados) {
#ifdef RESPIRADOR
  bool enviaProgresso = true;
#else
  bool enviaProgresso = false;
#endif
  bool verb = true;

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
  delay(1000);
  _leSensoresPorTempo(_tLeitura);
  dados->curvaAdc[0] = _medeAdcSensFlux(sensFlux);
  dados->curvaFluxo[0] = 0;
  
  if (verb) {
    saidaRasp.enviaTexto(" ");
    saidaRasp.enviaTexto("Buscando fluxo positivo");
    saidaRasp.enviaTexto("    [pwm]  [fluxo]");
  }
  int pwm = 0;
  do {
      pwm += 100;
      if (pwm > 4000) {
        if (enviaProgresso) {
          saidaRasp.enviaResultadoCalibracaoFluxoEx(-1);
        }
        return false;
      }
      _acionaValvParaSensFlux(sensFlux, pwm);
      _leSensoresPorTempo(_tLeitura);
      if (verb) {
        sprintf(_charBuf, "    %5d  %7.1f", pwm, _medeSensFlux(sensFlux));
        saidaRasp.enviaTexto(_charBuf);
      }
  } while (_medeSensFlux(sensFlux) < 4.0);
  pwm -= 200;
  _acionaValvParaSensFlux(sensFlux, 0);
  _leSensoresPorTempo(_tLeitura);
  
  int pwmInicial = pwm;
  int contaProgresso = 1;
  if (enviaProgresso) {
    saidaRasp.enviaResultadoCalibracaoFluxoEx(contaProgresso);
    contaProgresso++;
  }
  
  if (verb) {
    saidaRasp.enviaTexto(" ");
    saidaRasp.enviaTexto("Coletando pontos da curva");
    saidaRasp.enviaTexto("    [pwm]  [fluxo]  [limiar]  [valorAdc]");
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
    saidaRasp.enviaTexto(_charBuf);
  }
  
  bool repetePwm = false;
  contaLinha++;
  while (pwm < 4000) {
    if (!repetePwm) {
      pwm += passo;
    }
    if (enviaProgresso) {
      int passoProgresso = (4000 - pwmInicial) / 8;
      if (pwm > pwmInicial + (contaProgresso - 1) * passoProgresso) {
        saidaRasp.enviaResultadoCalibracaoFluxoEx(contaProgresso);
        contaProgresso++;
      }
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
      saidaRasp.enviaTexto(_charBuf);
    }
    contaLinha++;
    if (verb && contaLinha % 15 == 0) {
      saidaRasp.enviaTexto("    [pwm]  [fluxo]  [limiar]  [valorAdc]");
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

  if (enviaProgresso) {
    saidaRasp.enviaResultadoCalibracaoFluxoEx(contaProgresso);
  }
  desligaValvulas();
  return true;
}

bool Ensaio::ensaiaValvulaAOP(ValvulasAOPEnum valv, float* dadosValv) {
  bool verb = true;

  int pwm;
  if (verb) {
    saidaRasp.enviaTexto(" ");
    saidaRasp.enviaTexto("Buscando pwm de abertura grosseiro");
    saidaRasp.enviaTexto("    [pwm]  [saida]");
  }
  pwm = 0;
  while (true) {
    _acionaValvulaAOP(valv, pwm);
    _leSensoresPorTempo(_tLeitura);
    if (verb) {
      sprintf(_charBuf, "    %5d  %7.1f", pwm, _medeValvulaAOP(valv));
      saidaRasp.enviaTexto(_charBuf);
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
    saidaRasp.enviaTexto(" ");
    saidaRasp.enviaTexto("Buscando pwm de fechamento grosseiro");
    saidaRasp.enviaTexto("    [pwm]  [saida]");
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
      saidaRasp.enviaTexto(_charBuf);
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
    saidaRasp.enviaTexto(" ");
    saidaRasp.enviaTexto("Coletando pwm de abertura");
    saidaRasp.enviaTexto("    [pwm]  [saida]");
  }
  pwm = pwmAberturaGrosseiro;
  while (true) {
    _acionaValvulaAOP(valv, pwm);
    _leSensoresPorTempo(_tLeitura);
    if (verb) {
      sprintf(_charBuf, "    %5d  %7.1f", pwm, _medeValvulaAOP(valv));
      saidaRasp.enviaTexto(_charBuf);
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
    saidaRasp.enviaTexto(" ");
    saidaRasp.enviaTexto("Buscando saida maxima");
    saidaRasp.enviaTexto("    [pwm]  [saida]");
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
      saidaRasp.enviaTexto(_charBuf);    
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
    saidaRasp.enviaTexto(" ");
    saidaRasp.enviaTexto("Buscando pwm de fechamento");
    saidaRasp.enviaTexto("    [pwm]  [saida]");
  }
  pwm = pwmFechamentoGrosseiro;
  while (true) {
    _acionaValvulaAOP(valv, pwm);
    _leSensoresPorTempo(_tLeitura);
    if (verb) {
      sprintf(_charBuf, "    %5d  %7.1f", pwm, _medeValvulaAOP(valv));
      saidaRasp.enviaTexto(_charBuf);
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
  bool verb = true;

  int pwm = 0;
  curvaExalacaoPiloto[0] = 0.0;
  curvaExalacaoSistema[0] = 0.0;

  if (_ajustaFluxoBase(false)) {
    if (verb) {
      saidaRasp.enviaTexto(" ");
      saidaRasp.enviaTexto("Buscando inicio da curva");
      saidaRasp.enviaTexto("    [pwm]  [presPil]");
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
        saidaRasp.enviaTexto(_charBuf);
      }
    } while (statusSensores.valorPressao[5] < SAIDA_ZERO);
    
    pwm -= 80;
    valvulaOxExalacaoVP3(0);
    _leSensoresPorTempo(_tLeitura);
    if (verb) {
      saidaRasp.enviaTexto(" ");
      saidaRasp.enviaTexto("Coletando pontos");
      saidaRasp.enviaTexto("    [pwm]  [presPil]  [limiar]  [presSis]");
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
        saidaRasp.enviaTexto(_charBuf);
        contaLinha++;

        if (contaLinha % 15 == 0) {
          saidaRasp.enviaTexto("    [pwm]  [presPil]  [limiar]  [presSis]");
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
  bool verb = true;

  if (_ajustaFluxoBase(false)) {
    valvulaOxExalacaoVP3(4000);

    if (verb) {
      saidaRasp.enviaTexto(" ");
      saidaRasp.enviaTexto("Aumentando pressao ate atingir limiar de teste");
    }
    unsigned int tInicio = millis();
    while (true) {
      _leSensoresPorTempo(10);
      if (statusSensores.valorPressao[3] > PRESSAO_ENSAIO_VALV_SEG) {
        valvulaFluxoArVP1(0);
        break;
      }
      if (passouTempoMillis(tInicio, PERIODO_ENSAIO_VALV_SEG)) {
        desligaValvulas();
        return false;
      }
    }
    if (verb) {
      sprintf(_charBuf, "    Pressao sistema: %.1f cmH2O", statusSensores.valorPressao[3]);
      saidaRasp.enviaTexto(_charBuf);
    }

    if (verb) {
      saidaRasp.enviaTexto(" ");
      saidaRasp.enviaTexto("Coletando tempo de resposta da valvula");
    }
    tInicio = millis();
    valvulaSeguranca(true);
    do {
      _leSensoresPorTempo(10);
      if (passouTempoMillis(tInicio, PERIODO_ENSAIO_VALV_SEG)) {
        desligaValvulas();
        valvulaSeguranca(false);
        return false;
      }
    } while (statusSensores.valorPressao[3] > SAIDA_ZERO);
    *tResposta = millis() - tInicio;
    if (verb) {
      sprintf(_charBuf, "    Tempo de resposta: %d ms", *tResposta);
      saidaRasp.enviaTexto(_charBuf);
    }

    desligaValvulas();
    valvulaSeguranca(false);
    return true;

  } else {
    desligaValvulas();
    return false;
  }
}

bool Ensaio::ensaiaCircuitoPaciente(float* volumeVazado) {
  bool verb = true;

  valvulaOxExalacaoVP3(0);
  if (ensaio._ajustaFluxoBase(false)) {
    float pressaoInicial = statusSensores.valorPressao[3];
    if (verb) {
      sprintf(_charBuf, "    PressaoInicial: %.1f", pressaoInicial);
      saidaRasp.enviaTexto(_charBuf);
    }
    valvulaOxExalacaoVP3(4000);

    float volumeContido = 0.0;
    unsigned int tInicio = millis();
    unsigned int tInicioEspera = 0;
    unsigned int tAnterior = 0;
    unsigned int tAtual = millis();
    bool emEspera = false;
    while (true) {
      ensaio._leSensoresPorTempo(10);
      tAnterior = tAtual;
      tAtual = millis();
      volumeContido += statusSensores.valorFluxo[1] * (tAtual - tAnterior) / 60000;
      if (statusSensores.valorPressao[3] > PRESSAO_TESTE_VAZAMENTO && !emEspera) {
        valvulaFluxoArVP1(0);
        valvulaFluxoOxVP2(0);
        tInicioEspera = millis();
        if (verb) { saidaRasp.enviaTexto("    Em espera"); }
        emEspera = true;
      }
      if (emEspera) {
        if (passouTempoMillis(tInicioEspera, 100)) {
          break;
        }
      }
      if (passouTempoMillis(tInicio, 5000)) {
        desligaValvulas();
        return false;
      }
    }

    float pressaoFinal = statusSensores.valorPressao[3];

    float complacenciaCalculada = volumeContido / (pressaoFinal - pressaoInicial);
    if (verb) {
      sprintf(_charBuf, "    Pressao final: %.1f", pressaoFinal);
      saidaRasp.enviaTexto(_charBuf);
      sprintf(_charBuf, "    Volume contido: %.3f", 1000*volumeContido);
      saidaRasp.enviaTexto(_charBuf);
      sprintf(_charBuf, "    Complacencia calculada: %.3f", 1000*complacenciaCalculada);
      saidaRasp.enviaTexto(_charBuf);
    }

    tInicioEspera = millis();
    pressaoInicial = statusSensores.valorPressao[3];
    if (verb) {
      sprintf(_charBuf, "    Pressao inicial: %.1f", pressaoInicial);
      saidaRasp.enviaTexto(_charBuf);
    }
    do {
      valvulaFluxoArVP1(0);
      valvulaFluxoOxVP2(0);  
      _leSensoresPorTempo(100);
    } while (!passouTempoMillis(tInicioEspera, 10000));
    pressaoFinal = statusSensores.valorPressao[3];
    
    *volumeVazado = complacenciaCalculada * (pressaoInicial - pressaoFinal);
    if (verb) {
      sprintf(_charBuf, "    Pressao final: %.1f", pressaoFinal);
      saidaRasp.enviaTexto(_charBuf);
      sprintf(_charBuf, "    Volume vazado: %.1f", *volumeVazado*1000);
      saidaRasp.enviaTexto(_charBuf);
    }
    desligaValvulas();
    return true;
  }
  else {
    return false;
  }
}

bool Ensaio::_ajustaFluxoBase(bool usarSensorFluxoCalib) {
  bool verb = true;

  int pwm = 0;
  float fluxo = 0; 

  if (verb) {
    saidaRasp.enviaTexto(" ");
    saidaRasp.enviaTexto("Buscando fluxo positivo");
    saidaRasp.enviaTexto("    [pwm]  [fluxo]");
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
      saidaRasp.enviaTexto(_charBuf);
    }
  } while (fluxo < 3.0);

  if (verb) {
    saidaRasp.enviaTexto(" ");
    saidaRasp.enviaTexto("Ajustando pwm para fluxo base");
    saidaRasp.enviaTexto("    [pwm]  [fluxo]");
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
      saidaRasp.enviaTexto(_charBuf);
    }
  } while (fluxo < FLUXO_BASE);

  _pwmFluxoBase = pwm;

  return true;
}

bool Ensaio::_ajustaPilotoBase() {
  bool verb = true;

  int pwm = 0;

  if (verb) {
    saidaRasp.enviaTexto(" ");
    saidaRasp.enviaTexto("Buscando pressurizacao positiva do sistema");
    saidaRasp.enviaTexto("    [pwmPil]  [presPil]  [presSis]");
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
      saidaRasp.enviaTexto(_charBuf);
    }
  } while (statusSensores.valorPressao[4] < 1.0);

  if (verb) {
    saidaRasp.enviaTexto(" ");
    saidaRasp.enviaTexto("Ajustando piloto para pressurizar sistema");
    saidaRasp.enviaTexto("    [pwmPil]  [presPil]  [presSis]");
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
      saidaRasp.enviaTexto(_charBuf);
    }
  } while (statusSensores.valorPressao[4] < 5.0);

  _pwmPilotoBase = pwm;

  valvulaFluxoArVP1(0);
  return true;
}

void Ensaio::_leSensoresPorTempo(int periodo) {
#ifdef SIMULADOR
  _simulaSensores();
  delay(periodo/20);
#else
  
  unsigned long tInicial = millis();
  while (!passouTempoMillis(tInicial, periodo)) {
    sensores.leSensores();
    delay(10);
  }

#endif
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

void Ensaio::_simulaSensores() {
  statusSensores.valorFluxo[1] = mapFloat(pwmValvAr, 1500, 3500, 0, 75);
  statusSensores.valorFluxo[2] = mapFloat(pwmValvO2, 1200, 3200, 0, 60);
  statusSensores.valorPressao[5] = mapFloat(pwmValvPil, 1000, 2500, 0, 110);
  
  statusSensores.valorFluxo[4] = statusSensores.valorFluxo[1] + statusSensores.valorFluxo[2];
  statusSensores.fluxoCalibracao = statusSensores.valorFluxo[4]; 

  statusSensores.adcFluxo[1] = mapFloat(statusSensores.valorFluxo[1], 0, 75, 1000, 50000);
  statusSensores.adcFluxo[2] = mapFloat(statusSensores.valorFluxo[2], 0, 60, 500, 20000);
  statusSensores.adcFluxo[4] = mapFloat(statusSensores.valorFluxo[4], 0, 135, 15000, 70000);

  statusSensores.valorPressao[3] = mapFloat(statusSensores.valorPressao[5], 10.0, 60.0, 0.0, 70.0);
  statusSensores.valorPressao[4] = mapFloat(statusSensores.valorPressao[5], 10.0, 60.0, 0.0, 70.0);
}
