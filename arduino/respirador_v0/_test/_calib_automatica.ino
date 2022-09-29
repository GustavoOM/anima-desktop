#include "_global.h"

#define TOLER_ARBITRARIA_ZERO_SENS_FLUXO (0.005)
#define TOLER_ARBITRARIA_PWM_ABERTURA (0.10)
#define TOLER_ARBITRARIA_PWM_FECHAMENTO (0.10)
#define TOLER_ARBITRARIA_SAIDA_PICO (0.10)
#define TOLER_ARBITRARIA_CURVA_EXALACAO (0.02)
#define TOLER_ARBITRARIA_VALVULA_SEGURANCA (0.1)

ParametrosSensorFluxo dadosCalibSensorFluxoAr;
ParametrosSensorFluxo dadosCalibSensorFluxoO2;
ParametrosSensorFluxo dadosCalibSensorFluxoEx;
ParametrosValvulasAOP dadosCalibValvulasAOP;
ParametrosValvulaExalacao dadosCalibValvulaExalacao;

float limiaresValvulaExalacao[9] = {2, 4, 6, 9, 12, 18, 25, 35, 50};
char buffer[100];

void calibAutomatica() {
  calibraSensoresFluxo();
  calibraAutotestesValvulasAOP();
  calibraAutotestesValvulaExalacao();
  calibraAutotestesValvulaSeguranca();
  carregaDadosCalibracao();
}

void calibraSensoresFluxo() {
  calibraSensorFluxo(SENS_FLUX_AR, &dadosCalibSensorFluxoAr);
  calibraSensorFluxo(SENS_FLUX_O2, &dadosCalibSensorFluxoO2);
  calibraSensorFluxo(SENS_FLUX_EX, &dadosCalibSensorFluxoEx);
}

void calibraSensorFluxo(SensFluxEnum sensFlux, ParametrosSensorFluxo *dados) {
  
  bool finalizado = false;
  while (!finalizado) {
    finalizado = ensaio.ensaiaSensorFluxo(sensFlux, dados);
    if (finalizado) {
      defineParametrosSensorFluxo(dados);
    } else {
      erro();
    }
  }
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
    float dadosValvula[3];
    finalizado = ensaio.ensaiaValvulaAOP(valv, dadosValvula);
    if (finalizado) {
      defineParametrosValvulaAOP(valv, dadosValvula);
    }
    else {
      erro();
    }
  }
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
    int tempoValvulaSeguranca = 0;
    finalizado = ensaio.ensaiaValvulaSeguranca(&tempoValvulaSeguranca); 
    if (finalizado) {
      dadosCalibValvulasAOP.tempoMaxValvSeguranca = tempoValvulaSeguranca * (1 + TOLER_ARBITRARIA_VALVULA_SEGURANCA);
    } else {
      erro();
    }
  }
}

void carregaDadosCalibracao() {
  paramSensorFluxoAr = dadosCalibSensorFluxoAr;
  paramSensorFluxoO2 = dadosCalibSensorFluxoO2;
  paramSensorFluxoEx = dadosCalibSensorFluxoEx;
  paramValvulasAOP = dadosCalibValvulasAOP;
  paramValvulaExalacao = dadosCalibValvulaExalacao;
}

void erro() {
  desligaValvulas();
}
