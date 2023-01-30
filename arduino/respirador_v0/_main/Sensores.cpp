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
  _defineMedidas();
  _defineMedidasRasp();
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
