#include "_global.h"

//#define VERBOSE

//----------------------------------------------------------------------
EstrategiaPSV::EstrategiaPSV()
{
	_state = INICIAL;
}

//----------------------------------------------------------------------
/*
 * inicia a estrategia com os parametros informados
 */
void EstrategiaPSV::startCiclo(
	ParametrosVentilacao *params)
{

	//copia todos os parametros por valor para memoria local
	_localParams = *params;

	// recalcula todos os valores derivados, se precisar
	refreshLocalParameters();

	// tudo pronto. Segue para inicio da respiracao
	_state = INICIO_INSPIRACAO;
}

void EstrategiaPSV::refreshLocalParameters()
{
	//atualiza ciclo
	statusCiclo.modo = _localParams.modo;
}

//----------------------------------------------------------------------
void EstrategiaPSV::loop()
{
	step();
}

//-----------------------------------------------------------------------------
void EstrategiaPSV::step()
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
    }
  }


  // É esperado que o workflow vai parar num determinado estado, portanto a condicao
  // de state=_state vai ser true num periodo de tempo muito menor 
  // do que o tempo de loop global, que é TAMS. 
  // Mas para seguranca adcional (loop infinido) conta também 
  // o tempo corrido, e interrompe caso exceda esta condição.
	while (!passouTempoMicros(t, TAMS*1000.0))
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
			
		// case INICIO_PAUSA_INSPIRACAO: //Nao se aplica no PSV
		// case EM_PAUSA_INSPIRACAO: //Nao se aplica no PSV
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
		if ((stateFlg & _state)!= 0) 
			break;
	}
}

//-----------------------------------------------------------------------------
void EstrategiaPSV::actionForInicioInspiracao()
{
	/*
    * sinaliza controle para inicio de inspiracao com fluxo constante
    */
	statusCiclo.tInicioIns = millis();
#if defined(VERBOSE) && !defined(ARDUINO_PLOTTER)
	SerialUSB.println("Inspira...");
#endif
	controle.iniciaInspiracao(&_localParams);

	//muda estado
	_state = EM_INSPIRACAO;
	statusCiclo.fase = FASE_INS;
}

//-----------------------------------------------------------------------------
void EstrategiaPSV::actionForEmInspiracao()
{

	//verifica se atingiu o tempo limite máximo. O esperado é que a
	//expiracao espontânea inicie antes
	if (passouTempoMillis(statusCiclo.tInicioIns, _localParams.tImax))
	{
		_state = INSPIRACAO_FINALIZADA;
	}
	else if (cicloDetected())
	{
		_state = INSPIRACAO_FINALIZADA;
	}
}

//-----------------------------------------------------------------------------
void EstrategiaPSV::actionForInspiracaoFinalizada()
{
	_state = INICIO_EXPIRACAO;
}

//-----------------------------------------------------------------------------
void EstrategiaPSV::actionForInicioExpiracao()
{

	//reset do ciclo de respiracao
	statusCiclo.tInicioExp = millis();

	//sinaliza controle para inicio de inspiracao
#if defined(VERBOSE) && !defined(ARDUINO_PLOTTER)
	SerialUSB.println("Expira...");
#endif
	controle.iniciaExpiracao();

	//muda estado para
	_state = EM_EXPIRACAO;
	statusCiclo.fase = FASE_EXP;
}

//-----------------------------------------------------------------------------
void EstrategiaPSV::actionForEmExpiracao()
{
	// no PSV finaliza expiracao somente por trigger
	if (triggerDetected())
	{
	 	//para a expiracacao e inicia a expiracao
		_triggerOnExit = ESPONTANEO;
		_state = EXPIRACAO_FINALIZADA;
	}
}

void EstrategiaPSV::actionForExpiracaoFinalizada()
{
	statusCiclo.tFimExp= millis();
}

bool EstrategiaPSV::isFimDeCiclo(MandEspEnum *tipoTriggerProximo)
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
