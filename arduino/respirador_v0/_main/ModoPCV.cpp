#include "_global.h"


ModoPCV::ModoPCV() { }


void ModoPCV::loop() {
  switch (estadoControle.faseAtual) {

    case EM_INSPIRACAO:
      estadoControle.setPressaoIns = _geraSetpointPressaoIns();
      estadoControle.setPressaoIns *= _calculaCorrecaoSet();
      // ao contrário dos demais, o subsistema de controle de pressão inspiratória é
      // executado apenas na inspiração do modo PCV
      estadoControle.setFluxo = scPressaoIns.loop();
      estadoControle.setPressaoExp = paramCicloControle.pressaoCon + 3.0;
      // utilizar a pressão limite como setpoint faz com que a válvula de exalação se
      // mantenha fechada durante a inspiração a menos que a pressão esteja muito alta
      break;

    case EM_PAUSA_INSPIRACAO:
      estadoControle.setFluxo = 0.0; 
      estadoControle.setPressaoExp = paramCicloControle.pressaoCon + 3.0;
      break;

    case EM_EXPIRACAO:
      estadoControle.setFluxo = paramCicloControle.fluxoBase;
      estadoControle.setPressaoExp = paramCicloControle.peep;  
      break;
  }

  scFluxo.loop();
  scPressaoExp.loop();
}


/* Gera setpoint de pressão inspiratória construindo uma rampa ascendente até a pressão
de controle setada, tendo, essa rampa, um tempo de subida setado pelo usuário. */
float ModoPCV::_geraSetpointPressaoIns() {
  // considera-se o instante da metade do período de amostragem como o tempo atual
  float tAtual = (estadoControle.contIterSetPressaoIns + 0.5) * (TAMS_PID / 1000.0);

  float setPressaoIns;
  if (tAtual < paramCicloControle.tSubida) {
    float aux = (paramCicloControle.pressaoCon - paramCicloControle.peep) / paramCicloControle.tSubida;
    setPressaoIns = aux * tAtual  +  paramCicloControle.peep; 
  } else { // após o tempo de subida, setpoint é constante
    setPressaoIns = paramCicloControle.pressaoCon;
  }

  estadoControle.contIterSetPressaoIns++;
  return setPressaoIns;
}


/* Realiza correção do setpoint de pressão inspiratória. */
float ModoPCV::_calculaCorrecaoSet() {
  if (estadoControle.faseAnterior != EM_INSPIRACAO) { // transição exp/ins
    _correcaoSetCicloCiclo();
  }
  
  return estadoControle.corrCCPressao;
}


/* Calcula fator de correção ciclo-a-ciclo na transição exp/ins em função da razão de
pressão setada/medido. */
void ModoPCV::_correcaoSetCicloCiclo() {
  float razaoPressao;
  if (abs(statusPaciente.pPlateau50ms) > 0.01) { // evita divisão por 0
    razaoPressao =  paramCicloControle.pressaoCon / statusPaciente.pPlateau50ms;
  } else {
    razaoPressao = 1.0; 
  }

  // correção é aplicada de forma cumulativa
  float corrCalculada = razaoPressao * estadoControle.corrCCPressao *
                        paramAjusteControle.fatorCCPressao;
  // esse fatorCC é como se fosse um ganho para controlar a velocidade com que ela ocorre
  
  // saturação para evitar comportamentos extremos
  if (corrCalculada < paramAjusteControle.minCCPressao) {
    estadoControle.corrCCPressao = paramAjusteControle.minCCPressao;
  } else if (corrCalculada > paramAjusteControle.maxCCPressao) {
    estadoControle.corrCCPressao = paramAjusteControle.maxCCPressao;
  } else {
    estadoControle.corrCCPressao = corrCalculada;
  }
}
