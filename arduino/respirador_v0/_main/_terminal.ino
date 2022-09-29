#include "_global.h"

#ifdef TERMINAL

void setupTerminal() {
  setupMaquina();
  carregaTerminal();
}

void carregaTerminal() {
  while (true) {
    SerialUSB.println("Insira 1 para iniciar");
    if (SerialUSB.available() > 0) {
      int ent = SerialUSB.parseInt();
      if (ent == 1) {
        break;
      }
    }
    delay(500);
  }
  
  while (true) {
    SerialUSB.println();
    SerialUSB.println("Voce quer carregar os parametros dos arquivos de configuracao?");
    SerialUSB.println("1 - Sim");
    SerialUSB.println("2 - Nao");
    while (SerialUSB.available() <= 0);
    int ent = SerialUSB.parseInt();
    if (ent == 1) {
      SerialUSB.println("Rode o script [terminal.sh] na linha de comando do Linux");
      entradaRasp.setup();
      entradaRasp.recebeParametrosInicializacao();
      break;
    } else if (ent == 2) {
      break;
    }
  }
  SerialUSB.println();
}

void loopTerminal() {

  if (SerialUSB.available() > 0) {
    int valv = SerialUSB.parseInt();
    int pwm = SerialUSB.parseInt();

    if (valv == 1) { valvulaFluxoArVP1(pwm); }
    if (valv == 2) { valvulaFluxoOxVP2(pwm); }
    if (valv == 3) { valvulaOxExalacaoVP3(pwm); }
  }

  for (int i=0; i<100; i++) {
    sensores.leSensores();
    delay(10);
  }

  SerialUSB.println("----------------------------------");
  
  SerialUSB.println("SENSORES DE FLUXO");
  SerialUSB.print("Fluxo de ar: ");
  SerialUSB.print(statusSensores.valorFluxo[1], 1);
  SerialUSB.println(" L/min");
  SerialUSB.print("       (adc) ");
  SerialUSB.println(statusSensores.adcFluxo[1]);
  SerialUSB.print("Fluxo de oxigenio: ");
  SerialUSB.print(statusSensores.valorFluxo[2], 1);
  SerialUSB.println(" L/min");
  SerialUSB.print("             (adc) ");
  SerialUSB.println(statusSensores.adcFluxo[2]);
  SerialUSB.print("Fluxo de exalacao: ");
  SerialUSB.print(statusSensores.valorFluxo[4], 1);
  SerialUSB.println(" L/min");
  SerialUSB.print("             (adc) ");
  SerialUSB.println(statusSensores.adcFluxo[4]);
  SerialUSB.print("Fluxo para calibração: ");
  SerialUSB.print(statusSensores.fluxoCalibracao, 1);
  SerialUSB.println(" L/min");

  SerialUSB.println("----------------------------------");
  
  SerialUSB.println("SENSORES DE PRESSAO");
  SerialUSB.print("Pressao de inalacao: ");
  SerialUSB.print(statusSensores.valorPressao[3], 1);
  SerialUSB.println(" cmH2O");
  SerialUSB.print("Pressao de exalacao: ");
  SerialUSB.print(statusSensores.valorPressao[4], 1);
  SerialUSB.println(" cmH2O");
  SerialUSB.print("Pressao piloto: ");
  SerialUSB.print(statusSensores.valorPressao[5], 1);
  SerialUSB.println(" cmH2O");

  SerialUSB.println("----------------------------------");

  SerialUSB.println("OUTROS");
  SerialUSB.print("Pressao de ar/O2: ");
  SerialUSB.print(statusSensores.valorPressao[1], 0);
  SerialUSB.print(" ");
  SerialUSB.print(statusSensores.valorPressao[2], 0);
  SerialUSB.println(" PSI");
  SerialUSB.print("Tensao da fonte/bateria: ");
  SerialUSB.print(statusSensores.tensaoFonte, 1);
  SerialUSB.print(" ");
  SerialUSB.print(statusSensores.tensaoBateria, 1);
  SerialUSB.println(" V");

  SerialUSB.println("==================================");
  
  SerialUSB.println("COMANDOS");
  SerialUSB.println("1 - Valvula de ar");
  SerialUSB.println("2 - Valvula de oxigenio");
  SerialUSB.println("3 - Valvula piloto");
  SerialUSB.println();
  
  SerialUSB.println("==================================");
}

#endif
