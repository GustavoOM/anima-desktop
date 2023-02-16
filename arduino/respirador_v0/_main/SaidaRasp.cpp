#include "_global.h"
#include "limits.h"

//#define VERBOSE

void SaidaRasp::setup() {
  memset(_buffer, 0, sizeof(_buffer));
  _tUltimoGraficosAlarmes = millis();
  _tUltimoIndicadores = millis();
}

void SaidaRasp::loop() {
  if (passouTempoMillis(_tUltimoGraficosAlarmes, 100)) {
    _enviaDadosGraficosAlarmes();
    _tUltimoGraficosAlarmes = millis();
  }
  else if (passouTempoMillis(_tUltimoIndicadores, 400)) {
    _enviaDadosIndicadores();
    _tUltimoIndicadores = millis();
  }
}

char* SaidaRasp::_adiLongNoBuffer(char* ponteiroBuf, long valor) {
  return _itochar(ponteiroBuf, valor);
}

char* SaidaRasp::_adiUintNoBuffer(char* ponteiroBuf, unsigned int valor) {
  return _utochar(ponteiroBuf, valor);
}

char* SaidaRasp::_adiFloatNoBuffer(char* ponteiroBuf, float valor) {
  if (!isinf(valor) && !isnan(valor)) {
    ponteiroBuf = _ftochar(ponteiroBuf, valor);
  } else {
    *ponteiroBuf++= ',';
  }
  return ponteiroBuf;
}

char* SaidaRasp::_iniCabecalhoNoBuffer(MsgSaidaEnum tipoMsg) {
  memset(_buffer, 0, sizeof(_buffer));
  char *p = _buffer;
  unsigned long tempo = millis();
  
  sprintf(p, "^%02d,1,", int(tipoMsg));
  p += 6;
  p = _adiLongNoBuffer(p, tempo);
  
  return p;
}

void SaidaRasp::_finRodapeNoBuffer(char* p) {
  /*
  int posVirguChecksum = int(p) - int(_buffer) - 1;
  unsigned long checksum = calculaChecksum(_buffer, posVirguChecksum);

  p = _adiUintNoBuffer(p, checksum);
  *(p-1)= ';';
  *p++= '\0';
  */
}

void SaidaRasp::_enviaMensagem() {

}

void SaidaRasp::enviaResposta(unsigned int idMsg) {
  char* p = _iniCabecalhoNoBuffer(MSG_RESPOSTA);

  p = _adiUintNoBuffer(p, idMsg);

  _finRodapeNoBuffer(p);
  _enviaMensagem();
}

void SaidaRasp::_enviaDadosGraficosAlarmes() {
  char* p = _iniCabecalhoNoBuffer(MSG_GRAFICOS_ALARMES);
  
  p = _adiFloatNoBuffer(p, medidasRasp.fluxoIns);
  p = _adiFloatNoBuffer(p, medidasRasp.fluxoExp);
  p = _adiFloatNoBuffer(p, medidasRasp.pressaoSis);
  p = _adiFloatNoBuffer(p, statusCiclo.vTidal);
  p = _adiUintNoBuffer(p, statusCiclo.fase);
  p = _adiUintNoBuffer(p, alarmes.geraNumeroEnvioRasp());

  _finRodapeNoBuffer(p);
  _enviaMensagem();
}

void SaidaRasp::_enviaDadosIndicadores() {
  char* p = _iniCabecalhoNoBuffer(MSG_INDICADORES);

  p = _adiFloatNoBuffer(p, statusPaciente.pPlateau50ms);
  p = _adiFloatNoBuffer(p, statusPaciente.pPico);
  p = _adiFloatNoBuffer(p, statusPaciente.freq);
  p = _adiFloatNoBuffer(p, statusPaciente.tIns);
  p = _adiUintNoBuffer(p, statusPaciente.vTidalIns);
  p = _adiFloatNoBuffer(p, statusPaciente.vMinIns);
  p = _adiFloatNoBuffer(p, statusPaciente.peep50ms);
  p = _adiFloatNoBuffer(p, statusPaciente.fio2);
  p = _adiFloatNoBuffer(p, statusPaciente.cstat);
  p = _adiFloatNoBuffer(p, statusPaciente.pMed);
  p = _adiFloatNoBuffer(p, statusPaciente.cdyn);
  p = _adiFloatNoBuffer(p, statusSensores.valorPressao[1]);
  p = _adiFloatNoBuffer(p, statusSensores.valorPressao[2]);
  
  _finRodapeNoBuffer(p);
  _enviaMensagem();
}

void SaidaRasp::enviaResultadoAutotestes(AutotestesEnum comando, int falha) {
  char* p = _iniCabecalhoNoBuffer(MSG_RESUL_AUTOTESTES);

  p = _adiUintNoBuffer(p, comando);
  p = _adiLongNoBuffer(p, falha);

  _finRodapeNoBuffer(p);
  _enviaMensagem();
}

void SaidaRasp::enviaResultadoCalibracaoFluxoEx(int progresso) {
  char* p = _iniCabecalhoNoBuffer(MSG_RESUL_CALIB_FLUXEX);

  p = _adiUintNoBuffer(p, progresso);
  
  _finRodapeNoBuffer(p);
  _enviaMensagem();
}

void SaidaRasp::enviaParametrosSensoresPressao(ParametrosSensoresPressao dadosCalib) {
  char* p = _iniCabecalhoNoBuffer(MSG_PARAM_SENSORES_PRESSAO);  

  for (int i=0; i<3; i++) {
    p = _adiLongNoBuffer(p, dadosCalib.minAdcNoZero[i]);
  }
  for (int i=0; i<3; i++) {
    p = _adiLongNoBuffer(p, dadosCalib.maxAdcNoZero[i]);
  }

  _finRodapeNoBuffer(p);
  _enviaMensagem();
}

void SaidaRasp::enviaParametrosSensorFluxo(SensFluxEnum sensFlux, ParametrosSensorFluxo dadosCalib) {
  char* p = _iniCabecalhoNoBuffer(MSG_PARAM_SENSOR_FLUXO);

  p = _adiUintNoBuffer(p, (long) sensFlux);
  for (int i=0; i<18; i++) {
    p = _adiLongNoBuffer(p, dadosCalib.curvaAdc[i]);
  }
  for (int i=0; i<18; i++) {
    p = _adiFloatNoBuffer(p, dadosCalib.curvaFluxo[i]);
  }
  p = _adiLongNoBuffer(p, dadosCalib.minAdcNoZero);
  p = _adiLongNoBuffer(p, dadosCalib.maxAdcNoZero);

  _finRodapeNoBuffer(p);
  _enviaMensagem();
}

void SaidaRasp::enviaParametrosValvulaExalacao(ParametrosValvulaExalacao dadosCalib) {
  char* p = _iniCabecalhoNoBuffer(MSG_PARAM_VALVULA_EXALACAO);

  for (int i=0; i<11; i++) {
    p = _adiFloatNoBuffer(p, dadosCalib.curvaSistema[i]);
  }
  for (int i=0; i<11; i++) {
    p = _adiFloatNoBuffer(p, dadosCalib.curvaPiloto[i]);
  }
  for (int i=0; i<9; i++) {
    p = _adiFloatNoBuffer(p, dadosCalib.limiaresSistema[i]);
  }
  for (int i=0; i<9; i++) {
    p = _adiFloatNoBuffer(p, dadosCalib.minPilotoNoLimiarSistema[i]);
  }
  for (int i=0; i<9; i++) {
    p = _adiFloatNoBuffer(p, dadosCalib.maxPilotoNoLimiarSistema[i]);
  }
  p = _adiFloatNoBuffer(p, dadosCalib.minPressaoPicoSistema);
  
  _finRodapeNoBuffer(p);
  _enviaMensagem();
}

void SaidaRasp::enviaParametrosValvulasAOP(ParametrosValvulasAOP dadosCalib) {
  char* p = _iniCabecalhoNoBuffer(MSG_PARAM_VALVULAS_AOP);

  for (int i=0; i<3; i++) {
    p = _adiUintNoBuffer(p, dadosCalib.pwmAberturaMin[i]);
  }
  for (int i=0; i<3; i++) {
    p = _adiUintNoBuffer(p, dadosCalib.pwmAberturaMax[i]);
  }
  for (int i=0; i<3; i++) {
    p = _adiUintNoBuffer(p, dadosCalib.pwmFechamentoMin[i]);
  }
  for (int i=0; i<3; i++) {
    p = _adiUintNoBuffer(p, dadosCalib.pwmFechamentoMax[i]);
  }
  for (int i=0; i<3; i++) {
    p = _adiFloatNoBuffer(p, dadosCalib.saidaPicoMin[i]);
  }
  for (int i=0; i<3; i++) {
    p = _adiFloatNoBuffer(p, dadosCalib.saidaPicoMax[i]);
  }
  p = _adiUintNoBuffer(p, dadosCalib.tempoMaxValvSeguranca);

  _finRodapeNoBuffer(p);
  _enviaMensagem();
}

void SaidaRasp::enviaTexto(char* texto) {
#ifdef RESPIRADOR
#ifndef ARDUINO_PLOTTER
#endif
  return;
#endif

  _iniCabecalhoNoBuffer(TEXTO);

  strcat(_buffer, texto);
  strcat(_buffer, ";");
}

/**
 * Converte um valor inteiro para string, e move
 * o ponteiro de escrita para a proxima posicao 
 * a escrever. Também adiciona ',' ao final
 * 
 * Não vai funcionar para INT_MIN (-2147483648), mas é 
 * esperado que não vamos ter este valor. 
 * 
 * Não é colocado \0 ao final do buffer
 */
char* SaidaRasp::_itochar( char *p, long d) {
  if( isinf( d) || isnan(d) || d== INT_MIN) {
    *p++= ',';
    return p;
  }
  if( d== 0) {
    *p++= '0';
    *p++= ',';
    return p;
  }
  if( d< 0) {
    *p++= '-';
    d*= -1;
  }
  char *q= tmpb;
  while( d> 0) {
    *q++= '0'+ d%10;
    d= d/10;
  }
  for( q--;; q--) {
    *p++= *q;
    if( q== tmpb) break;
  }
  *p++= ',';
  return p;
}


/**
 * Converte um valor float para string, e move
 * o ponteiro de escrita para a proxima posicao 
 * a escrever. Também adiciona ',' ao final.
 * 
 * Não é colocado \0 ao final do buffer
 */
char* SaidaRasp::_ftochar( char *p, float f) {
  // sem cada decimal, converte como int
  if( f== (long)f && f > 1.0) {
    return _itochar( p, (long)f);
  }
  if( isinf( f) || isnan(f)) {
    *p++= ',';
    return p;
  }
  long d= (long)(f*100);
  if( d== 0) {
    *p++= '0';
    *p++= ',';
    return p;
  }
  if( d< 0) {
    *p++= '-';
    d*= -1;
  }
  if( d < 10) {
    *p++= '0';
    *p++= '.';
    *p++= '0';
    *p++= '0'+ d%10;
    *p++= ',';
    return p;
  }
  char *q= tmpb;
  int dec= 0;
  while( d> 0) {
    *q++= '0'+ d%10;
    if( ++dec== 2) 
      *q++= '.';
    d= d/10;
  }
  for( q--;; q--) {
    *p++= *q;
    if( q== tmpb) break;
  }
  *p++= ',';
  return p;
}

/**
 * Converte um valor unsigned int para string e move
 * o ponteiro de escrita para a proxima posicao 
 * a escrever. Também adiciona ',' ao final
 * 
 * Não é colocado \0 ao final do buffer
 */
char* SaidaRasp::_utochar( char *p, unsigned int d) {
  if( isinf( d) || isnan(d)) {
    *p++= ',';
    return p;
  }
  if( d== 0) {
    *p++= '0';
    *p++= ',';
    return p;
  }
  char *q= tmpb;
  while( d> 0) {
    *q++= '0'+ d%10;
    d= d/10;
  }
  for( q--;; q--) {
    *p++= *q;
    if( q== tmpb) break;
  }
  *p++= ',';
  return p;
}
