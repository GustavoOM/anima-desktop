#include "_global.h"

//#define VERBOSE

//----------------------------------------------------------------------
EstrategiaAC::EstrategiaAC()
{
	_state = INICIAL;
}

//----------------------------------------------------------------------
/*
 * inicia a estrategia com os parametros informados
 */
void EstrategiaAC::startCiclo(
	ParametrosVentilacao *params)
{

	//copia todos os parametros por valor para memoria local

	_localParams = *params;

	// recalcula todos os valores derivados, se precisar
	refreshLocalParameters();

	// tudo pronto. Segue para inicio da respiracao
	_state = INICIO_INSPIRACAO;
}

void EstrategiaAC::refreshLocalParameters()
{

	/*
   * Calcula variaveis derivadas. As que sao iguais usa diretamente o parametro
   */

  // pausa inspiratoria somente VCV. Seta 0 para prevenir
  if( _localParams.modo== MODO_PCV) _localParams.tPausaIns= 0;

	_tResp = 60000 / _localParams.freq;
	_tExp = _tResp - _localParams.tIns - _localParams.tPausaIns;

	//atualiza ciclo
	statusCiclo.modo = _localParams.modo;
}

//----------------------------------------------------------------------
void EstrategiaAC::loop()
{
	step();
}

//-----------------------------------------------------------------------------
void EstrategiaAC::step()
{
  unsigned int t= micros();
  unsigned int stateFlg= 0x0;

  // excepcionamente executa a manobra de chavear para a expiracao, 
  // caso esteja marcada a flag, e desliga a flag
  if( _switchToExpFlg) {
    _switchToExpFlg= false;

    // chaveia para a expiracao, caso esteja em 
    if( _state!= INICIAL && _state!= EM_EXPIRACAO) {
      	_state = INICIO_EXPIRACAO;
#if defined(VERBOSE) && !defined(ARDUINO_PLOTTER)
        SerialUSB.println( "EstrategiaAC: FORCA ESTADO PARA EXPIRACAO! ");
#endif
    }
  }

  // É esperado que o workflow vai parar num determinado estado, portanto a condicao
  // de state=_state vai ser true num periodo de tempo muito menor 
  // do que o tempo de loop global, que é TAMS. 
  // Mas para seguranca adcional (loop infinido) conta também 
  // o tempo corrido, e interrompe caso exceda esta condição.
	while (1) 
	{
    // armazena um estado em que ja esteve
    stateFlg |= _state;

		switch (_state)
		{
		case INICIAL:
			//nada a fazer..
			break;
		case INICIO_INSPIRACAO:
			actionForInicioInspiracao();
			break;
		case EM_INSPIRACAO:
			actionForEmInspiracao();
			break;
		case INICIO_PAUSA_INSPIRACAO:
			actionForInicioPausaInspiracao();
			break;
		case EM_PAUSA_INSPIRACAO:
			actionForEmPausaInspiracao();
			break;
		case INSPIRACAO_FINALIZADA:
			actionForInspiracaoFinalizada();
			break;
		case INICIO_EXPIRACAO:
			actionForInicioExpiracao();
			break;
		case EM_EXPIRACAO:
			actionForEmExpiracao();
			break;
		case EXPIRACAO_FINALIZADA:
			actionForExpiracaoFinalizada();
			break;
		}
    // se o novo estado é o mesmo de algum que ja foi executado, então abandona o 
    // controle do loop
		if ((stateFlg & _state)!= 0) {
			break;
    } 
	}
}

//-----------------------------------------------------------------------------
void EstrategiaAC::actionForInicioInspiracao()
{
	statusCiclo.tInicioIns = millis();

#if defined(VERBOSE) && !defined(ARDUINO_PLOTTER)
	SerialUSB.print("Inspira...(");
	SerialUSB.print(_localParams.tIns);
	SerialUSB.println(" ms)");
#endif
	controle.iniciaInspiracao(&_localParams);

	//muda estado
	_state = EM_INSPIRACAO;
	statusCiclo.fase = FASE_INS;
}

//-----------------------------------------------------------------------------
void EstrategiaAC::actionForEmInspiracao()
{

	//verifica se atingiu o tempo
	if (passouTempoMillis(statusCiclo.tInicioIns, _localParams.tIns))
	{
		if (_localParams.tPausaIns > 0)
		{
			_state = INICIO_PAUSA_INSPIRACAO;
		}
		else
		{
			_state = INSPIRACAO_FINALIZADA;
		}
	}
	else if (cicloDetected())
	{
		_state = INSPIRACAO_FINALIZADA;
	}
}

//-----------------------------------------------------------------------------
void EstrategiaAC::actionForInicioPausaInspiracao()
{
	statusCiclo.tInicioPausaIns = millis();
	controle.iniciaPausaIns();
	_state = EM_PAUSA_INSPIRACAO;

#if defined(VERBOSE) && !defined(ARDUINO_PLOTTER)
  SerialUSB.print( "INICIA PAUSA..");
  SerialUSB.println( _localParams.tPausaIns);
#endif
}

//-----------------------------------------------------------------------------
void EstrategiaAC::actionForEmPausaInspiracao()
{

	if (passouTempoMillis(statusCiclo.tInicioPausaIns, _localParams.tPausaIns))
	{
		// incluso no fluxo da expiracao
		_state = INSPIRACAO_FINALIZADA;
#if defined(VERBOSE) && !defined(ARDUINO_PLOTTER)
     SerialUSB.println( "FINALIZA PAUSA..");
#endif
	}
}

//-----------------------------------------------------------------------------
void EstrategiaAC::actionForInspiracaoFinalizada()
{
	_state = INICIO_EXPIRACAO;

#if defined(VERBOSE) && !defined(ARDUINO_PLOTTER)
	SerialUSB.print("Fim inspiracao. ^t=");
	SerialUSB.print(statusCiclo.tIns);
	SerialUSB.print(", vTidalIns= ");
	SerialUSB.print(statusCiclo.vTidallns);
	SerialUSB.println();
#endif
}

//-----------------------------------------------------------------------------
void EstrategiaAC::actionForInicioExpiracao()
{

	//reset do ciclo de respiracao
	statusCiclo.tInicioExp = millis();

	//sinaliza controle para inicio de inspiracao

#if defined(VERBOSE) && !defined(ARDUINO_PLOTTER)
	SerialUSB.print("Expira...(");
	SerialUSB.print(_tExp);
	SerialUSB.println(" ms)");
#endif
	controle.iniciaExpiracao();

	//muda estado para
	_state = EM_EXPIRACAO;
	statusCiclo.fase = FASE_EXP;
}

//-----------------------------------------------------------------------------
void EstrategiaAC::actionForEmExpiracao()
{
	//verifica se atingiu o tempo
	if (passouTempoMillis(statusCiclo.tInicioExp, _tExp))
	{
		/*
      * para a expiracacao
      */
		_triggerOnExit = MANDATORIO;
		_state = EXPIRACAO_FINALIZADA;
	}
	else if (triggerDetected())
	{
		/*
      * para a expiracacao e inicia a expiracao
      */
		_triggerOnExit = ESPONTANEO;
		_state = EXPIRACAO_FINALIZADA;
	}
}

void EstrategiaAC::actionForExpiracaoFinalizada()
{
	statusCiclo.tFimExp= millis();

#if defined(VERBOSE) && !defined(ARDUINO_PLOTTER)
	SerialUSB.print("Fim expiracao. ^t=");
	SerialUSB.print(statusCiclo.tExp);
	SerialUSB.print(" ms, vTidalExp= ");
	SerialUSB.print(statusCiclo.vTidalExp);
	SerialUSB.println();
#endif
}

bool EstrategiaAC::isFimDeCiclo(MandEspEnum *tipoTriggerProximo)
{
	if (_state == EXPIRACAO_FINALIZADA)
	{
		*tipoTriggerProximo = _triggerOnExit;
		return true;
	}
	else
	{
		return false;
	}
}
