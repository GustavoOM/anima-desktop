#include "_global.h"

//#define VERBOSE


EstrategiaTopLayer::EstrategiaTopLayer() { }

void EstrategiaTopLayer::setup()
{
  memset(&params, 0, sizeof(params));
  _trigger = MAND_ESP_UNDEFINED;
  _state= EstratTopLayerStates::INICIAL;
  _isStoppedFlg = true;
  _mustStopNowFlg = false;
  _fio2Fixo21 = false;
  _fio2Fixo100 = false;
  _restartBlocked = false;
}

void EstrategiaTopLayer::loop()
{
	step();
}

void EstrategiaTopLayer::reloadParamsIfExists()
{
  // copia por valor para variavel local
	params = entradaRasp.dadosRecebidosLoop.ventilacao;
}

void EstrategiaTopLayer::iniciaNovoCiclo()
{
  //calcula metricas de fim de ciclo
  metricas.atualizaStatusPacienteEmFimCiclo( _trigger);

	//zera dados do ciclo
	memset(&statusCiclo, 0, sizeof(statusCiclo));
	statusCiclo.mandEsp = _trigger;
  statusCiclo.pInsMinima= 9999; // inicializa
  statusCiclo.pExpMinima= 9999; // inicializa

  if (params.modo != MODO_PSV) {
    _runModoBackup = false;
  }

  /**
   * Inicia a estrategia, e ajusta os parametors se precisar
   */
	switch (params.modo)
	{
	case MODO_VCV:
	case MODO_PCV:
		_estrategia = &estrategiaAC;
		break;
	case MODO_PSV:
    _estrategia = &estrategiaPSV;

    /**
     * Caso esteja no modo de backup, sobrescreve os parametros
     * de ventilacao, com os valores que estao no modo backup.
     * Este ovewrite é feito somente nos parametros locais, usado 
     * na ventilacao. Note que no buffer de leitura os valores 
     * continuam conforme recebido pelo Rasp e a cada ciclo
     * o parametro local é sobrescrito pelos parametros no buffer 
     * de leitura.
     */
    if( _runModoBackup) {
#if defined(VERBOSE) && !defined(ARDUINO_PLOTTER)
      SerialUSB.println( "selecionado PSV, mas roda ciclo no MODO BACKUP ----");
#endif
  		_estrategia = &estrategiaAC;
      params.modo= params.bkModo;
      if( params.bkModo== MODO_VCV) {   // VCV
        params.vTidal= params.bkVTidal;
      } else {  // PCV
        params.pCon= params.bkPcon;
      }
      params.freq= params.bkFreq;
      params.tIns= params.bkTIns;
    }
		break;

	// Adicionar outro modos aqui quando houver...
	default:
		_estrategia = &estrategiaAC;
	}

  /**
   * Caso tenha sido ativado a manobra de fiO2, sobrescreve os parametros
   */ 

	_estrategia->startCiclo(&params);
}

void EstrategiaTopLayer::step()
{
  unsigned int t= micros();
  unsigned int stateFlg= 0x0;


  // É esperado que o workflow vai parar num determinado estado, portanto a condicao
  // de state=_state vai ser true num periodo de tempo muito menor 
  // do que o tempo de loop global, que é TAMS. 
  // Mas para seguranca adcional (loop infinido) conta também 
  // o tempo corrido, e interrompe caso exceda esta condição.
  while (!passouTempoMicros(t, TAMS*1000.0))
	{
    // armazena um estado em que ja esteve
    stateFlg |= (unsigned int)_state;

		switch (_state)
		{
		case EstratTopLayerStates::INICIAL:
			actionForInicial();
			break;

		case EstratTopLayerStates::INICIO_CICLO:
			actionForInicioCiclo();
			break;

		case EstratTopLayerStates::EM_CICLO:
			actionForEmCiclo();
			break;
		}

    // se o novo estado é o mesmo de algum que ja foi executado, então abandona o 
    // controle do loop
		if ((stateFlg & (unsigned int)_state)!= 0)  {
			break;
    } 
	}
}

/**
 * Este é o estado inicial em que o equipamenento ainda nao 
 * está em ventilacao. Ao receber o parametro é iniciado
 * a ventilacao. Caso contrario, permanece neste estado
 */
void EstrategiaTopLayer::actionForInicial()
{
  reloadParamsIfExists();
  if (!_isStoppedFlg) 
  {
    if (params.modo != 0)
    {
      memset( &statusPaciente, 0, sizeof( statusPaciente));
      statusPaciente.tInicioPaciente= millis();

      statusPaciente.faixaEtaria= params.faixaEtaria;
      statusPaciente.sexo= params.sexo;
      statusPaciente.altura= params.altura;
      statusPaciente.peso= params.peso;
      
      _state = EstratTopLayerStates::INICIO_CICLO;
    }
  }
}

/**
 * Este é o estado de inicio de cada ciclo
 */
void EstrategiaTopLayer::actionForInicioCiclo()
{
  if( _mustStopNowFlg) {  
#if defined(VERBOSE) && !defined(ARDUINO_PLOTTER)
    SerialUSB.println( "- - - - - - -  PARA respiracao!! - - - - - - -");
#endif
    stopVentilacao();
    _mustStopNowFlg = false;
    _isStoppedFlg = true;
		_state = EstratTopLayerStates::INICIAL;
    return;
  }

	iniciaNovoCiclo();
	_state = EstratTopLayerStates::EM_CICLO;
}

void EstrategiaTopLayer::actionForEmCiclo()
{
  //
  // se for detectado sinalizacao para modo backup, independente
  // do estagio de respiracao atual, chaveia para o modo backup
  // e inicia novo ciclo
  //
  if( _modoBackupFlg) {
#if defined(VERBOSE) && !defined(ARDUINO_PLOTTER)
    SerialUSB.println( "- - - - - - -  Detectado notificacao de modo backup. Reset ciclo - - - - - - -");
#endif
    _state= EstratTopLayerStates::INICIO_CICLO;
		// carrega os novos parametros se existir
		reloadParamsIfExists();
    _trigger= MANDATORIO;    

    // desflega a notificacao de backup
    _modoBackupFlg= false;
    // seta modo backup
    _runModoBackup= true;
    return;
  }


	_estrategia->loop();

	// verifica se terminou
	if (_estrategia->isFimDeCiclo(&_trigger))
	{
		_state = EstratTopLayerStates::INICIO_CICLO;
		// carrega os novos parametros se existir
		reloadParamsIfExists();
	}
	else
	{
		// aguarda...
	}
}

/***
 * Para a ventilacao até ter nova leitura de parametros com modo setado
 */
void EstrategiaTopLayer::notifyStopVentilacao(bool isFromAlarm) {
  _mustStopNowFlg= true;
  if (isFromAlarm) {
    _restartBlocked = true;
  }
}

void EstrategiaTopLayer::notifyRestartVentilacao() {
  if (!_restartBlocked) {
    _isStoppedFlg= false;
    _mustStopNowFlg = false; 
  }
}

void EstrategiaTopLayer::stopVentilacao() {
  //faz reset do modo de ventilacao corrente
  memset( &statusCiclo, 0, sizeof( statusCiclo));
  memset( &statusPaciente, 0, sizeof( statusPaciente));
  _trigger = MAND_ESP_UNDEFINED;
  _state= EstratTopLayerStates::INICIAL;

  // para o controle
  controle.paraRespiracao();
}


/**
 * Notifica para chaveamento imediato para modo backup.
 * Esta manobra é realizada no modo PSV quando detectado Apneia.
 * Dado que não foi detectado esforço inspiratório, a estrategia
 * deve chavear imediatamente para o modo backup e iniciar 
 * um novo ciclo, enviando ar.
 */
void EstrategiaTopLayer::notifyImediatoModoBackup() {
  _modoBackupFlg= true;
#if defined(VERBOSE) && !defined(ARDUINO_PLOTTER)
  SerialUSB.println( "Recebido notificacao de modo backup");
#endif
}

/**
 * Desliga o modo backup.
 * 
 * Esta acao é refletida no inicio do proximo ciclo.
 */
void EstrategiaTopLayer::notifyDesligaModoBackupNoProximoCiclo() {
  _runModoBackup= false;
#if defined(VERBOSE) && !defined(ARDUINO_PLOTTER)
  SerialUSB.println( "-- DESLIGA MODO BACKUP NO PROXIMO CICLO -- ");
#endif
}

/**
 * Chaveia para expiracao. A ser chamado em manobras de alarmes.
 * 
 */
void EstrategiaTopLayer::notifySwitchToExp() {
  if( _estrategia== NULL) return;
#if defined(VERBOSE) && !defined(ARDUINO_PLOTTER)
  SerialUSB.println( "Recebido notificacao de switch para exp ");
#endif
  _estrategia->notifySwitchToExp();
}

/**
 * Avisa que FiO2 tem que ser fixado em 20,9%
 * A ser chamado em caso de falha no fluxo de O2
 * 
 */
void EstrategiaTopLayer::notifyFiO2Fixo21() {
  _fio2Fixo21 = true;
  if (!_fio2Fixo100) {
    controle.setFiO2Fixo(FIO2_FIXO_21, true);
  } else {
    notifyStopVentilacao(true);
    valvulaSeguranca(true);
  }
}

void EstrategiaTopLayer::desativaFiO2Fixo21() {
  _fio2Fixo21 = false;
  controle.setFiO2Fixo(FIO2_FIXO_21, false);
}

/**
 * Avisa que FiO2 tem que ser fixado em 100%
 * A ser chamado em caso de falha no fluxo de ar
 * 
 */
void EstrategiaTopLayer::notifyFiO2Fixo100() {
  _fio2Fixo100 = true;
  if (!_fio2Fixo21) {
    controle.setFiO2Fixo(FIO2_FIXO_100, true);
  } else {
    notifyStopVentilacao(true);
    valvulaSeguranca(true);
  }
}

void EstrategiaTopLayer::desativaFiO2Fixo100() {
  _fio2Fixo100 = false;
  controle.setFiO2Fixo(FIO2_FIXO_100, false);
}

/**
 * Avisa que houve falha no sensor de fluxo expiratório, o que
 * acarreta na execução de diversas manobras
 * 
 */
void EstrategiaTopLayer::notifyFalhaSensorFluxoExp() {
  _falhaSensorFluxoExpflg = true;
}

/**
 * Retorna estado da estratégia ativa
 * 
 */
EstrategiaStatesEnum EstrategiaTopLayer::getState() {
  return _estrategia->getState();
}

bool EstrategiaTopLayer::isModoBackupAtivo() {
  return _runModoBackup;
}
