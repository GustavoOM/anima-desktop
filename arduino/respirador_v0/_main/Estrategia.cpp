#include "_global.h"

//#define VERBOSE
// simula trigger no modo psv, porque o simulador de pulmao nao faz isso
// sem isso a respiracao fica eternamente na expiracao
//#define SIMULA_TRIGGER_PSV

#define DELAY_T_TRIGGER (200)
#define DELAY_T_CICLO (200)

// Tempo minimo em que tem que persistir o evento para ser considerado ciclo
#define DELTA_T_CICLO (50)   
// Tempo minimo em que tem que persistir o evento para ser considerado trigger
#define DELTA_T_TRIGGER (50)    
//----------------------------------------------------------------------


Estrategia::Estrategia() { }

/*
 * Verifica se há indicios de inspiração. Chamado somente durante expiracao.
 */
bool Estrategia::triggerDetected()
{
  if (statusCiclo.fase != FASE_EXP)
    return false;
  if (!passouTempoMillis(statusCiclo.tInicioExp, DELAY_T_TRIGGER))
    return false;

  /*
   * Trigger por pressao. 
   * 
   * Quando a pressao detectada for menor que peep - sensibilidade
   */

  if (_localParams.sensInsP < 0) {

    // pressao de trigger é a pressao de peep menos a sensibilidade, que é um
    // valor negativo
    float pressaoTrigger=  _localParams.peep+  _localParams.sensInsP;

    // se foi detectado o evento de ciclo por pressao..
    if( medidas.pressaoSis < pressaoTrigger)
    {
      if( statusCiclo.tInicioTriggerInsP> 0) {
        // se o evento dura continuamente por um delta T, então considera ciclo detectado
        // caso contrario, marca o inicio do evento para contagem
        if( passouTempoMillis(statusCiclo.tInicioTriggerInsP, DELTA_T_TRIGGER)) {
#if defined(VERBOSE) && !defined(ARDUINO_PLOTTER)
          SerialUSB.print("Trigger por pressao! Pressao= ");
          SerialUSB.println( medidas.pressaoSis);
#endif
          return true;
        } 
      } else {
#if defined(VERBOSE) && !defined(ARDUINO_PLOTTER)
        SerialUSB.print("Trigger event detectado. Marca... ");
        SerialUSB.println( medidas.pressaoSis);
#endif
        statusCiclo.tInicioTriggerInsP= millis();
      }
    } else {
      statusCiclo.tInicioTriggerInsP= 0;
    }
  }

  /*
   * Trigger por fluxo. 
   * 
   * Quando fluxo é maior que o fluxo indicado.
   * Para modo VNI não tem sensibilidade por fluxo.
   */
  if (_localParams.sensInpF > 0) {
    float fluxo = medidas.fluxoPac;

    // se foi detectado o evento de ciclo por fluxo..
    if( fluxo> _localParams.sensInpF) 
    {
      if( statusCiclo.tInicioTriggerInsF> 0) {
        // se o evento dura continuamente por um delta T, então considera ciclo detectado
        // caso contrario, marca o inicio do evento para contagem
        if( passouTempoMillis(statusCiclo.tInicioTriggerInsF, DELTA_T_TRIGGER)) {
#if defined(VERBOSE) && !defined(ARDUINO_PLOTTER)
          SerialUSB.print("Trigger por fluxo! Fluxo= ");
          SerialUSB.println( fluxo);
#endif
          return true;
        } 
      } else {
        statusCiclo.tInicioTriggerInsF= millis();
      }
    } else {
      statusCiclo.tInicioTriggerInsF= 0;
    }
  }

#ifdef SIMULA_TRIGGER_PSV
  // simula trigger no PSV, por tempo e fluxo
  if (_localParams.modo == MODO_PSV)
  {
    float fluxo = medidas.fluxoPac;
    if( fluxo>= -2.0) {
#if defined(VERBOSE) && !defined(ARDUINO_PLOTTER)
      SerialUSB.print("Trigger por fluxo proximo a zero (provisorio)! fluxo= ");
      SerialUSB.println( fluxo);
#endif
      return true;
    }
    // forca depois de 10s
    if (passouTempoMillis(statusCiclo.tInicioExp, 10000)) {
#if defined(VERBOSE) && !defined(ARDUINO_PLOTTER)
      SerialUSB.print("Trigger por limite de tempo de 10s (provisorio)! fluxo= ");
      SerialUSB.println( fluxo);
#endif
      return true;
    }
  }
#endif
  return false;
}

bool Estrategia::isModoPsvOuDerivado( ModosEnum modo) {
  return modo== MODO_PSV 
      || modo== MODO_SIMV_VCV_PSV 
      || modo== MODO_SIMV_PCV_PSV;
}


//----------------------------------------------------------------------
/*
 * Verifica se há indicios de expiracao. Chamado somente durante inspiracao.
 * Valido somente para modos que incluir PSV
 */
bool Estrategia::cicloDetected()
{
  if( !isModoPsvOuDerivado( statusCiclo.modo)) 
    return false;
  if (statusCiclo.fase != FASE_INS)
    return false;
  if (!passouTempoMillis(statusCiclo.tInicioIns, DELAY_T_CICLO))
    return false;

  /*
   * ciclo por pressao. Quando o ciclo é menor do que o percentual indicado da
   * pressao de pico
   */
  if (_localParams.sensExpF > 0 && statusCiclo.fluxoInspPico > 0)
  {
    double fluxo = medidas.fluxoPac;
    double fluxoDeTrigger = _localParams.sensExpF * statusCiclo.fluxoInspPico;
    if (fluxo < fluxoDeTrigger)
    {
      if( statusCiclo.tInicioTriggerExp> 0) {
        // se o evento dura continuamente por um delta T, então considera ciclo detectado
        // caso contrario, marca o inicio do evento para contagem
        if( passouTempoMillis(statusCiclo.tInicioTriggerExp, DELTA_T_CICLO)) {
#if defined(VERBOSE) && !defined(ARDUINO_PLOTTER)
          SerialUSB.print("Ciclo por fluxo! Fluxo= ");
          SerialUSB.println( fluxo);
#endif
          return true;
        } 
      } else {
        statusCiclo.tInicioTriggerExp= millis();
      }
    } else {
      statusCiclo.tInicioTriggerExp= 0;
    }
  }
  return false;
}



/**
 * Recebe notificacao par chavear imediatamente para expiracao.
 * 
 * Marca o flag local e na proxima chamada ao loop executa a manobra.
 */
void Estrategia::notifySwitchToExp() {
  _switchToExpFlg= true;
#if defined(VERBOSE) && !defined(ARDUINO_PLOTTER)
  SerialUSB.println( "Estrategia: Recebido notificacao de switch para exp ");
#endif
}



EstrategiaStatesEnum Estrategia::getState() {
  return _state;
}
