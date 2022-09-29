#include "_global.h"

//#define VERBOSE

Autotestes::Autotestes() { }

void Autotestes::realizaAutotestes() {
  memset(&dados, 0, sizeof(dados));
  bool falhou = false;
  for (int teste = TESTE_COMUNICACAO; teste <= TESTE_CIRCUITO_PACIENTE; teste++) {
    long falha = _executaTeste((AutotestesEnum) teste);
    if (falha != 0) {
      falhou = true;
      Serial.print("  Teste "); Serial.print(teste); Serial.println(" falhou!");
    }
  }
  if (!falhou) {
    Serial.println("  Nenhum teste falhou!");
  }
}

long Autotestes::_executaTeste(AutotestesEnum teste) {
  switch (teste) {
    case TESTE_COMUNICACAO:
      return _testeComunicacao();
    case TESTE_ALIMENTACAO_ELETRICA:
      return _testeAlimentacaoEletrica();
    case TESTE_BATERIA:
      return _testeBateria();
    case TESTE_ALIMENTACAO_AR:
      return _testeAlimentacaoAr();
    case TESTE_ALIMENTACAO_O2:
      return _testeAlimentacaoO2();
    case TESTE_CONVERSORES:
      return _testeConversores();
    case TESTE_SENSORES_PRESSAO:
      return _testeSensoresPressao();
    case TESTE_SENSORES_FLUXO:
      return _testeSensoresFluxo();
    case TESTE_VALVULA_AR:
      return _testeValvulaAr();
    case TESTE_VALVULA_O2:
      return _testeValvulaO2();
    case TESTE_VALVULA_PILOTO:
      return _testeValvulaPiloto();
    case TESTE_VALVULA_EXALACAO:
      return _testeValvulaExalacao();
    case TESTE_VALVULA_SEGURANCA:
      return _testeValvulaSeguranca();
    case TESTE_CIRCUITO_PACIENTE:
      return _testeCircuitoPaciente();
  }
}

long Autotestes::_testeComunicacao() {
  return 0;
}

long Autotestes::_testeAlimentacaoEletrica() {
  ensaio.simulaSensores();
  if (statusSensores.tensaoFonte < MIN_TENSAO_FONTE) {
    return 1;
  } else {
    return 0;
  }
}


long Autotestes::_testeBateria() {
  ensaio.simulaSensores();
  if (statusSensores.tensaoBateria < TENSAO_BATERIA_25) {
    return 1;
  } else {
    return 0;
  }
}


long Autotestes::_testeAlimentacaoAr() {
  ensaio.simulaSensores();
  bool cond1 = statusSensores.valorPressao[1] < PRESSAO_MIN_PSI_REG;
  bool cond2 = PRESSAO_MAX_PSI_REG < statusSensores.valorPressao[1];
  if (cond1 || cond2) {
    return 1;
  } else {
    return 0;
  }
}


long Autotestes::_testeAlimentacaoO2() {
  ensaio.simulaSensores();
  bool cond1 = statusSensores.valorPressao[2] < PRESSAO_MIN_PSI_REG;
  bool cond2 = PRESSAO_MAX_PSI_REG < statusSensores.valorPressao[2];
  if (cond1 || cond2) {
    return 1;
  } else {
    return 0;
  }
}

long Autotestes::_testeConversores() {
  ensaio.simulaSensores();

  long falha = 0;
  for (int i=3; i<=5; i++) {
    if (statusSensores.erroTimeoutPressao[i]) {
      falha |= 1 << (i-3);
    }
  }
  for (int i=1; i<=4; i++) {
    if (statusSensores.erroTimeoutFluxo[i]) {
      falha |= 1 << (i+2);
    }
  }
  return falha;
}

long Autotestes::_testeSensoresPressao() {
  ensaio.simulaSensores();

  long falha = 0;
  for (int i=3; i<=5; i++) {
    if (statusSensores.erroOffsetPressao[i]) {
      falha |= 1 << (i-3);
    }
  }
  return falha;
}

long Autotestes::_testeSensoresFluxo() {
  ensaio.simulaSensores();

  long falha = 0;
  for (int i=1; i<=4; i++) {
    if (statusSensores.erroOffsetFluxo[i]) {
      falha |= 1 << (i-1);
    }
  }
  return falha;
}


long Autotestes::_testeValvulaAr() {
  float dadosValvulaAr[3];
  if (ensaio.ensaiaValvulaAOP(VALV_AR, dadosValvulaAr)) {
    dados.pwmAbertura[VALV_AR] = dadosValvulaAr[0];
    dados.pwmFechamento[VALV_AR] = dadosValvulaAr[1];
    dados.saidaPico[VALV_AR] = dadosValvulaAr[2];

    if (_verificaFalhaValvula(VALV_AR)) {
      return 1;
    } else {
      return 0;
    }
  }
  else {
    return 1;
  }
}


long Autotestes::_testeValvulaO2() {
  float dadosValvulaO2[3];
  if (ensaio.ensaiaValvulaAOP(VALV_O2, dadosValvulaO2)) {
    dados.pwmAbertura[VALV_O2] = dadosValvulaO2[0];
    dados.pwmFechamento[VALV_O2] = dadosValvulaO2[1];
    dados.saidaPico[VALV_O2] = dadosValvulaO2[2];

    if (_verificaFalhaValvula(VALV_O2)) {
      return 1;
    } else {
      return 0;
    }
  }
  else {
    return 1;
  }
}


long Autotestes::_testeValvulaPiloto() {
  float dadosValvulaPiloto[3];
  if (ensaio.ensaiaValvulaAOP(VALV_PILOTO, dadosValvulaPiloto)) {
    dados.pwmAbertura[VALV_PILOTO] = dadosValvulaPiloto[0];
    dados.pwmFechamento[VALV_PILOTO] = dadosValvulaPiloto[1];
    dados.saidaPico[VALV_PILOTO] = dadosValvulaPiloto[2];

   if (_verificaFalhaValvula(VALV_PILOTO)) {
     return 1;
   } else {
     return 0;
   }
  }
  else {
    return 1;
  }
}


bool Autotestes::_verificaFalhaValvula(ValvulasAOPEnum valv) {
  bool cond1 = dados.pwmAbertura[valv] < paramValvulasAOP.pwmAberturaMin[valv];
  bool cond2 = paramValvulasAOP.pwmAberturaMax[valv] < dados.pwmAbertura[valv];
  bool cond3 = dados.pwmFechamento[valv] < paramValvulasAOP.pwmFechamentoMin[valv];
  bool cond4 = paramValvulasAOP.pwmFechamentoMax[valv] < paramValvulasAOP.pwmFechamentoMax[valv];
  bool cond5 = dados.saidaPico[valv] < paramValvulasAOP.saidaPicoMin[valv];

  return cond1 || cond2 || cond3 || cond4 || cond5;
}


long Autotestes::_testeValvulaExalacao() {
  bool falha = false;
  if (ensaio.ensaiaValvulaExalacao(
    dados.curvaExalacaoPiloto,
    dados.curvaExalacaoSistema,
    paramValvulaExalacao.limiaresSistema)) {
    
    float pilotoNoLimiarSistema;
    for (int i=0; i<9; i++) {
      pilotoNoLimiarSistema = mapCurvaFloat(
        paramValvulaExalacao.limiaresSistema[i],
        dados.curvaExalacaoSistema,
        dados.curvaExalacaoPiloto,
        11);

      bool cond1 = pilotoNoLimiarSistema < paramValvulaExalacao.minPilotoNoLimiarSistema[i];
      bool cond2 = paramValvulaExalacao.maxPilotoNoLimiarSistema[i] < pilotoNoLimiarSistema;
      falha |= cond1 || cond2;
    }

    bool cond = dados.curvaExalacaoSistema[10] < paramValvulaExalacao.minPressaoPicoSistema;
    falha |= cond;
  }
  else {
    falha = true;
  }

  if (falha) {
    return 1;
  } else {
    return 0;
  }
}


long Autotestes::_testeCircuitoPaciente() {
  float volumeVazado;
  if (ensaio.ensaiaCircuitoPaciente(&volumeVazado)) {
    if (volumeVazado > VOLUME_VAZADO_ACEITAVEL) {
      return 1;
    } else {
      return 0;
    }
  } else {
    return 1;
  }
}


long Autotestes::_testeValvulaSeguranca() {
  if (ensaio.ensaiaValvulaSeguranca(&dados.tempoValvulaSeguranca)) {

    if (paramValvulasAOP.tempoMaxValvSeguranca < dados.tempoValvulaSeguranca) {
      return 1;
    } else {
      return 0;
    }
  } else {
    return 1;
  }
}
