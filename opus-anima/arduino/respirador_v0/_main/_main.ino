#include "_global.h"

/* COMANDOS DA SERIAL
 * 
 * 99 - Para/continua respiração
 * 
 * Seleciona PID de referência:
 * 0 - PID do fluxo de ar
 * 1 - PID do fluxo de O2
 * 2 - PID de pressão inspiratória
 * 3 - PID de pressão piloto
 * 4 - PID de pressão expiratória
 * 
 * Liga/desliga gráfico padrão:
 * 10 - Fluxo do paciente
 * 11 - Pressão do sistema
 * 12 - Volume
 * 13 - Fluxo de ar
 * 14 - Fluxo de O2
 * 15 - Fluxo inspiratório
 * 16 - Fluxo expiratório
 * 17 - Pressão inspiratória
 * 18 - Pressão expiratória
 * 19 - Pressão piloto
 * 
 * Liga/desliga gráfico em função do PID de referência:
 * 20 - Ganho autoKp
 * 21 - Integral
 * 22 - Setpoint
 * 23 - Sensor
 * 24 - Referência para delta autoKp
 * 25 - Saida P
 * 26 - Saida I
 * 27 - Saida D
 * 28 - Saida calculada
 * 29 - Saida final (saturada) 
 * 
 * Liga/desliga gráfico padrão:
 * 30 - Pressão da alimentação de ar
 * 31 - Pressão da alimentação de O2
 * 32 - Tensão da fonte de alimentação elétrica
 * 33 - Tensão da bateria
 * 34 - ADC do sensor de fluxo de ar
 * 35 - ADC do sensor de fluxo de O2
 * 36 - ADC do sensor de fluxo de exalação
 *
 * 
 * OBS.: a aplicação aceita ligar até 4 gráficos simultâneos
 */

void setup() {
  SerialUSB.begin(115200);
  Serial1.begin(115200);
  
  #if defined(RESPIRADOR)
    setupRespirador();
  #elif defined(CALIBRA_AR_O2_PIL)
    setupCalibArO2Pil();
  #elif defined(CALIBRA_AUTOMATICA)
    setupCalibAutomatica(); 
  #else //TERMINAL
    setupTerminal();
  #endif
}

void loop() {
  #if defined(RESPIRADOR)
    loopRespirador();
  #elif defined(CALIBRA_AR_O2_PIL)
    loopCalibArO2Pil();
  #elif defined(CALIBRA_AUTOMATICA)
    loopCalibAutomatica();
  #else //TERMINAL
    loopTerminal();
  #endif 
}

void setupMaquina() {
  pinMode(LED_BUILTIN, OUTPUT);

  analogReadResolution(12);
  analogWriteResolution(12);
  analogReference(AR_DEFAULT);

  pinMode(ioVP1, OUTPUT);
  pinMode(ioVP2, OUTPUT);
  pinMode(ioVP3, OUTPUT);
  pinMode(ioVSeguranca, OUTPUT);
  pinMode(ioAlarme, OUTPUT);
  pinMode(ioAlarmeExterno, OUTPUT);
  pinMode(ioLigaFonte, OUTPUT);
  pinMode(ioLigaCPU, OUTPUT);
  pinMode(ioSoftKey, INPUT_PULLUP);
  pinMode(ioRasp, INPUT);

  digitalWrite(ioVP1, LOW);
  digitalWrite(ioVP2, LOW);
  digitalWrite(ioVP3, LOW);
  digitalWrite(ioVSeguranca, HIGH);
  digitalWrite(ioAlarme, LOW);
  digitalWrite(ioAlarmeExterno, LOW);
  digitalWrite(ioLigaFonte, HIGH);
  digitalWrite(ioLigaCPU, HIGH);
  
  sensorF1.begin(35, 36);
  sensorF1.set_data_rate(DR_2000SPS);
  sensorF1.set_pga_gain(PGA_GAIN_1);
  sensorF1.select_mux_channels(MUX_AIN1_AIN0);
  sensorF1.Start_Conv();

  sensorF2.begin(38, 40);
  sensorF2.set_data_rate(DR_2000SPS);
  sensorF2.set_pga_gain(PGA_GAIN_1);
  sensorF2.select_mux_channels(MUX_AIN0_AIN1);
  sensorF2.Start_Conv();

  sensorF3.begin(47, 45);
  sensorF3.set_data_rate(DR_2000SPS);
  sensorF3.set_pga_gain(PGA_GAIN_4);
  sensorF3.select_mux_channels(MUX_AIN0_AIN1);
  sensorF3.Start_Conv();

  sensorF4.begin(51, 49);
  sensorF4.set_data_rate(DR_2000SPS);
  sensorF4.set_pga_gain(PGA_GAIN_1);
  sensorF4.select_mux_channels(MUX_AIN0_AIN1);
  sensorF4.Start_Conv();

  sensorP3.begin(50, 48);
  sensorP3.set_data_rate(DR_2000SPS);
  sensorP3.set_pga_gain(PGA_GAIN_1);
  sensorP3.select_mux_channels(MUX_SE_CH0);
  sensorP3.Start_Conv();

  sensorP4.begin(46, 25);
  sensorP4.set_data_rate(DR_2000SPS);
  sensorP4.set_pga_gain(PGA_GAIN_1);
  sensorP4.select_mux_channels(MUX_SE_CH0);
  sensorP4.Start_Conv();

  sensorP5.begin(33, 34);
  sensorP5.set_data_rate(DR_2000SPS);
  sensorP5.set_pga_gain(PGA_GAIN_1);
  sensorP5.select_mux_channels(MUX_SE_CH0);
  sensorP5.Start_Conv();

  delay(30);

#ifndef RESPIRADOR
  sensores.zeraSensores();
#endif
}

void loopGrafico() {

  verificaComandosGrafico();
  verificaParadaGrafico();

  if (!paradaMaquina) {
    bool alteracao = false;
    for (int i=0; i<4; i++) {
      if (grafico.curvasAtuais[i] != grafico.curvasAnteriores[i]) {
        alteracao = true;
        break;
      }
    }
  
    if (alteracao) {
      plotaLegenda();
      for (int i=0; i<4; i++) {
        grafico.curvasAnteriores[i] = grafico.curvasAtuais[i];
      }
    } else {
      plotaValores(); 
    }
  }
}

void plotaLegenda() {
  SerialUSB.flush();
 
  for (int i=0; i<4; i++) {
    switch (grafico.curvasAtuais[i]) {
      case 0: SerialUSB.print("----"); break;

      case 10: SerialUSB.print("fluxoPac"); break;
      case 11: SerialUSB.print("pressSis"); break;
      case 12: SerialUSB.print("volume"); break;
      case 13: SerialUSB.print("fluxoAr"); break;
      case 14: SerialUSB.print("fluxoO2"); break;
      case 15: SerialUSB.print("fluxoIn"); break;
      case 16: SerialUSB.print("fluxoEx"); break;
      case 17: SerialUSB.print("pressIn"); break;
      case 18: SerialUSB.print("pressEx"); break;
      case 19: SerialUSB.print("pressPil"); break;
      
      case 20: SerialUSB.print("ganho"); break;
      case 21: SerialUSB.print("integral"); break;
      case 22: SerialUSB.print("setpoint"); break;
      case 23: SerialUSB.print("sensor"); break;
      case 24: SerialUSB.print("refDelta"); break;
      case 25: SerialUSB.print("saidaP"); break;
      case 26: SerialUSB.print("saidaI"); break;
      case 27: SerialUSB.print("saidaD"); break;
      case 28: SerialUSB.print("saidaCalc"); break;
      case 29: SerialUSB.print("saida"); break;

      case 30: SerialUSB.print("pressAr"); break;
      case 31: SerialUSB.print("pressO2"); break;
      case 32: SerialUSB.print("fonte"); break;
      case 33: SerialUSB.print("bateria"); break;
      case 34: SerialUSB.print("adcF1"); break;
      case 35: SerialUSB.print("adcF2"); break;
      case 36: SerialUSB.print("adcF4"); break;
      case 37: SerialUSB.print("fluxoCal"); break;
      case 38: SerialUSB.print("rFlux"); break;
      case 39: SerialUSB.print("rPress"); break;

      case 40: SerialUSB.print("tUtil1"); break;
      case 41: SerialUSB.print("tUtil2"); break;
      case 42: SerialUSB.print("tUtil3"); break;
      case 43: SerialUSB.print("tUtil4"); break;
      case 44: SerialUSB.print("tUtilCi"); break;
      case 45: SerialUSB.print("tTot1"); break;
      case 46: SerialUSB.print("tTot2"); break;
      case 47: SerialUSB.print("tTot3"); break;
      case 48: SerialUSB.print("tTot4"); break;
      case 49: SerialUSB.print("tTotCi"); break;
    }
    SerialUSB.print(" ");   
  }
  
  SerialUSB.println();
}

void plotaValores() {
  float valores[4] = {0.0, 0.0, 0.0, 0.0};

  float escalaSaida = 100.0;
  if (grafico.pidRef == &estadoPidPressaoExp || grafico.pidRef == &estadoPidPressaoIns) {
    escalaSaida = 1.0;
  }

  for (int i=0; i<4; i++) {
    if (grafico.curvasAtuais[i] != 0) {
      switch (grafico.curvasAtuais[i]) {
        case 10: valores[i] = medidas.fluxoPac; break;
        case 11: valores[i] = medidas.pressaoSis; break;
        case 12: valores[i] = statusCiclo.vTidal; break;
        case 13: valores[i] = medidas.fluxoAr; break;
        case 14: valores[i] = medidas.fluxoO2; break;
        case 15: valores[i] = medidas.fluxoAr + medidas.fluxoO2; break;
        case 16: valores[i] = medidas.fluxoExp; break;
        case 17: valores[i] = statusSensores.valorPressao[3]; break;
        case 18: valores[i] = statusSensores.valorPressao[4]; break;
        case 19: valores[i] = medidas.pressaoPil; break;
        
        case 20: valores[i] = grafico.pidRef->ganho; break;
        case 21: valores[i] = grafico.pidRef->integral; break;
        case 22: valores[i] = grafico.pidRef->setpointAtual; break;
        case 23: valores[i] = grafico.pidRef->sensorAtual; break;
        case 24: valores[i] = grafico.pidRef->refDelta; break;
        case 25: valores[i] = grafico.pidRef->saidaProp/escalaSaida; break;
        case 26: valores[i] = grafico.pidRef->saidaInte/escalaSaida; break;
        case 27: valores[i] = grafico.pidRef->saidaDeri/escalaSaida; break;
        case 28: valores[i] = grafico.pidRef->saidaCalc/escalaSaida; break;
        case 29: valores[i] = grafico.pidRef->saidaAtual/escalaSaida; break;

        case 30: valores[i] = statusSensores.valorPressao[1]; break;
        case 31: valores[i] = statusSensores.valorPressao[2]; break;
        case 32: valores[i] = statusSensores.tensaoFonte; break;
        case 33: valores[i] = statusSensores.tensaoBateria; break;
        case 34: valores[i] = statusSensores.adcFluxo[1]; break;
        case 35: valores[i] = statusSensores.adcFluxo[2]; break;
        case 36: valores[i] = statusSensores.adcFluxo[4]; break;
        case 37: valores[i] = statusSensores.fluxoCalibracao; break;
        case 38: valores[i] = medidasRasp.fluxoPac; break;
        case 39: valores[i] = medidasRasp.pressaoSis; break;

        case 40: valores[i] = respirador.dadosTempo.tUtilIter[0]; break;
        case 41: valores[i] = respirador.dadosTempo.tUtilIter[1]; break;
        case 42: valores[i] = respirador.dadosTempo.tUtilIter[2]; break;
        case 43: valores[i] = respirador.dadosTempo.tUtilIter[3]; break;
        case 44: valores[i] = respirador.dadosTempo.tUtilCiclo; break;
        case 45: valores[i] = respirador.dadosTempo.tTotalIter[0]; break;
        case 46: valores[i] = respirador.dadosTempo.tTotalIter[1]; break;
        case 47: valores[i] = respirador.dadosTempo.tTotalIter[2]; break;
        case 48: valores[i] = respirador.dadosTempo.tTotalIter[3]; break;
        case 49: valores[i] = respirador.dadosTempo.tTotalCiclo; break;
      }
    }
    else {
      valores[i] = 0.0;
    }
  }

  for (int i=0; i<4; i++) {
    SerialUSB.print(valores[i]); SerialUSB.print(" ");
  }

  SerialUSB.println();
}

void verificaParadaGrafico() {
  if (paradaMaquina) {
    respirador.setLoop(false);
  
    valvulaFluxoArVP1(0);
    valvulaFluxoOxVP2(0);
    valvulaOxExalacaoVP3(0);
  } else {
    respirador.setLoop(true);
  }
}

void verificaComandosGrafico() {
  if (SerialUSB.available() > 0) {
    int entrada = SerialUSB.parseInt();
    int entrada2 = SerialUSB.parseInt();

    if (0 <= entrada && entrada <= 4) {
      switch (entrada) {
        case 0: grafico.pidRef = &estadoPidFluxoAr; break;
        case 1: grafico.pidRef = &estadoPidFluxoO2; break;
        case 2: grafico.pidRef = &estadoPidPressaoIns; break;
        case 3: grafico.pidRef = &estadoPidPressaoPil; break;
        case 4: grafico.pidRef = &estadoPidPressaoExp; break;
      }
    }

    if (10 <= entrada && entrada <= 59) {
      bool curvaLigada = false;
      for (int i=0; i<4; i++) {
        if (grafico.curvasAtuais[i] == entrada) {
          grafico.curvasAtuais[i] = 0;
          curvaLigada = true;
          break;
        }
      }
      if (!curvaLigada) {
        for (int i=0; i<4; i++) {
          if (grafico.curvasAtuais[i] == 0) {
            grafico.curvasAtuais[i] = entrada;
            break;
          }
        }
      }
    }
    
    if (entrada == 99) {
      paradaMaquina = !paradaMaquina;
    }
  }
}
