#include "_global.h"


ModoVCV::ModoVCV() { }


void ModoVCV::loop() {
  switch (estadoControle.faseAtual) {

    case EM_INSPIRACAO:
      estadoControle.setFluxo = _geraSetpointFluxo();
      estadoControle.setFluxo *= _calculaCorrecaoSet();
      estadoControle.setPressaoExp = paramCicloControle.pressaoMax + 3.0;
      // utilizar a pressão limite como setpoint faz com que a válvula de exalação se
      // mantenha fechada durante a inspiração a menos que a pressão esteja muito alta
      break;

    case EM_PAUSA_INSPIRACAO:
      estadoControle.setFluxo = 0.0;
      estadoControle.setPressaoExp = paramCicloControle.pressaoMax + 3.0;
      break;

    case EM_EXPIRACAO:
      estadoControle.setFluxo = paramCicloControle.fluxoBase;
      estadoControle.setPressaoExp = paramCicloControle.peep;  
      break;
  }

  scFluxo.loop();
  scPressaoExp.loop();
}


/* Gera setpoint de fluxo na inspiração a partir de forma de onda setada pelo usuário,
de modo que a área da mesma corresponda ao volume solicitado. */
float ModoVCV::_geraSetpointFluxo() {
  // considera-se o instante da metade do período de amostragem como o tempo atual
	float tAtual = (estadoControle.contIterSetFluxo + 0.5) * (TAMS_PID / 1000.0);
  // pico de fluxo nas rampas ascendente/descendente
  float fluxoMaxReta = (paramCicloControle.volume * 2.0) / (paramCicloControle.tIns / 60.0);

  float setFluxo;
  // cálculo se dá por meio das equações no tempo das formas de onda
  switch (paramCicloControle.tipoCurva) {
    case FLUXO_ASCENDENTE:
      setFluxo = (fluxoMaxReta / paramCicloControle.tIns) * tAtual;
      break;

    case FLUXO_DESCENDENTE:
      setFluxo = - (fluxoMaxReta / paramCicloControle.tIns) * tAtual  +  fluxoMaxReta;
      break;

    case FLUXO_RETANGULAR:
      setFluxo = paramCicloControle.volume / (paramCicloControle.tIns / 60.0);
      break;

    case FLUXO_SENOIDAL:
      setFluxo = (paramCicloControle.volume / 2.0) * (3.1416 / paramCicloControle.tIns) * 60.0 *
                  sin((3.1416 / paramCicloControle.tIns) * tAtual);
  }

  // como a camada de Controle não controla o tempo, essa geração extrapola a equação da
  // forma de onda caso haja um atraso no chaveamento de fases pela Estratégia
  estadoControle.contIterSetFluxo++;
  return setFluxo;
}


/* Realiza correção do setpoint de fluxo na inspiração. */
float ModoVCV::_calculaCorrecaoSet() {
  if (estadoControle.faseAnterior != EM_INSPIRACAO) {
    if (estadoControle.faseAnterior == INICIAL) { // iteração inicial
      _correcaoSetInicial();
    } else { // transição exp/ins
      _correcaoSetCicloCiclo();
    }
  }
  
  return estadoControle.corrCCVolume;
}


/* Calcula fator de correção ciclo-a-ciclo na transição exp/ins em função da razão de
volume setado/medido. */
void ModoVCV::_correcaoSetCicloCiclo() {
  float razaoVolume;
  if (abs(statusPaciente.vTidalIns) > 0.01) { // evita divisão por 0
    razaoVolume = (paramCicloControle.volume * 1000.0) / statusPaciente.vTidalIns;
  } else {
    razaoVolume = 1.0;
  }

  // correção é aplicada de forma cumulativa
  float corrCalculada = razaoVolume * estadoControle.corrCCVolume *
                        paramAjusteControle.fatorCCVolume;
  // esse fatorCC é como se fosse um ganho para controlar a velocidade com que ela ocorre

  // saturação para evitar comportamentos extremos
  if (corrCalculada < paramAjusteControle.minCCVolume) {
    estadoControle.corrCCVolume = paramAjusteControle.minCCVolume;
  } else if (corrCalculada > paramAjusteControle.maxCCVolume) {
    estadoControle.corrCCVolume = paramAjusteControle.maxCCVolume;
  } else {
    estadoControle.corrCCVolume = corrCalculada;
  }
}

/* Calcula fator de correção inicial a partir da forma de onda utilizada e o tempo de
inspiração. */
void ModoVCV::_correcaoSetInicial() {
  // não foi testado!
  
  /* float tIns = paramCicloControle.tIns;
  //Constante de tempo do SCF
  float tau = 0.05;

  switch (paramCicloControle.tipoCurva) {
    case FLUXO_ASCENDENTE:
      estadoControle.corrCCVolume = -(tIns * tIns)/(2 * tau * tIns - (tIns * tIns) + 2 * tau * tau * math.exp(-tIns/tau) - 2 * tau * tau
      break;

    case FLUXO_RETANGULAR:
      estadoControle.corrCCVolume = tIns/(-tIns/(tIns + tau * math.exp(tau) - tau))
      break;
    default:
      estadoControle.corrCCVolume = 1.0; */
  
  estadoControle.corrCCVolume = 1.0;   
}
