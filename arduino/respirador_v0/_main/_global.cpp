#include "_global.h"


//-------< global variables >--------------------------------------------------

// Pwm de entrada das válvulas
int pwmValvAr = 0;
int pwmValvO2 = 0;
int pwmValvPil = 0;
bool valvSegAberta = false;

ADS1220 sensorP3;
ADS1220 sensorP4;
ADS1220 sensorP5;
ADS1220 sensorP6;
ADS1220 sensorF1;
ADS1220 sensorF2;
ADS1220 sensorF3;
ADS1220 sensorF4;

bool paradaMaquina = false;

//-------< functions >---------------------------------------------------------

// Funções para verificar se passou determinado período de tempo
boolean passouTempoMillis(unsigned long inicio, unsigned long intervalo) {
  return (millis() - inicio) >= intervalo;
}

boolean passouTempoMicros(unsigned long inicio, unsigned long intervalo) {
  return (micros() - inicio) >= intervalo;
}

// Funções para atuar nas válvulas
void valvulaFluxoArVP1(int periodo) {
  pwmValvAr = periodo;
#ifndef SIMULADOR
  analogWrite(ioVP1, periodo);
#endif
}

void valvulaFluxoOxVP2(int periodo) {
  pwmValvO2 = periodo;
#ifndef SIMULADOR
  analogWrite(ioVP2, periodo);
#endif
}

void valvulaOxExalacaoVP3(int periodo) {
  pwmValvPil = periodo;
#ifndef SIMULADOR
  analogWrite(ioVP3, periodo);
#endif
}

void valvulaSeguranca(bool abre) {
  if (abre) {
    analogWrite(ioVSeguranca, 0);
    valvSegAberta = true;
  }
  else {
    analogWrite(ioVSeguranca, 2500);
    valvSegAberta = false;
  }
}

// Mapeia um valor de um intervalo para outro
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  if (x < in_min) {
    return out_min;
  } else if (x > in_max) {
    return out_max;
  } else {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  }
}

float mapCurvaFloat(float pontoX, float* curvaX, float* curvaY, int tamCurva) {
  
  if (pontoX < curvaX[0]) {
    return curvaY[0];
  }

  for (int i=0; i<tamCurva; i++) {
    if ((pontoX >= curvaX[i]) && (pontoX < curvaX[i+1])) {
      return mapFloat(pontoX, curvaX[i], curvaX[i+1], curvaY[i], curvaY[i+1]);         
    }
  }

  if (pontoX >= curvaX[tamCurva-1]) {
    return curvaY[tamCurva-1];
  }
}

void desligaValvulas() {
  valvulaFluxoArVP1(0);
  valvulaFluxoOxVP2(0);
  valvulaOxExalacaoVP3(0);
}

float saturaFloat(float valor, float minimo, float maximo) {
  if (valor < minimo) {
    return minimo;
  } else if (valor > maximo) {
    return maximo;
  } else {
    return valor;
  }
}

unsigned long calculaChecksum(char* msgBuf, int fimMsg) {
  unsigned long soma = 0;
  unsigned long bloco = 0;
  for (int i=0; i<fimMsg; i++) {
    int j = i % 4;
    if (j == 0) {
      bloco = 0;
    }

    bloco |= msgBuf[i] << 8*(3-j);

    if (j == 3) {
      soma += bloco;
    }
    if (i == (fimMsg-1) && j != 3) {
      bloco = bloco >> 8*(3-j);
      soma += bloco;
    }
  }
  
  return soma;
}

//-------< global types >--------------------------------------------------

// Metricas
StatusPaciente statusPaciente;
StatusCiclo statusCiclo;

// Controle
ParametrosCicloControle paramCicloControle;
ParametrosAjusteControle paramAjusteControle;
EstadoControle estadoControle;

// PIDs
ParametrosPid paramPidFluxoAr;
ParametrosPid paramPidFluxoO2;
ParametrosPid paramPidPressaoPil;
ParametrosPid paramPidPressaoExp;
ParametrosPid paramPidPressaoIns;
EstadoPid estadoPidFluxoAr;
EstadoPid estadoPidFluxoO2;
EstadoPid estadoPidPressaoPil;
EstadoPid estadoPidPressaoExp;
EstadoPid estadoPidPressaoIns;

// Outros
StatusSensores statusSensores;
Medidas medidas;
Medidas medidasRasp;
DadosAutotestes dadosAutotestes;
ParametrosSensoresPressao paramSensoresPressao;
ParametrosSensorFluxo paramSensorFluxoAr;
ParametrosSensorFluxo paramSensorFluxoO2;
ParametrosSensorFluxo paramSensorFluxoEx;
ParametrosValvulaExalacao paramValvulaExalacao;
ParametrosValvulasAOP paramValvulasAOP;

//-------< global classes >--------------------------------------------------

// Camada da Estrategia
Respirador respirador;
Alarmes alarmes;
EstrategiaTopLayer estrategiaTop;
EstrategiaAC estrategiaAC;
EstrategiaPSV estrategiaPSV;
EntradaRasp entradaRasp;
SaidaRasp saidaRasp;
Metricas metricas;

// Camada do Controle
Controle controle;
ModoVCV modoVCV;
ModoPCV modoPCV;
SCFluxo scFluxo;
SCPressaoExp scPressaoExp;
SCPressaoIns scPressaoIns;
Pid pidFluxoAr;
Pid pidFluxoO2;
Pid pidPressaoExp;
Pid pidPressaoPil;
Pid pidPressaoIns;

// Outros
SimuladorLogico sim;
Sensores sensores;
Autotestes autotestes;
Valvula valvulaCal;
ComandosDegrauCalibracao paramCal;
ObjetosCalibrador objetosCal;
ModoGrafico grafico;
Ensaio ensaio;
