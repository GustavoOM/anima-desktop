#include "_global.h"

#ifdef CALIBRA_AUTOMATICA

#define TOLER_ARBITRARIA_ZERO_SENS_FLUXO (0.005)
#define TOLER_ARBITRARIA_ZERO_SENS_PRESSAO (0.005)
#define TOLER_ARBITRARIA_PWM_ABERTURA (0.10)
#define TOLER_ARBITRARIA_PWM_FECHAMENTO (0.10)
#define TOLER_ARBITRARIA_SAIDA_PICO (0.10)
#define TOLER_ARBITRARIA_CURVA_EXALACAO (0.02)
#define TOLER_ARBITRARIA_VALVULA_SEGURANCA (0.1)

ParametrosSensoresPressao dadosCalibSensoresPressao;
ParametrosSensorFluxo dadosCalibSensorFluxoAr;
ParametrosSensorFluxo dadosCalibSensorFluxoO2;
ParametrosSensorFluxo dadosCalibSensorFluxoEx;
ParametrosValvulasAOP dadosCalibValvulasAOP;
ParametrosValvulaExalacao dadosCalibValvulaExalacao;

float limiaresValvulaExalacao[9] = {2, 4, 6, 9, 12, 18, 25, 35, 50};
char buffer[100];

void setupCalibAutomatica() {
  ensaio.setup();
  saidaRasp.setup();
  setupMaquina();
  esperaInicioCalibAutomatica();
}

void loopCalibAutomatica() {
  
  calibraSensoresPressao();
  calibraSensoresFluxo();
  calibraAutotestesValvulasAOP();
  calibraAutotestesValvulaExalacao();
#ifndef SIMULADOR
  calibraAutotestesValvulaSeguranca();
#endif
  enviaDadosParaRasp();

  while (true) {
    desligaValvulas();
  }
}

void esperaInicioCalibAutomatica() {
  while (true) {
    String texto = entradaRasp.esperaRecebeTexto();
    if (texto.equals("!(inicia)!")) {
      saidaRasp.enviaTexto("!(inicia)!");
      break;
    } else {
      saidaRasp.enviaTexto("!(erro)!");
    }
  }
  delay(500);
}

void calibraSensoresPressao() {
  saidaRasp.enviaTexto("====================================================================");
  saidaRasp.enviaTexto("SENSORES DE PRESSAO");
  saidaRasp.enviaTexto("--------------------------------------------------------------------");
  saidaRasp.enviaTexto(" ");

  sensores.zeraSensoresPressao();
  saidaRasp.enviaTexto("Coletando valores de adc no zero");
  delay(500);
  for (int i=0; i<3; i++) {
    float minAdcNoZero = statusSensores.offsetPressao[i+3] - TOLER_ARBITRARIA_ZERO_SENS_PRESSAO * 80000;
    dadosCalibSensoresPressao.minAdcNoZero[i] = saturaFloat(minAdcNoZero, -40000, 40000);
    float maxAdcNoZero = statusSensores.offsetPressao[i+3] + TOLER_ARBITRARIA_ZERO_SENS_PRESSAO * 80000;
    dadosCalibSensoresPressao.maxAdcNoZero[i] = saturaFloat(maxAdcNoZero, -40000, 40000);
  }

  for (int i=0; i<3; i++) {
    sprintf(buffer, "    Adc no zero P%d: %.0f", i+3, statusSensores.offsetPressao[i+3]);
    saidaRasp.enviaTexto(buffer);
  }
  
  saidaRasp.enviaTexto(" ");
}

void calibraSensoresFluxo() {

  calibraSensorFluxo(SENS_FLUX_AR, &dadosCalibSensorFluxoAr);
  carregaParametrosSensorFluxo(&dadosCalibSensorFluxoAr, &paramSensorFluxoAr);

  calibraSensorFluxo(SENS_FLUX_O2, &dadosCalibSensorFluxoO2);
  carregaParametrosSensorFluxo(&dadosCalibSensorFluxoO2, &paramSensorFluxoO2);

  calibraSensorFluxo(SENS_FLUX_EX, &dadosCalibSensorFluxoEx);
  carregaParametrosSensorFluxo(&dadosCalibSensorFluxoEx, &paramSensorFluxoEx);
}

void calibraSensorFluxo(SensFluxEnum sensFlux, ParametrosSensorFluxo *dados) {
  
  bool finalizado = false;
  while (!finalizado) {
    saidaRasp.enviaTexto("====================================================================");
    switch (sensFlux) {
      case SENS_FLUX_AR:
        saidaRasp.enviaTexto("SENSOR DE FLUXO DE AR"); break;
      case SENS_FLUX_O2:
        saidaRasp.enviaTexto("SENSOR DE FLUXO DE O2"); break;
      case SENS_FLUX_EX:
        saidaRasp.enviaTexto("SENSOR DE FLUXO DE EXALACAO"); break;
    }
    saidaRasp.enviaTexto("--------------------------------------------------------------------");
    saidaRasp.enviaTexto(" ");

    finalizado = ensaio.ensaiaSensorFluxo(sensFlux, dados);
    if (finalizado) {
      defineParametrosSensorFluxo(dados);
    } else {
      erro();
    }
  }
  saidaRasp.enviaTexto(" ");
}

void defineParametrosSensorFluxo(ParametrosSensorFluxo *dados) {
  float minAdcNoZero = dados->curvaAdc[0] - TOLER_ARBITRARIA_ZERO_SENS_FLUXO * 80000;
  dados->minAdcNoZero = saturaFloat(minAdcNoZero, -40000, 40000);
  float maxAdcNoZero = dados->curvaAdc[0] + TOLER_ARBITRARIA_ZERO_SENS_FLUXO * 80000;
  dados->maxAdcNoZero = saturaFloat(maxAdcNoZero, -40000, 40000);
}

void calibraAutotestesValvulasAOP() {

  calibraValvulaAOP(VALV_AR);
  calibraValvulaAOP(VALV_O2);
  calibraValvulaAOP(VALV_PILOTO);
}

void calibraValvulaAOP(ValvulasAOPEnum valv) {
  bool finalizado = false;
  while (!finalizado) {
    saidaRasp.enviaTexto("====================================================================");
    switch (valv) {
      case VALV_AR:
        saidaRasp.enviaTexto("ENSAIO DA VALVULA DE AR"); break;
      case VALV_O2:
        saidaRasp.enviaTexto("ENSAIO DA VALVULA DE O2"); break;
      case VALV_PILOTO:
        saidaRasp.enviaTexto("ENSAIO DA VALVULA PILOTO"); break;
    }
    saidaRasp.enviaTexto("--------------------------------------------------------------------");
    saidaRasp.enviaTexto(" ");

    float dadosValvula[3];
    finalizado = ensaio.ensaiaValvulaAOP(valv, dadosValvula);
    if (finalizado) {
      defineParametrosValvulaAOP(valv, dadosValvula);
    }
    else {
      erro();
    }
  }
  saidaRasp.enviaTexto(" ");
}

void defineParametrosValvulaAOP(ValvulasAOPEnum valv, float* dadosValvula) {

  float pwmAberturaMin = dadosValvula[0] - TOLER_ARBITRARIA_PWM_ABERTURA * 4000;
  dadosCalibValvulasAOP.pwmAberturaMin[valv] = saturaFloat(pwmAberturaMin, 0, 4000);
  float pwmAberturaMax = dadosValvula[0] + TOLER_ARBITRARIA_PWM_ABERTURA * 4000;
  dadosCalibValvulasAOP.pwmAberturaMax[valv] = saturaFloat(pwmAberturaMax, 0, 4000);
  float pwmFechamentoMin = dadosValvula[1] - TOLER_ARBITRARIA_PWM_FECHAMENTO * 4000;
  dadosCalibValvulasAOP.pwmFechamentoMin[valv] = saturaFloat(pwmFechamentoMin, 0, 4000);
  float pwmFechamentoMax = dadosValvula[1] + TOLER_ARBITRARIA_PWM_FECHAMENTO * 4000;
  dadosCalibValvulasAOP.pwmFechamentoMax[valv] = saturaFloat(pwmFechamentoMax, 0, 4000);
  float saidaPicoMin = dadosValvula[2] - TOLER_ARBITRARIA_SAIDA_PICO * 100;
  dadosCalibValvulasAOP.saidaPicoMin[valv] = saturaFloat(saidaPicoMin, 0, 100);
  float saidaPicoMax = dadosValvula[2] + TOLER_ARBITRARIA_SAIDA_PICO * 100;
  dadosCalibValvulasAOP.saidaPicoMax[valv] = saturaFloat(saidaPicoMax, 0, 100);
}

void calibraAutotestesValvulaExalacao() {
  bool finalizado = false;
  while (!finalizado) {
    saidaRasp.enviaTexto("====================================================================");
    saidaRasp.enviaTexto("ENSAIO DA VALVULA DE EXALACAO");
    saidaRasp.enviaTexto("--------------------------------------------------------------------");
    saidaRasp.enviaTexto(" ");

    finalizado = ensaio.ensaiaValvulaExalacao(
      dadosCalibValvulaExalacao.curvaPiloto,
      dadosCalibValvulaExalacao.curvaSistema,
      limiaresValvulaExalacao);
    if (finalizado) {
      defineParametrosValvulaExalacao();
    } else {
      erro();
    }
  }
  saidaRasp.enviaTexto(" ");
}

void defineParametrosValvulaExalacao() {
  
  float minPressaoPicoSistema = dadosCalibValvulaExalacao.curvaSistema[10] - TOLER_ARBITRARIA_CURVA_EXALACAO * 100;
  dadosCalibValvulaExalacao.minPressaoPicoSistema = saturaFloat(minPressaoPicoSistema, 0, 100);

  float pilotoNoLimiarSistema;
  for (int i=0; i<9; i++) {
    pilotoNoLimiarSistema = mapCurvaFloat(
      limiaresValvulaExalacao[i],
      dadosCalibValvulaExalacao.curvaSistema,
      dadosCalibValvulaExalacao.curvaPiloto,
      11
    );
    dadosCalibValvulaExalacao.limiaresSistema[i] = limiaresValvulaExalacao[i];
    float minPilotoNoLimiarSistema = pilotoNoLimiarSistema - TOLER_ARBITRARIA_CURVA_EXALACAO * 100;
    dadosCalibValvulaExalacao.minPilotoNoLimiarSistema[i] = saturaFloat(minPilotoNoLimiarSistema, 0, 100);
    float maxPilotoNoLimiarSistema = pilotoNoLimiarSistema + TOLER_ARBITRARIA_CURVA_EXALACAO * 100;
    dadosCalibValvulaExalacao.maxPilotoNoLimiarSistema[i] = saturaFloat(maxPilotoNoLimiarSistema, 0, 100);
  }
}


void calibraAutotestesValvulaSeguranca() {
  bool finalizado = false;
  while (!finalizado) {
    saidaRasp.enviaTexto("====================================================================");
    saidaRasp.enviaTexto("ENSAIO DA VALVULA DE SEGURANCA");
    saidaRasp.enviaTexto("--------------------------------------------------------------------");
    saidaRasp.enviaTexto(" ");
    int tempoValvulaSeguranca = 0;
    finalizado = ensaio.ensaiaValvulaSeguranca(&tempoValvulaSeguranca); 
    if (finalizado) {
      dadosCalibValvulasAOP.tempoMaxValvSeguranca = tempoValvulaSeguranca * (1 + TOLER_ARBITRARIA_VALVULA_SEGURANCA);
    } else {
      erro();
    }
  }
  saidaRasp.enviaTexto(" ");
}

void carregaParametrosSensorFluxo(ParametrosSensorFluxo *dadosCalib, ParametrosSensorFluxo *param) {
  for (int i=0; i<18; i++) {
    param->curvaAdc[i] = dadosCalib->curvaAdc[i];
    param->curvaFluxo[i] = dadosCalib->curvaFluxo[i];
  }
}

void enviaDadosParaRasp() {
  saidaRasp.enviaTexto("!(finaliza)!");
  delay(100);
  saidaRasp.enviaParametrosSensoresPressao(dadosCalibSensoresPressao);
  saidaRasp.enviaParametrosSensorFluxo(SENS_FLUX_AR, dadosCalibSensorFluxoAr);
  saidaRasp.enviaParametrosSensorFluxo(SENS_FLUX_O2, dadosCalibSensorFluxoO2);
  saidaRasp.enviaParametrosSensorFluxo(SENS_FLUX_EX, dadosCalibSensorFluxoEx);
  saidaRasp.enviaParametrosValvulaExalacao(dadosCalibValvulaExalacao);
  saidaRasp.enviaParametrosValvulasAOP(dadosCalibValvulasAOP);
}

void erro() {
  saidaRasp.enviaTexto(" ");
  saidaRasp.enviaTexto("====================================================================");
  saidaRasp.enviaTexto(" ");
  saidaRasp.enviaTexto("ERRO !!!");
  desligaValvulas();

  saidaRasp.enviaTexto(" ");
  saidaRasp.enviaTexto("Insira 1 para reiniciar a calibracao do sensor atual");
  String entrada = "";
  while (!entrada.equals("1")) {
    saidaRasp.enviaTexto("!(entrada)!");
    entrada = entradaRasp.esperaRecebeTexto();
  }
  saidaRasp.enviaTexto(" ");
  saidaRasp.enviaTexto(" ");
  saidaRasp.enviaTexto(" ");
} 

#endif
