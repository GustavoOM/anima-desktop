#include "_global.h"

DadosTesteUnitario dadosReferencia;

void setup() {
  Serial.begin(115200);
  ensaio.setup();
}

void loop() {
  testeUnitarioAutotestes();
  while (true);
}

void testeUnitarioAutotestes() {
  inicializaTestes();

  Serial.println("EXPECTATIVA:");
  Serial.println("  Sem falhas nos testes");
  dadosTeste = dadosReferencia;
  Serial.println("RESULTADO:");
  executaTeste();
  
  setaTeste(TESTE_ALIMENTACAO_ELETRICA);
  dadosTeste.tensaoFonte = 12.4;
  executaTeste();

  setaTeste(TESTE_BATERIA);
  dadosTeste.tensaoBateria = 10.9;
  executaTeste();

  setaTeste(TESTE_ALIMENTACAO_AR);
  dadosTeste.pressaoAr = 29;
  executaTeste();

  setaTeste(TESTE_ALIMENTACAO_O2);
  dadosTeste.pressaoO2 = 29;
  executaTeste();

  Serial.println("// Conversor para sensor de pressao");
  setaTeste(TESTE_CONVERSORES);
  dadosTeste.erroTimeoutPressao[0] = true;
  executaTeste();

  Serial.println("// Conversor para sensor de fluxo");
  setaTeste(TESTE_CONVERSORES);
  dadosTeste.erroTimeoutFluxo[1] = true;
  executaTeste();

  setaTeste(TESTE_SENSORES_PRESSAO);
  dadosTeste.erroOffsetPressao[2] = true;
  executaTeste();

  setaTeste(TESTE_SENSORES_FLUXO);
  dadosTeste.erroOffsetFluxo[3] = true;
  executaTeste();

  Serial.println("// Mudando posicao de abertura e fechamento");
  setaTeste(TESTE_VALVULA_AR);
  dadosTeste.minPwmValvulasAOP[0] = 1200;
  executaTeste();

  Serial.println("// Mudando saida maxima");
  setaTeste(TESTE_VALVULA_AR);
  dadosTeste.maxSaidaValvulasAOP[0] = 65;
  executaTeste();

  Serial.println("// Mudando posicao de abertura e fechamento");
  setaTeste(TESTE_VALVULA_O2);
  dadosTeste.minPwmValvulasAOP[1] = 3000;
  executaTeste();

  Serial.println("// Mudando saida maxima");
  setaTeste(TESTE_VALVULA_O2);
  dadosTeste.maxSaidaValvulasAOP[1] = 40;
  executaTeste();

  Serial.println("// Mudando posicao de abertura e fechamento");
  setaTeste(TESTE_VALVULA_PILOTO);
  dadosTeste.minPwmValvulasAOP[2] = 1500;
  executaTeste();

  Serial.println("// Mudando saida maxima");
  setaTeste(TESTE_VALVULA_PILOTO);
  dadosTeste.maxSaidaValvulasAOP[2] = 95;
  executaTeste();

  Serial.println("// Mudando ponto inicial da curva");
  setaTeste(TESTE_VALVULA_EXALACAO);
  dadosTeste.minPilotoCurvaSis = 7;
  executaTeste();

  Serial.println("// Mudando ponto final da curva");
  setaTeste(TESTE_VALVULA_EXALACAO);
  dadosTeste.maxPilotoCurvaSis = 70;
  executaTeste();

  Serial.println("// Mudando pressao maxima");
  setaTeste(TESTE_VALVULA_EXALACAO);
  dadosTeste.maxPressaoSis = 63;
  executaTeste();

  setaTeste(TESTE_VALVULA_SEGURANCA);
  dadosTeste.tempoValvulaSeg = 150;
  executaTeste();

  setaTeste(TESTE_CIRCUITO_PACIENTE);
  dadosTeste.vazamentoCircuitPac = 0.08;
  executaTeste();
}

void inicializaTestes() {
  dadosReferencia.tensaoFonte = 13.0;
  dadosReferencia.tensaoBateria = 12.5;
  dadosReferencia.pressaoAr = 40;
  dadosReferencia.pressaoO2 = 40;
  for (int i=0; i<3; i++) {
    dadosReferencia.erroTimeoutPressao[i] = false;
    dadosReferencia.erroOffsetPressao[i] = false;
  }
  for (int i=0; i<4; i++) {
    dadosReferencia.erroTimeoutFluxo[i-1] = false;
    dadosReferencia.erroOffsetFluxo[i-1] = false;
  }
  dadosReferencia.minPwmValvulasAOP[0] = 1800;
  dadosReferencia.maxPwmValvulasAOP[0] = 3500;
  dadosReferencia.minSaidaValvulasAOP[0] = 0;
  dadosReferencia.maxSaidaValvulasAOP[0] = 80;
  dadosReferencia.minPwmValvulasAOP[1] = 2100;
  dadosReferencia.maxPwmValvulasAOP[1] = 3400;
  dadosReferencia.minSaidaValvulasAOP[1] = 0;
  dadosReferencia.maxSaidaValvulasAOP[1] = 55;
  dadosReferencia.minPwmValvulasAOP[2] = 800;
  dadosReferencia.maxPwmValvulasAOP[2] = 2000;
  dadosReferencia.minSaidaValvulasAOP[2] = 0;
  dadosReferencia.maxSaidaValvulasAOP[2] = 110;
  dadosReferencia.minAdcSensoresFluxo[0] = 25;
  dadosReferencia.maxAdcSensoresFluxo[0] = 30050;
  dadosReferencia.minAdcSensoresFluxo[1] = -370;
  dadosReferencia.maxAdcSensoresFluxo[1] = 24300;
  dadosReferencia.minAdcSensoresFluxo[2] = -1200;
  dadosReferencia.maxAdcSensoresFluxo[2] = 57000;
  dadosReferencia.minPilotoCurvaSis = 11;
  dadosReferencia.maxPilotoCurvaSis = 63;
  dadosReferencia.minPressaoSis = 0;
  dadosReferencia.maxPressaoSis = 75;
  dadosReferencia.tempoValvulaSeg = 130;
  dadosReferencia.vazamentoCircuitPac = 0.03;
  
  dadosTeste = dadosReferencia;
  calibAutomatica();
}

void setaTeste(AutotestesEnum teste) {
  Serial.println("EXPECTATIVA:");
  Serial.print("  Falha no teste "); Serial.println(teste);
  dadosTeste = dadosReferencia;
  Serial.println("RESULTADO:");
}

void executaTeste() {
  autotestes.realizaAutotestes();
  Serial.println();
}
