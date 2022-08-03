#include "_global.h"

//#define VERBOSE

Sensores::Sensores () {
  memset(&statusSensores, 0, sizeof(StatusSensores));
  memset(&medidas, 0, sizeof(Medidas));
  memset(&medidasRasp, 0, sizeof(Medidas));
  
  _sensoresZerados = false;

  _filtroP3 = new Filtro(FILTRO_ATMEL);
  _filtroP4 = new Filtro(FILTRO_ATMEL);
  _filtroP5 = new Filtro(FILTRO_ATMEL);
  _filtroF1 = new Filtro(FILTRO_ATMEL);
  _filtroF2 = new Filtro(FILTRO_ATMEL);
  _filtroF4 = new Filtro(FILTRO_ATMEL);
  _filtroFCal = new Filtro(FILTRO_ATMEL);
  _filtroAdcF1 = new Filtro(FILTRO_ATMEL);
  _filtroAdcF2 = new Filtro(FILTRO_ATMEL);
  _filtroAdcF4 = new Filtro(FILTRO_ATMEL);

  _filtroFluxoArRasp = new Filtro(FILTRO_RASP);
  _filtroFluxoO2Rasp = new Filtro(FILTRO_RASP);
  _filtroFluxoInsRasp = new Filtro(FILTRO_RASP);
  _filtroFluxoExpRasp = new Filtro(FILTRO_RASP);
  _filtroFluxoPacRasp = new Filtro(FILTRO_RASP);
  _filtroPressaoSisRasp = new Filtro(FILTRO_RASP);
  _filtroPressaoPilRasp = new Filtro(FILTRO_RASP);
}

void Sensores::leSensores() {
  leSensoresAlimentacao();
  leSensoresPressao();
  leSensoresFluxo();
  
  _defineMedidas();
  _defineMedidasRasp();
}

void Sensores::leSensoresAlimentacao() {
  statusSensores.tensaoBateria = _leSensorAlimentacaoEletrica(A2);
  statusSensores.tensaoFonte = _leSensorAlimentacaoEletrica(A3);
  statusSensores.valorPressao[1] = _leSensorAlimentacaoGas(A9);
  statusSensores.valorPressao[2] = _leSensorAlimentacaoGas(A10);
}

void Sensores::leSensoresPressao() {
  _leSensorPressao(3, &sensorP3, _filtroP3);
  _leSensorPressao(4, &sensorP4, _filtroP4);
  _leSensorPressao(5, &sensorP5, _filtroP5);
}

void Sensores::leSensoresFluxo() {
  _leSensorFluxo(1, &sensorF1, _filtroF1, _filtroAdcF1);
  _leSensorFluxo(2, &sensorF2, _filtroF2, _filtroAdcF2);
  statusSensores.valorFluxo[3] = statusSensores.valorFluxo[1] + statusSensores.valorFluxo[2];
  _leSensorFluxo(4, &sensorF4, _filtroF4, _filtroAdcF4);
  _leSensorFluxoCalibracao();
}

float Sensores::_leSensorAlimentacaoEletrica(int pino) {
  int valorAdc = analogRead(pino);
  if (valorAdc < 100) {
    return 0;  
  } else {
    return mapFloat(valorAdc, 0, 4095, 0, 37.1);
  }
}

float Sensores::_leSensorAlimentacaoGas(int pino) {
  int valorAdc = analogRead(pino);
  float tensao = mapFloat(valorAdc, 0, 4095, 0, 3.3);
  if (tensao < 0.2) {
    return 0;
  } else {
    return mapFloat(tensao, 0.2, 2.7, 0, 72);
  }
}

void Sensores::_leSensorPressao(int idSensor, ADS1220 *conversor, Filtro *filtro) {
  if (!statusSensores.falhaConversorPressao[idSensor]) {
    long valorAdc = conversor->Read_WaitForData();
    statusSensores.adcPressao[idSensor] = valorAdc / 100;
    statusSensores.erroTimeoutPressao[idSensor] = conversor->getTimeOutFlag();

    float tensao = (float) ((valorAdc * VFSR * 1000) / FSR);
    float pressao = (tensao - statusSensores.offsetPressao[idSensor]) * 0.1052988;
    statusSensores.valorPressao[idSensor] = filtro->filtrar(pressao);
  }
  else {
    statusSensores.adcPressao[idSensor] = 0;
    statusSensores.erroTimeoutPressao[idSensor] = true;
    statusSensores.valorPressao[idSensor] = 0;
  }
}

void Sensores::_leSensorFluxo(int idSensor, ADS1220 *conversor, Filtro *filtroFluxo, Filtro* filtroAdc) {
  if (!statusSensores.falhaConversorFluxo[idSensor]) {
    long valorAdc = conversor->Read_WaitForData();
    statusSensores.adcFluxo[idSensor] = filtroAdc->filtrar(valorAdc / 100);
    statusSensores.erroTimeoutFluxo[idSensor] = conversor->getTimeOutFlag();

    float fluxo = _calculaFluxo(idSensor, valorAdc / 100);
    statusSensores.valorFluxo[idSensor] = filtroFluxo->filtrar(fluxo);
  }
  else {
    statusSensores.adcFluxo[idSensor] = 0;
    statusSensores.erroTimeoutFluxo[idSensor] = true;
    statusSensores.valorFluxo[idSensor] = 0;
  }
}

void Sensores::_leSensorFluxoCalibracao() {
  float somaTensaoMili = 0;
  int nLeituras = 20;
  for (int i=0; i<nLeituras; i++) {
    long valorAdc = analogRead(A8);
    float tensaoMili = mapFloat(valorAdc, 0, 4095, 0, 3310);
    somaTensaoMili += tensaoMili;
    delayMicroseconds(5);
  }
  float mediaTensaoMili = somaTensaoMili / nLeituras;

  float fluxo;
  if (mediaTensaoMili < 533) { // fluxo zero
    mediaTensaoMili = 533;
  } else if (mediaTensaoMili <= 610) { // fluxo ate 5.5 L/min
    fluxo = mediaTensaoMili * 0.0714 - 38.071;
  } else if (mediaTensaoMili > 610   && mediaTensaoMili <=  711) { // fluxo ate 10.3 L/min
    fluxo = mediaTensaoMili * 0.0475 - 23.49;
  } else if (mediaTensaoMili > 711   && mediaTensaoMili <=  866) { // fluxo ate 20 L/min
    fluxo = mediaTensaoMili * 0.0626 - 34.195;
  } else if (mediaTensaoMili > 866   && mediaTensaoMili <= 1100) { // fluxo ate 35.8 L/min
    fluxo = mediaTensaoMili * 0.0675 - 38.474;
  } else if (mediaTensaoMili > 1100  && mediaTensaoMili <= 1505) { // fluxo ate 62.5 L/min
    fluxo = mediaTensaoMili * 0.0659 - 36.719;
  } else { // fluxo acima de 62.5 L/min
    fluxo = mediaTensaoMili * 0.0603 - 28.254;
  }
    
  statusSensores.fluxoCalibracao = _filtroFCal->filtrar(fluxo);
}

float Sensores::_calculaFluxo(int idSens, float adc) {
  ParametrosSensorFluxo *params;
  switch (idSens) {
    case 1:
      params = &paramSensorFluxoAr; break;
    case 2:
      params = &paramSensorFluxoO2; break;
    case 4:
      params = &paramSensorFluxoEx; break;
  }
  long *curvaAdc = params->curvaAdc;
  float *curvaFluxo = params->curvaFluxo;

  if (statusSensores.offsetFluxo[idSens] >= curvaAdc[0]) {
    adc = adc - (statusSensores.offsetFluxo[idSens] - curvaAdc[0]);
  } else {
    adc = adc + (curvaAdc[0] - statusSensores.offsetFluxo[idSens]);
  }
  
  if (adc < curvaAdc[0]) {
    return curvaFluxo[0];
  }

  for (int i=0; i<17; i++) {
    if (adc >= curvaAdc[i] && adc < curvaAdc[i+1]) {
      return mapFloat(adc, curvaAdc[i], curvaAdc[i+1], curvaFluxo[i], curvaFluxo[i+1]);
    }
  }

  if (adc >= curvaAdc[17]) {
    float coefAngularReta = (curvaFluxo[17] - curvaFluxo[16]) / (curvaAdc[17] - curvaAdc[16]);
    float coefLinearReta = curvaFluxo[17] - coefAngularReta * curvaAdc[17]; 
    return coefAngularReta * adc + coefLinearReta;
  }
  
  return 0.0;
}

void Sensores::_defineMedidas() {
  medidas.fluxoAr = statusSensores.valorFluxo[1];
  medidas.fluxoO2 = statusSensores.valorFluxo[2];
  medidas.fluxoIns = statusSensores.valorFluxo[3];
  medidas.fluxoExp = statusSensores.valorFluxo[4];
  medidas.fluxoPac = medidas.fluxoIns - medidas.fluxoExp;
  if (!statusSensores.falhaConversorPressao[4]) {
    medidas.pressaoSis = (statusSensores.valorPressao[3] + statusSensores.valorPressao[4]) / 2.0;
  } else {
    medidas.pressaoSis = statusSensores.valorPressao[3];
  }
  medidas.pressaoIns = statusSensores.valorPressao[3];
  medidas.pressaoPil = statusSensores.valorPressao[5];
}

void Sensores::_defineMedidasRasp() {
  medidasRasp.fluxoAr = _filtroFluxoArRasp->filtrar(medidas.fluxoAr);
  medidasRasp.fluxoO2 = _filtroFluxoO2Rasp->filtrar(medidas.fluxoO2);
  medidasRasp.fluxoIns = _filtroFluxoInsRasp->filtrar(medidas.fluxoIns);
  medidasRasp.fluxoExp = _filtroFluxoExpRasp->filtrar(medidas.fluxoExp);
  medidasRasp.fluxoPac = _filtroFluxoPacRasp->filtrar(medidas.fluxoPac);
  medidasRasp.pressaoSis = _filtroPressaoSisRasp->filtrar(medidas.pressaoSis);
  medidasRasp.pressaoPil = _filtroPressaoPilRasp->filtrar(medidas.pressaoPil);
}

void Sensores::zeraSensores() {
  if (!_sensoresZerados) {
    zeraSensoresPressao();
    zeraSensoresFluxo();
    _sensoresZerados = true;
  }
}

void Sensores::zeraSensoresPressao() {
#ifdef SIMULADOR
   statusSensores.offsetPressao[3] = 236.1;
   statusSensores.offsetPressao[4] = 142.5;
   statusSensores.offsetPressao[5] = 936.9;
   delay(500);
#else

  _zeraSensorPressao(3, &sensorP3);
  _zeraSensorPressao(4, &sensorP4);
  _zeraSensorPressao(5, &sensorP5);

#endif
}

void Sensores::zeraSensoresFluxo() {
  _zeraSensorFluxo(1, &sensorF1);
  _zeraSensorFluxo(2, &sensorF2);
  _zeraSensorFluxo(4, &sensorF4);
}

void Sensores::_zeraSensorPressao(int idSensor, ADS1220 *conversor) {
  long valorAdc = conversor->Read_WaitForData();
  statusSensores.erroTimeoutFluxo[idSensor] |= conversor->getTimeOutFlag();
  float offset = (float) ((valorAdc * VFSR * 1000) / FSR);
  statusSensores.offsetPressao[idSensor] = offset;

#if defined(VERBOSE) && !defined(ARDUINO_PLOTTER)
  SerialUSB.print("  adcNoZeroMin  "); SerialUSB.println(paramSensoresPressao.minAdcNoZero[idSensor-3]);
  SerialUSB.print("  adcNoZero  "); SerialUSB.println(offset);
  SerialUSB.print("  adcNoZeroMax  "); SerialUSB.println(paramSensoresPressao.maxAdcNoZero[idSensor-3]);
  SerialUSB.println();
#endif

  bool cond1 = offset < paramSensoresPressao.minAdcNoZero[idSensor-3];
  bool cond2 = paramSensoresPressao.maxAdcNoZero[idSensor-3] < offset;
  statusSensores.erroOffsetPressao[idSensor] |= cond1 || cond2;
}

void Sensores::_zeraSensorFluxo(int idSensor, ADS1220 *conversor) {
  long somaValorAdc = 0;
  int nLeituras = 10;
  for (int i=0; i<nLeituras; i++) {
    somaValorAdc += conversor->Read_WaitForData() / 100;
    statusSensores.erroTimeoutFluxo[idSensor] |= conversor->getTimeOutFlag();
  }
  float offset = somaValorAdc / nLeituras;
  statusSensores.offsetFluxo[idSensor] = offset;

  ParametrosSensorFluxo *param;
  switch (idSensor) {
    case 1:
      param = &paramSensorFluxoAr; break;

    case 2:
      param = &paramSensorFluxoO2; break;
      break;
    
    case 4:
      param = &paramSensorFluxoEx; break;
      break;
  }

#if defined(VERBOSE) && !defined(ARDUINO_PLOTTER)
  SerialUSB.print("  adcNoZeroMin  "); SerialUSB.println(param->minAdcNoZero);
  SerialUSB.print("  adcNoZero  "); SerialUSB.println(offset);
  SerialUSB.print("  adcNoZeroMax  "); SerialUSB.println(param->maxAdcNoZero);
  SerialUSB.println();
#endif

  bool cond1 = offset < param->minAdcNoZero;
  bool cond2 = param->maxAdcNoZero < offset;
  statusSensores.erroOffsetFluxo[idSensor] |= cond1 || cond2;
}

void Sensores::calibraFluxoEx() {
  ParametrosSensorFluxo parametros;
  memset(&parametros, 0, sizeof(ParametrosSensorFluxo));
  
  if (ensaio.ensaiaSensorFluxo(SENS_FLUX_EX, &parametros)) {
    paramSensorFluxoEx = parametros;
  } else {
    saidaRasp.enviaResultadoCalibracaoFluxoEx(-1);
  }
}
