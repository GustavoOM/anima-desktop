#include "_global.h"

//#define VERBOSE

void Alarmes::setup() {
  memset(&_param, 0, sizeof(_param));
  memset(&_status, 0, sizeof(_status));
} 

void Alarmes::loop() {
  _recarregaParametros();
  _verificaVentilacao();
  _verificaTransicaoFase();
  _verificaPrimeirosCincoSegundos();
  _verificaPrimeiroMinuto();

  _checaBateria();
  _checaFalhaComunicacao();
  _checaBaixaPressaoArGrave();
  _checaBaixaPressaoAr();
  _checaBaixaPressaoO2Grave();
  _checaBaixaPressaoO2();  
  _checaFonteEletrica();
  _checaComandoInvalido();

  if (_status.isEmVentilacao && !_status.isPrimeirosCincoSegundos) {
    _checaApneia();
    _checaDesconexaoCircuito();
    _checaVazamentoCircuito();
    _checaObstrucaoPorVolume();
    _checaObstrucaoPorPico();
    _checaObstrucaoPorTempo();
    _checaPeepAlta();
    _checaAutoPeep();
    _checaPressaoInsAlta();
    _checaVolumeCorrenteAlto();
    _checaFiO2Alta();
    _checaFiO2Baixa();
    _checaFrequenciaAlta();
    _checaPressaoInsBaixa();
    _checaVolumeCorrenteBaixo();
    _checaVolumeMinutoAlto();
    _checaVolumeMinutoBaixo();
    _checaInversaoIE();
    _checaFalhaConversores();
    _checaFalhaPressaoPiloto();
  }

#ifdef VERBOSE
  SerialUSB.print("alarme ativo /");
  for (int i=0; i<32; i++) {
    if (_status.alarmesAtivos[i]) {
      SerialUSB.print(i);
      SerialUSB.print("/");
    }
  }
  SerialUSB.println();
#endif
}

unsigned int Alarmes::geraNumeroEnvioRasp() {
  unsigned int bitsAlarmes = 0;
  for (int i=0; i<32; i++) {
    if (_status.alarmesAtivos[i]) {
      bitsAlarmes |= (1<<i);
    }
  }
  return bitsAlarmes;
}

void Alarmes::_recarregaParametros() {
  _param = entradaRasp.dadosRecebidosLoop.limitesAlarmes;
}

void Alarmes::_checaApneia() {
  if (estrategiaTop.params.modo == MODO_PSV || estrategiaTop.isModoBackupAtivo()) { 
    if (!estrategiaTop.isModoBackupAtivo()) {
      bool cond1 = statusCiclo.tInicioExp > 0;      
      bool cond2 = passouTempoMillis(statusCiclo.tInicioExp, estrategiaTop.params.tApneia);
 
      if(cond1 && cond2) {
        _status.alarmesAtivos[0] = true;
        _status.alarmesAtivos[1] = true;
  
        estrategiaTop.notifyImediatoModoBackup();
      }
  
    } else {
      if (statusPaciente.nTriggerEspConsecut>= 3) {
        estrategiaTop.notifyDesligaModoBackupNoProximoCiclo();
        _status.alarmesAtivos[0] = false;
        _status.alarmesAtivos[1] = false;
      }
    }   
  }
  else {
    _status.alarmesAtivos[0] = false;
    _status.alarmesAtivos[1] = false;
  }
}

void Alarmes::_checaBateria() {
  float vBat = statusSensores.tensaoBateria;
  if (statusSensores.tensaoFonte < MIN_TENSAO_FONTE && vBat < TENSAO_BATERIA_75) {
    if (TENSAO_BATERIA_50 * 1.01 < vBat) {
      _status.alarmesAtivos[2] = true;
      _status.alarmesAtivos[3] = false;
      _status.alarmesAtivos[4] = false;
    } else if (TENSAO_BATERIA_25 * 1.01 < vBat && vBat < TENSAO_BATERIA_50) {
      _status.alarmesAtivos[2] = false;
      _status.alarmesAtivos[3] = true;
      _status.alarmesAtivos[4] = false;
    } else if (vBat < TENSAO_BATERIA_25) {
      _status.alarmesAtivos[2] = false;
      _status.alarmesAtivos[3] = false;
      _status.alarmesAtivos[4] = true;
    } // valores dentro da faixa de 1% de tolerância apenas mantém o estado anterior,
      // impedindo que haja oscilação do estado entre dois alarmes distintos
  } else {
    _status.alarmesAtivos[2] = false;
    _status.alarmesAtivos[3] = false;
    _status.alarmesAtivos[4] = false;
  }
}

void Alarmes::_checaDesconexaoCircuito() {
  if (_status.isInicioIns) {
    float razaoVolume = statusPaciente.vTidalIns/statusPaciente.vTidalExp;
    
    bool cond1 = valvSegAberta == false;
    bool cond2 = (razaoVolume < 1/1.3) || (razaoVolume > 1.3);
    bool cond3 = statusPaciente.peep50ms < 2.0;

    if(cond1) {
      if (cond2 && cond3) {
        if(_status.contadorDesconexaoCircuito < 0)
          _status.contadorDesconexaoCircuito = 0;
        _status.contadorDesconexaoCircuito++;
      }
      else {
        if(_status.contadorDesconexaoCircuito > 0)
          _status.contadorDesconexaoCircuito = 0;
        _status.contadorDesconexaoCircuito--;
      }

      if (_status.contadorDesconexaoCircuito >= 3) {
        _status.alarmesAtivos[5] = true;
      }
      if (_status.contadorDesconexaoCircuito <= -3) {
        _status.alarmesAtivos[5] = false;
      }
    }
  }
}

void Alarmes::_checaVazamentoCircuito() {
  if (_status.isInicioIns) {
    float razaoVolume = statusPaciente.vTidalIns/statusPaciente.vTidalExp;
    
    bool cond1 = valvSegAberta == false;
    bool cond2 = (razaoVolume < 1.0/2) || (razaoVolume > 2);
    bool cond3 = statusPaciente.peep50ms < estrategiaTop.params.peep &&
                statusPaciente.peep50ms > 2.0;
    
    if (cond1) {
      if (cond2 && cond3) {
        if (_status.contadorVazamentoCircuito < 0) 
          _status.contadorVazamentoCircuito = 0;
          _status.contadorVazamentoCircuito++;
      } 
      else {
        if (_status.contadorVazamentoCircuito > 0)
          _status.contadorVazamentoCircuito = 0;
          _status.contadorVazamentoCircuito--;
      }

      if (_status.contadorVazamentoCircuito >= 3) {
        _status.alarmesAtivos[6] = true;
      }
      if (_status.contadorVazamentoCircuito <= -3) {
        _status.alarmesAtivos[6] = false;
      }
    }
  }
} 

void Alarmes::_checaFalhaComunicacao() {
  // checado na API
  _status.alarmesAtivos[7] = false;
} 

void Alarmes::_checaObstrucaoPorVolume() {
  if (_status.isInicioIns) {
    float razaoVolume = statusPaciente.vTidalIns/statusPaciente.vTidalExp;
    bool cond1 = (razaoVolume < 1/2) || (razaoVolume > 2);

    float deltaPressao = statusCiclo.pPlateau50ms - estrategiaTop.params.peep;
    float deltaPeep = statusPaciente.peep50ms - estrategiaTop.params.peep;
    bool cond2 = deltaPeep > 0.3 * deltaPressao;

    if (cond1 && cond2) {
      if (_status.contadorObstrucaoPorVolume < 3) {
        _status.contadorObstrucaoPorVolume++;
      }
    } else {
      if (_status.contadorObstrucaoPorVolume > 0) {
        _status.contadorObstrucaoPorVolume--;
      }
    }

    if (_status.contadorObstrucaoPorVolume >= 3) {
      _status.alarmesAtivos[8] = true;
    }
    if (_status.contadorObstrucaoPorVolume <= 0) {
      _status.alarmesAtivos[8] = false;
    }
  }
}

void Alarmes::_checaObstrucaoPorPico() {
  if (statusCiclo.pPicoIns > _param.pInsMax + 10.0) {
    _status.alarmesAtivos[9] = true;
    valvulaSeguranca(true);
  }

  if (_status.isInicioIns) {
    _status.alarmesAtivos[9] = false;
    valvulaSeguranca(false);
  }
}

void Alarmes::_checaObstrucaoPorTempo() {
  unsigned long deltaTempo = 600;
  float deltaPressao = 2.0;

  if (_status.isInicioExp) {
    _status.inicioObstrucaoPorTempo = millis();
    _status.analiseObstrucaoPorTempo = true;
  }

  float setPressao;
  switch (estrategiaTop.params.modo) {
    case MODO_VCV:
      setPressao = estrategiaTop.params.pInsLimite; break;
    case MODO_PCV:
      setPressao = estrategiaTop.params.pCon; break;
    case MODO_PSV:
      setPressao = estrategiaTop.params.pSup; break;
  }

  if (_status.analiseObstrucaoPorTempo) {
    if (passouTempoMillis(_status.inicioObstrucaoPorTempo, deltaTempo)) {
      if (statusCiclo.pPlateau50ms > setPressao) {
        if (medidas.pressaoIns > (statusCiclo.pPlateau50ms - deltaPressao)) {
          _status.alarmesAtivos[10] = true;
          valvulaSeguranca(true);
        }
      }
      _status.analiseObstrucaoPorTempo = false;
    }
  }

  if (_status.isInicioIns) {
    _status.alarmesAtivos[10] = false;
    valvulaSeguranca(false);
  }
}

void Alarmes::_checaPeepAlta() {
  if (_param.pPeepMax == 0) {
    _status.alarmesAtivos[11] = false;
  }
  else {
    if (_status.isInicioIns) {
      if (statusPaciente.peep50ms > _param.pPeepMax) {
        _status.alarmesAtivos[11] = true;
      } else {
        _status.alarmesAtivos[11] = false;
      }
    }
  }
}

void Alarmes::_checaAutoPeep() {
  if (_status.isInicioIns) {
    float razaoVolume = statusPaciente.vTidalIns/statusPaciente.vTidalExp;
    bool cond1 = (razaoVolume < 1/1.2) || (razaoVolume > 1.2);

    float deltaPeep = 2.0;
    bool cond2 = statusPaciente.peep50ms > estrategiaTop.params.peep + deltaPeep;

    if (cond1 && cond2) {
      if (_status.contadorAutoPeep < 3) {
        _status.contadorAutoPeep++;
      }
    } else {
      if (_status.contadorAutoPeep > 0) {
        _status.contadorAutoPeep--;
      }
    }

    if (_status.contadorAutoPeep >= 3) {
      _status.alarmesAtivos[12] = true;
    }
    if (_status.contadorAutoPeep <= 0) {
      _status.alarmesAtivos[12] = false;
    }
  }
}

void Alarmes::_checaPressaoInsAlta() {
  if (statusCiclo.fase == FASE_INS) {
    if (_status.isInicioIns) {
      if (_status.cicloOkPressaoInsAlta) {
        _status.alarmesAtivos[13] = false;
      }
      _status.cicloOkPressaoInsAlta = true;
    }

    if (statusCiclo.pPicoIns > _param.pInsMax) {
      _status.cicloOkPressaoInsAlta = false;
      _status.alarmesAtivos[13] = true;
      if (estrategiaTop.params.modo == MODO_VCV) {
        controle.cortaFluxoIns();
      }
      if (estrategiaTop.params.modo == MODO_PCV || estrategiaTop.params.modo == MODO_PSV) {
        estrategiaTop.notifySwitchToExp();
      }
    }
  }
}

void Alarmes::_checaVolumeCorrenteAlto() {
  if (_status.isInicioExp) {
    if (statusCiclo.vTidal > _param.vTidalInsMax) {
      _status.alarmesAtivos[14] = true;
    } else {
      _status.alarmesAtivos[14] = false;
    }
  }
}

void Alarmes::_checaBaixaPressaoArGrave() {
  bool cond1 = statusSensores.valorPressao[1] < PRESSAO_MIN_PSI_REG;
  bool cond2 = estrategiaTop.params.fio2 != 1.00;

  if (cond1 && cond2) {
    _status.alarmesAtivos[15] = true;
    estrategiaTop.notifyFiO2Fixo100();
  } else {
    _status.alarmesAtivos[15] = false;
    estrategiaTop.desativaFiO2Fixo100();
  }
}

void Alarmes::_checaBaixaPressaoAr() {
  bool cond1 = statusSensores.valorPressao[1] < PRESSAO_MIN_PSI_REG;
  bool cond2 = estrategiaTop.params.fio2 == 1.00;

  if (cond1 && cond2) {
    _status.alarmesAtivos[16] = true;
  } else {
    _status.alarmesAtivos[16] = false;
  }
}

void Alarmes::_checaBaixaPressaoO2Grave() {
  bool cond1 = statusSensores.valorPressao[2] < PRESSAO_MIN_PSI_REG;
  bool cond2 = estrategiaTop.params.fio2 > 0.21;

  if (cond1 && cond2) {
    _status.alarmesAtivos[17] = true;
    estrategiaTop.notifyFiO2Fixo21();
  } else {
    _status.alarmesAtivos[17] = false;
    estrategiaTop.desativaFiO2Fixo21();
  }
}

void Alarmes::_checaBaixaPressaoO2() {
  bool cond1 = statusSensores.valorPressao[2] < PRESSAO_MIN_PSI_REG;
  bool cond2 = estrategiaTop.params.fio2 <= 0.21;

  if (cond1 && cond2) {
    _status.alarmesAtivos[18] = true;
  } else {
    _status.alarmesAtivos[18] = false;
  }
}

void Alarmes::_checaFiO2Alta() {
  if (statusPaciente.fio2 > _param.maxFio2) {
    _status.alarmesAtivos[19] = true;
  } else {
    _status.alarmesAtivos[19] = false;
  }
}

void Alarmes::_checaFiO2Baixa() {
  if (statusPaciente.fio2 < _param.minFio2) {
    _status.alarmesAtivos[20] = true;
  } else {
    _status.alarmesAtivos[20] = false;
  }
}

void Alarmes::_checaFonteEletrica() {
  if (statusSensores.tensaoFonte < MIN_TENSAO_FONTE) {
    _status.alarmesAtivos[21] = true;
  } else {
    _status.alarmesAtivos[21] = false;
  }
}

void Alarmes::_checaFrequenciaAlta() {
  if (_param.freqMax == 0) {
    _status.alarmesAtivos[22] = false;
  }
  else {
    if (_status.isInicioIns) {
      if (statusPaciente.freq > _param.freqMax) {
        if (_status.contadorFrequenciaAlta < 3) {
          _status.contadorFrequenciaAlta++;
        }
      } else {
        _status.contadorFrequenciaAlta = 0;
      }
  
      if (_status.contadorFrequenciaAlta >= 3) {
        _status.alarmesAtivos[22] = true;
      } else {
        _status.alarmesAtivos[22] = false;
      }
    }
  }
}

void Alarmes::_checaPressaoInsBaixa() {
  if (_param.pInsPlatMin == 0) {
    _status.alarmesAtivos[23] = false;
  }
  else {
    if (_status.isInicioExp) {
      if (statusCiclo.pPlateau50ms < _param.pInsPlatMin) {
        _status.alarmesAtivos[23] = true;
      } else {
        _status.alarmesAtivos[23] = false;
      }
    }
  }
}

void Alarmes::_checaVolumeCorrenteBaixo() {
  if (_param.vTidalInsMin == 0) {
    _status.alarmesAtivos[24] = false;
  }
  else {
    if (_status.isInicioExp) {
      if (statusPaciente.vTidalIns < _param.vTidalInsMin) {
        _status.alarmesAtivos[24] = true;
      } else {
        _status.alarmesAtivos[24] = false;
      }
    }
  }
}

void Alarmes::_checaVolumeMinutoAlto() {
  if (_param.vMinuMax == 0) {
    _status.alarmesAtivos[25] = false;
  }
  else {
    if (!_status.isPrimeiroMinuto) {
      if (_status.isInicioIns) {
        if(statusPaciente.vMinIns > _param.vMinuMax) {
          if(_status.contadorvMinAlto < 0)
            _status.contadorvMinAlto = 0;
          _status.contadorvMinAlto++;
        }
        else {
          if(_status.contadorvMinAlto > 0)
            _status.contadorvMinAlto = 0;
          _status.contadorvMinAlto--;
        }
  
        if (_status.contadorvMinAlto >= 3) {
          _status.alarmesAtivos[25] = true;
        }
        if (_status.contadorvMinAlto <= -3) {
          _status.alarmesAtivos[25] = false;
        }
      }
    }
  }
}

void Alarmes::_checaVolumeMinutoBaixo() {
  if (_param.vMinuMin == 0) {
    _status.alarmesAtivos[26] = false;
  }
  else {
    if (!_status.isPrimeiroMinuto) {
      if (_status.isInicioIns) {
        if(statusPaciente.vMinIns < _param.vMinuMin) {
          if(_status.contadorvMinBaixo < 0)
            _status.contadorvMinBaixo = 0;
          _status.contadorvMinBaixo++;
        }
        else {
          if(_status.contadorvMinBaixo > 0)
            _status.contadorvMinBaixo = 0;
          _status.contadorvMinBaixo--;
        }
  
        if (_status.contadorvMinBaixo >= 3) {
          _status.alarmesAtivos[26] = true;
        }
        if (_status.contadorvMinBaixo <= -3) {
          _status.alarmesAtivos[26] = false;
        }
      }
    }
  }
}

void Alarmes::_checaInversaoIE() {
  if (_status.isInicioIns) {
    bool cond1 = statusPaciente.relTempIE < 1.0;
    // tResp - tIns - tPausaIns = tExp >= tIns
    bool cond2 = 60000/estrategiaTop.params.freq - estrategiaTop.params.tIns - estrategiaTop.params.tPausaIns >= estrategiaTop.params.tIns;
    
    if(cond1 && cond2) {
      if(_status.contadorInversaoIE < 0)
        _status.contadorInversaoIE = 0;
      _status.contadorInversaoIE++;
    }
    else {
      if(_status.contadorInversaoIE > 0)
        _status.contadorInversaoIE = 0;
      _status.contadorInversaoIE--;
    }

    if (_status.contadorInversaoIE >= 3) {
      _status.alarmesAtivos[27] = true;
    }
    if (_status.contadorInversaoIE <= -3) {
      _status.alarmesAtivos[27] = false;
    }
  }
}

void Alarmes::_checaFalhaConversores() {
#ifdef SIMULADOR
  _status.alarmesAtivos[28] = false;
  return;
#endif

  if (statusSensores.erroTimeoutFluxo[1] || statusSensores.adcFluxo[1] < -40000) {
    if (_status.contadorConvFluxoAr < 3) {
      _status.contadorConvFluxoAr++;
    }
    if (_status.contadorConvFluxoAr >= 3) {
      statusSensores.falhaConversorFluxo[1] = true;
      estrategiaTop.notifyFiO2Fixo100();
    }
    _status.alarmesAtivos[28] |= true;
  } else {
    _status.contadorConvFluxoAr = 0;
  }

  if (statusSensores.erroTimeoutFluxo[2] || statusSensores.adcFluxo[2] < -40000) {
    if (_status.contadorConvFluxoO2 < 3) {
      _status.contadorConvFluxoO2++;
    }
    if (_status.contadorConvFluxoO2 >= 3) {
      statusSensores.falhaConversorFluxo[2] = true;
      estrategiaTop.notifyFiO2Fixo21();
    }
    _status.alarmesAtivos[28] |= true;
  } else {
    _status.contadorConvFluxoO2 = 0;
  }

  if (statusSensores.erroTimeoutFluxo[4] || statusSensores.adcFluxo[4] < -40000) {
    if (_status.contadorConvFluxoEx < 3) {
      _status.contadorConvFluxoEx++;
    }
    if (_status.contadorConvFluxoEx >= 3) {
      statusSensores.falhaConversorFluxo[4] = true;
      estrategiaTop.notifyStopVentilacao(true);
      valvulaSeguranca(true);
    }
    _status.alarmesAtivos[28] |= true;
  } else {
    _status.contadorConvFluxoEx = 0;
  }

  if (statusSensores.erroTimeoutPressao[3] || statusSensores.adcPressao[3] < -40000) {
    if (_status.contadorConvPressaoIns < 3) {
      _status.contadorConvPressaoIns++;
    }
    if (_status.contadorConvPressaoIns >= 3) {
      statusSensores.falhaConversorPressao[3] = true;
      estrategiaTop.notifyStopVentilacao(true);
      valvulaSeguranca(true);
    }
    _status.alarmesAtivos[28] |= true;
  } else {
    _status.contadorConvPressaoIns = 0;
  }

  if (statusSensores.erroTimeoutPressao[4] || statusSensores.adcPressao[4] < -40000) {
    if (_status.contadorConvPressaoExp < 3) {
      _status.contadorConvPressaoExp++;
    }
    if (_status.contadorConvPressaoExp >= 3) {
      statusSensores.falhaConversorPressao[4] = true;
    }
    _status.alarmesAtivos[28] |= true;
  } else {
    _status.contadorConvPressaoExp = 0;
  }

  if (statusSensores.erroTimeoutPressao[5] || statusSensores.adcPressao[5] < -40000) {
    if (_status.contadorConvPressaoPil < 3) {
      _status.contadorConvPressaoPil++;
    }
    if (_status.contadorConvPressaoPil >= 3) {
      statusSensores.falhaConversorPressao[5] = true;
      controle.setFalhaSensorPiloto(true);
    }
    _status.alarmesAtivos[28] |= true;
  } else {
    _status.contadorConvPressaoPil = 0;
  }
} 

void Alarmes::_checaComandoInvalido() {
  _status.alarmesAtivos[29] = entradaRasp.haErroComandoInvalido();
}

void Alarmes::_checaFalhaPressaoPiloto() {
#ifdef SIMULADOR
  _status.alarmesAtivos[30] = false;
  return;
#endif

  bool falhaSensorPressaoPiloto = statusSensores.valorPressao[5] < MINIMO_PRESSAO_PILOTO;
  controle.setFalhaSensorPiloto(falhaSensorPressaoPiloto || statusSensores.falhaConversorPressao[5]);
  _status.alarmesAtivos[30] = falhaSensorPressaoPiloto;
} 

void Alarmes::_verificaTransicaoFase() {
  _status.isInicioIns = (_status.faseAnterior != FASE_INS) &
                                 (statusCiclo.fase == FASE_INS);
  _status.isInicioExp = (_status.faseAnterior != FASE_EXP) &
                                 (statusCiclo.fase == FASE_EXP);
  _status.faseAnterior = statusCiclo.fase;
}

void Alarmes::_verificaVentilacao() {
  _status.isEmVentilacao = statusPaciente.tInicioPaciente != 0;
}

void Alarmes::_verificaPrimeirosCincoSegundos() {
  unsigned long t = millis();
  _status.isPrimeirosCincoSegundos = t - statusPaciente.tInicioPaciente < 5000;
}

void Alarmes::_verificaPrimeiroMinuto() {
  unsigned long t = millis();
  _status.isPrimeiroMinuto = t - statusPaciente.tInicioPaciente < 60000;
}