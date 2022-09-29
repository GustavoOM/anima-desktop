#include "_global.h"

#ifdef CALIBRA_AR_O2_PIL

unsigned long clockIter;
bool loopa = true;
int contIteracoes = 1;

void setupCalibArO2Pil() {
  setupMaquina(); 
  setupGraficoCalibrador();

  memset(&paramCal, 0, sizeof(ComandosDegrauCalibracao));
  memset(&objetosCal, 0, sizeof(ObjetosCalibrador));

  scFluxo.setup();
  scPressaoExp.setup();
  
  paramCal.valvula = 0;
  paramCal.periodoDeg = 1000;
  paramCal.valor1Deg = 0.0;
  paramCal.valor2Deg = 10.0;
  objetosCal.valvulaAnterior = 1;
  entradaRasp.setup();
  entradaRasp.recebeParametrosInicializacao();
}

void loopCalibArO2Pil() {
  clockIter = micros();

  controle.recarregaParametrosControle();
  carregaComandosDegrau();

  if (loopa) {
    sensores.leSensores();
    verificaValvula();

    if (contIteracoes == NUM_ITER) {
      switch (paramCal.valvula) {
        case 0: // AR
          objetosCal.estadoPid->sensorAtual = statusSensores.valorFluxo[1];
          valvulaCal.loop();
          valvulaFluxoArVP1(objetosCal.estadoPid->saidaAtual);
          break;

        case 1: // O2
          objetosCal.estadoPid->sensorAtual = statusSensores.valorFluxo[2];
          valvulaCal.loop();
          valvulaFluxoOxVP2(objetosCal.estadoPid->saidaAtual);
          break;

        case 3: // PILOTO
          objetosCal.estadoPid->sensorAtual = statusSensores.valorPressao[5];
          valvulaCal.loop();
          valvulaOxExalacaoVP3(objetosCal.estadoPid->saidaAtual);
          break;
      }
    #ifdef ARDUINO_PLOTTER
      loopGrafico();
    #endif
    }
  }

  entradaRasp.loop();

  verificaComandosGrafico();
  verificaParadaGrafico();
  while (micros() - clockIter < TAMS*1000.0);
  contIteracoes++;
  if (contIteracoes == round(TAMS_PID/TAMS + 1)) { contIteracoes = 1; }
}

void setupGraficoCalibrador() {
  memset(&grafico, 0, sizeof(ModoGrafico));

  grafico.curvasAtuais[0] = 22;
  grafico.curvasAtuais[1] = 23;

  grafico.pidRef = objetosCal.estadoPid;
}

void verificaValvula() {
  if (paramCal.valvula != objetosCal.valvulaAnterior) {
    switch (paramCal.valvula) {
      case 0:
        objetosCal.pid = &pidFluxoAr;
        objetosCal.paramPid = &paramPidFluxoAr;
        objetosCal.estadoPid = &estadoPidFluxoAr;
        break;

      case 1:
        objetosCal.pid = &pidFluxoO2;
        objetosCal.paramPid = &paramPidFluxoO2;
        objetosCal.estadoPid = &estadoPidFluxoO2;
        break;

      case 3:
        objetosCal.pid = &pidPressaoPil;
        objetosCal.paramPid = &paramPidPressaoPil;
        objetosCal.estadoPid = &estadoPidPressaoPil;
        break;
    }

    grafico.pidRef = objetosCal.estadoPid;
    objetosCal.valvulaAnterior = paramCal.valvula;
    
    valvulaFluxoArVP1(0);
    valvulaFluxoOxVP2(0);
    valvulaOxExalacaoVP3(0);
  }
}

void carregaComandosDegrau() {
  paramCal = entradaRasp.dadosRecebidosLoop.degrau;
}

#endif
