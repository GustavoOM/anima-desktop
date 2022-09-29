#include "_global.h"

//#define VERBOSE

//Pressao considerada como fluxo zero
#define FLUXO_ZERO (5.0)

//tempo minimo para considerar como plateau, em ms
#define DELTA_T_PLATEAU (100)
#define DELTA_T_PLATEAU_EXP (50)

#define DELAY_T_PEEP_EXP (200) //Provisorio


void Metricas::setup() {
  _tLastLoop = millis();
	_tCurLoop= _tLastLoop;
}

void Metricas::calculaMatricasEachLoop() {
	_tLastLoop= _tCurLoop;
	_tCurLoop= millis();

  statusCiclo.fluxo= medidas.fluxoPac;

	// volume tidal
	statusCiclo.vTidal+= 1000*medidas.fluxoPac* (_tCurLoop-_tLastLoop)/60000; // [mililitros]
	statusCiclo.vTidalInsO2+= 1000*(medidas.fluxoO2)* (_tCurLoop-_tLastLoop)/60000; // [mililitros]
	statusCiclo.vTidalInsAr+= 1000*(medidas.fluxoAr)* (_tCurLoop-_tLastLoop)/60000; // [mililitros]

  if( medidas.fluxoIns > medidas.fluxoExp) {
  	statusCiclo.vTidallns+= 1000*medidas.fluxoPac* (_tCurLoop-_tLastLoop)/60000; // [mililitros]
  }
  if( medidas.fluxoIns< medidas.fluxoExp) {
  	statusCiclo.vTidalExp+= 1000*(0-medidas.fluxoPac)* (_tCurLoop-_tLastLoop)/60000; // [mililitros]
  }

	// pressao de pico
  if( medidas.pressaoSis> statusCiclo.pPico) {
      statusCiclo.pPico= medidas.pressaoSis;
  }
  if( medidas.pressaoIns> statusCiclo.pPicoIns) {
      statusCiclo.pPicoIns= medidas.pressaoIns;
  }
	if( statusCiclo.fase== FASE_EXP && medidas.pressaoSis< statusCiclo.pExpMinima)  {
		statusCiclo.pExpMinima= medidas.pressaoSis;
	}
	if( statusCiclo.fase== FASE_INS && medidas.pressaoSis< statusCiclo.pInsMinima)  {
		statusCiclo.pInsMinima= medidas.pressaoSis;
	}

  // fluxo de pico
  // considera quando a curva comecar a cair, para não pegar pico ainda durante o aumento do fluxo
	if( statusCiclo.fase== FASE_INS && statusCiclo.fluxo> statusCiclo.fluxoInspPico)  {
		statusCiclo.fluxoInspPico= statusCiclo.fluxo;
	}
	if( statusCiclo.fase== FASE_EXP && statusCiclo.fluxo< statusCiclo.fluxoExpPico)  {
		statusCiclo.fluxoExpPico=  statusCiclo.fluxo;
	}

	// pressao de plateau e complacencia
	// Se ainda nao calculou plateau e esta em inspiracao, considera..
	if( statusCiclo.fase == FASE_INS ) {
		
		// se fluxo==0?
		if( abs( medidas.fluxoPac)< FLUXO_ZERO) {

			// se eh a primeira vez, somente sinaliza
			if( statusCiclo.tInicioFluxoZero== 0) {
				statusCiclo.tInicioFluxoZero= millis();

			// se ja esta neste estado por um delta, entao considera como plateau
      } else if( passouTempoMillis(statusCiclo.tInicioFluxoZero, DELTA_T_PLATEAU)) {
        if( statusCiclo.pPlateau== 0)
  				statusCiclo.pPlateau= medidas.pressaoSis;
        else 
  				statusCiclo.pPlateau= min( statusCiclo.pPlateau, medidas.pressaoSis);
			}
		} else {
			// faz reset do inicio de fluxo zero
			statusCiclo.tInicioFluxoZero= 0;
		}
	}

  // Calcula plateau do fim da última inspiração
  if( statusCiclo.fase == FASE_EXP) {
    statusCiclo.pPlateau50ms= statusCiclo.histPIns.getMediaUltimosTms( 50);
  }
  
	// PEEP total
  // Calcula sempre que fluxo = 0 na fase da expiracao, poorque a pressao pode ir diminuindo 
	if( statusCiclo.fase == FASE_EXP && passouTempoMillis(statusCiclo.tInicioExp, DELAY_T_PEEP_EXP)) {

		// se fluxo==0?
		if( abs( medidas.fluxoPac)< FLUXO_ZERO) {

			// se eh a primeira vez, somente sinaliza
			if( statusCiclo.tInicioFluxoZeroExp== 0) {
				statusCiclo.tInicioFluxoZeroExp= millis();

			// se ja esta neste estado por um delta, entao considera como plateau
      } else if( passouTempoMillis(statusCiclo.tInicioFluxoZeroExp, DELTA_T_PLATEAU_EXP)) {
        if( statusCiclo.peepTotal> 0)
  				statusCiclo.peepTotal= min( statusCiclo.peepTotal, medidas.pressaoSis);
        else
  				statusCiclo.peepTotal= medidas.pressaoSis;
        // fonte => https://www.hamilton-medical.com/ja/E-Learning-and-Education/Knowledge-Base/Knowledge-Base-Detail~2019-02-04~Measurement-of-AutoPEEP-and-total-PEEP~dd4c4ca2-feaf-44c8-a033-a0a250b29d4e~.html#DataTables_Table_0=od3
        statusCiclo.autoPeep= max( 0, statusCiclo.peepTotal - estrategiaTop.params.peep);
			}
		} else {
			// faz reset do inicio de fluxo zero
			statusCiclo.tInicioFluxoZeroExp= 0;
		}
	}

	// Soma a pressao ponderada pelo tempo para tirar a media ao final
	statusCiclo.pTotal= statusCiclo.pTotal+ medidas.pressaoSis* (_tCurLoop- max( statusCiclo.tInicioIns, _tLastLoop));

  /**
   * Historico de pressao
   */
  if( statusCiclo.fase== FASE_INS) {
    statusCiclo.histPIns.add( _tCurLoop, medidas.pressaoSis);
  } else if( statusCiclo.fase== FASE_EXP) {
    statusCiclo.histPExp.add( _tCurLoop, medidas.pressaoSis);
  }
}



/**
 * Calcula mas metricas em final de ciclo. Esta rotina é chamada pela camada da estrategia ao 
 * final de cada ciclo
 */
void Metricas::atualizaStatusPacienteEmFimCiclo( MandEspEnum trigger) {

  unsigned int t= millis();

  if( statusPaciente.tInicioPaciente== 0) statusPaciente.tInicioPaciente= t;

	// atualiza dados do paciente a partir do ciclo
	statusPaciente.fluxoInspPico = statusCiclo.fluxoInspPico;
	statusPaciente.pPico = statusCiclo.pPico;
	statusPaciente.pBase = min( statusCiclo.pInsMinima, statusCiclo.pExpMinima);
	statusPaciente.pInsMinima = statusCiclo.pInsMinima;
	statusPaciente.pExpMinima = statusCiclo.pExpMinima;
  statusPaciente.peep= statusCiclo.pExpMinima;
  statusPaciente.peepTotal= statusCiclo.peepTotal;
  statusPaciente.autoPeep= statusCiclo.autoPeep;
  if( statusCiclo.tInicioPausaIns> 0)
  	statusPaciente.tIns = statusCiclo.tInicioPausaIns - statusCiclo.tInicioIns;
  else
  	statusPaciente.tIns = statusCiclo.tInicioExp - statusCiclo.tInicioIns;
	statusPaciente.tExp = statusCiclo.tFimExp - statusCiclo.tInicioExp;
	if( statusPaciente.tIns> 0) {
		statusPaciente.relTempIE = (double)statusPaciente.tExp/ (double)statusPaciente.tIns;
	} else {
    statusPaciente.relTempIE= 0;
  }
  statusPaciente.fio2= (1.429 * statusCiclo.vTidalInsO2 + 0.209*statusCiclo.vTidalInsAr*1.293 )/ 
                       (1.429 * statusCiclo.vTidalInsO2 + statusCiclo.vTidalInsAr*1.293);
	statusPaciente.pPlateau = statusCiclo.pPlateau;

  // se tiver pressao de plateau, calcula a complacencia e a resistencia
  // fonte: https://journals.aboutscience.eu/index.php/aboutopen/article/view/300/477

  if( statusCiclo.pPlateau!= 0) {
    statusPaciente.rinsp = (statusCiclo.pPico - statusCiclo.pPlateau)/(statusCiclo.fluxoInspPico/60);
    statusPaciente.rexp = (statusCiclo.pPico - statusCiclo.pPlateau)/(statusCiclo.fluxoInspPico/60);
  } else {
    statusPaciente.rinsp= 0;
    statusPaciente.rexp= 0;
  }
  statusPaciente.cdyn =statusCiclo.vTidallns/(statusCiclo.pPico-statusPaciente.peep);
	if( statusCiclo.tFimExp!= statusCiclo.tInicioIns) {
		statusPaciente.pMed= statusCiclo.pTotal/(statusCiclo.tFimExp- statusCiclo.tInicioIns);
	} else {
    statusPaciente.pMed= 0;
  }

  // guarda o numero de triggers espontaneos seguidos
  if( trigger== ESPONTANEO) {
    statusPaciente.nTriggerEspConsecut+= 1;
  } else {
    statusPaciente.nTriggerEspConsecut= 0;
  }
#if defined(VERBOSE) && !defined(ARDUINO_PLOTTER)
	SerialUSB.print("statusPaciente.nTriggerEspConsecut= ");
	SerialUSB.print(statusPaciente.nTriggerEspConsecut);
	SerialUSB.println();
#endif
	statusPaciente.vTidalIns = statusCiclo.vTidallns;
	statusPaciente.vTidalExp = statusCiclo.vTidalExp;

  /**
   * Atualiza historico
   */
  // adiciona ao final da lista, e desloca inicio de a lista estiver cheia
  statusPaciente.hist[ statusPaciente.hFim].t= statusCiclo.tInicioIns;
  statusPaciente.hist[ statusPaciente.hFim].vIns= statusCiclo.vTidallns;
  statusPaciente.hist[ statusPaciente.hFim].vExp= statusCiclo.vTidalExp;
  statusPaciente.hFim= (statusPaciente.hFim+1) % MAX_CICLO_PACIENTE;

  // se buffer cheio (iFim=iInicio), entao remove o primeiro item
  if( statusPaciente.hFim == statusPaciente.hInicio) {
    statusPaciente.hInicio= (statusPaciente.hInicio+1) % MAX_CICLO_PACIENTE;
  }

  // desloca o inico para remover os fora da janela de 1 minuto
  unsigned int tInicioMinuto= t > 60000? t- 60000: 0;
  
  while( statusPaciente.hInicio!= statusPaciente.hFim &&    // buffer vazio
         statusPaciente.hist[statusPaciente.hInicio].t< tInicioMinuto) {  // fora da janela de 1 minuto
      
    statusPaciente.hInicio= (statusPaciente.hInicio+1) % MAX_CICLO_PACIENTE;
  }
  
  /**
   * Atualiza valores por minuto
   */
  statusPaciente.vMinExp= 0;
  statusPaciente.vMinIns= 0;
  if( t>= (statusPaciente.tInicioPaciente + 60000)) {
    for( int i= statusPaciente.hInicio; i!= statusPaciente.hFim; i= (i+1)%MAX_CICLO_PACIENTE) {
      statusPaciente.vMinExp+= statusPaciente.hist[ i].vExp;
      statusPaciente.vMinIns+= statusPaciente.hist[ i].vIns;
    }
  }
  statusPaciente.freq = round((float) 60000/(t - statusCiclo.tInicioIns));

  /**
   *  calcula a pressao de plateau e peep, calculado como sendo a media dos ultimos 50ms 
   * ao final da inspiracao e expiracao
   */
  statusPaciente.pPlateau50ms= statusCiclo.histPIns.getMediaUltimosTms( 50);
  statusPaciente.peep50ms= statusCiclo.histPExp.getMediaUltimosTms( 50);

  if( estrategiaTop.params.tPausaIns > 0) {
    statusPaciente.cstat =statusPaciente.vTidalIns/(statusPaciente.pPlateau50ms - statusPaciente.peep50ms);
  } else {
    statusPaciente.cstat= 0;
  }
}

void Metricas::loop() {
  calculaMatricasEachLoop();
}
