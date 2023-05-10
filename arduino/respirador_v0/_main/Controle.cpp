#include "_global.h"

/*
A camada de Controle possui o módulo Controle como módulo-pai, o qual chama os módulos
ModoVCV ou ModoPCV para realizar a operação definida na tela. Estes, por sua vez,
coordenam 3 subsistemas de controle que contém 5 PIDs para atuação nas válvulas; a
associação desses elementos é a seguinte, sendo que os dois primeiros são usados em ambos
os modos e o último apenas no PCV:
- SCFluxo: pidFluxoAr e pidFluxoO2;
- SCPressaoExp: pidPressaoExp e pidPressaoPil;
- SCPressaoIns: pidPressaoIns.
*/

Controle::Controle() { }

/*
Este é o módulo-pai da camada de Controle, fazendo a interface com a camada da Estratégia.
Assim, possui um método loop() para ser chamado a cada iteração de atuação nas válvulas e
os seguintes métodos para sinalizar o chaveamento de fases realizado pela Estratégia:
iniciaInspiracao(), iniciaPausaIns() e iniciaExpiracao(). Portanto, quem controla o tempo
durante uma respiração é a Estratégia.

Está associado à seguintes structs globais:
- ParametrosCicloControle, que reflete os parâmetros setados pelo usuário na tela durante
a operação do equipamento;
- ParametrosAjusteControle, que possui parâmetros adicionais definidos por calibração ou
experimentação;
- EstadoControle, que armazena variáveis de estado.
*/

void Controle::setup() {
  memset(&paramCicloControle, 0, sizeof(ParametrosCicloControle));
  memset(&paramAjusteControle, 0, sizeof(ParametrosAjusteControle));
  memset(&estadoControle, 0, sizeof(EstadoControle));

  _setDefaultParametrosAjusteControle(); // sobrescritos no caso de comunicação com a Rasp
  _resetEstadoControle();
  _deveRecarregarParametrosControle = false;

  // ModoVCV e ModoPCV não possuem processos de inicialização
  scFluxo.setup();
  scPressaoExp.setup();
  scPressaoIns.setup();
}

void Controle::loop() {
  printf("estadoControle.modoAtual = %d\n", estadoControle.modoAtual);
  switch (estadoControle.modoAtual) {
    case MODO_VCV:
      modoVCV.loop(); break;
    case MODO_PCV:
      modoPCV.loop(); break;
    case MODO_PSV:
      modoPCV.loop(); break;
  }
  estadoControle.faseAnterior = estadoControle.faseAtual;
}

void Controle::iniciaInspiracao(ParametrosVentilacao* param) {
  estadoControle.faseAtual = EM_INSPIRACAO;
  printf("Controle::iniciaInspiracao -> param.modo = %d\n", param->modo);
  _reiniciaEstado(param);
  _recarregaParametrosVentilacao(param);

#ifndef SIMULADOR
  if (_deveRecarregarParametrosControle) {
    recarregaParametrosControle();
    _deveRecarregarParametrosControle = false;
  }
#endif
}

void Controle::iniciaPausaIns() {
  estadoControle.faseAtual = EM_PAUSA_INSPIRACAO;
}

void Controle::iniciaExpiracao() {
  estadoControle.faseAtual = EM_EXPIRACAO;
}

// usado para fins de segurança
void Controle::paraRespiracao() {
  estadoControle.faseAtual = INICIAL;
  estadoControle.modoAnterior = estadoControle.modoAtual;
  estadoControle.modoAtual = MODO_UNDEFINED;

  valvulaFluxoArVP1(0);
  valvulaFluxoOxVP2(0);
  valvulaOxExalacaoVP3(0);

  _resetEstadoControle();
  scFluxo.resetPid();
  scPressaoExp.resetPid();
  scPressaoIns.resetPid();
}

// usado no alarme de pressão limite do VCV
void Controle::cortaFluxoIns() {
  paramCicloControle.volume = 0.0;
}

void Controle::setFiO2Fixo(FiO2FixoEnum fio2Fixo, bool estado) {
  if (fio2Fixo == FIO2_FIXO_21) {
    estadoControle.isFiO2Fixo21 = estado;
  }
  if (fio2Fixo == FIO2_FIXO_100) {
    estadoControle.isFiO2Fixo100 = estado;
  }
}

void Controle::setFalhaSensorPiloto(bool estado) {
  estadoControle.isSensorPilotoFalho = estado;
}

void Controle::notificaNovosParametrosControle() {
  _deveRecarregarParametrosControle = true;
}

void Controle::_reiniciaEstado(ParametrosVentilacao* param) {
  estadoControle.modoAnterior = estadoControle.modoAtual;
  estadoControle.modoAtual = param->modo;

  // contadores são utilizados para geração de setpoints que dependem do tempo, o qual
  // não é controlado pela camada de Controle
  estadoControle.contIterSetFluxo = 0;
  estadoControle.contIterSetPressaoIns = 0;
  if (estadoControle.modoAtual != estadoControle.modoAnterior) {
    scFluxo.resetPid();
    scPressaoExp.resetPid();
    scPressaoIns.resetPid();
    estadoControle.corrCCVolume = 1.0;
    estadoControle.corrCCPressao = 1.0;
  }
}

void Controle::_recarregaParametrosVentilacao(ParametrosVentilacao* param) {
  // parâmetros de ciclo chegam ao Controle por meio da struct ParametrosVentilacao
  paramCicloControle.tipoCurva = param->onda;
  paramCicloControle.volume = param->vTidal / 1000.0; // conversão para L
  paramCicloControle.fio2 = param->fio2;
  if (estadoControle.isFiO2Fixo21) {
    paramCicloControle.fio2 = 0.209;
  }
  if (estadoControle.isFiO2Fixo100) {
    paramCicloControle.fio2 = 1.00;
  }
  paramCicloControle.fluxoBase = FLUXO_BASE;
  paramCicloControle.pressaoCon = param->pCon;
  if (estadoControle.modoAtual == MODO_PSV) {
    paramCicloControle.pressaoCon = param->pSup;
  }
  paramCicloControle.peep = param->peep;
  paramCicloControle.pressaoMax = param->pInsLimite;
  paramCicloControle.tIns = param->tIns / 1000.0; // conversão para s
  paramCicloControle.tSubida = param->rTime / 1000.0; // conversão para s
}

void Controle::recarregaParametrosControle() {
  paramPidFluxoAr = entradaRasp.dadosRecebidosLoop.pid[0];
  paramPidFluxoO2 = entradaRasp.dadosRecebidosLoop.pid[1];
  paramPidPressaoIns = entradaRasp.dadosRecebidosLoop.pid[2];
  paramPidPressaoPil = entradaRasp.dadosRecebidosLoop.pid[3];
  paramPidPressaoExp = entradaRasp.dadosRecebidosLoop.pid[4];
  paramAjusteControle = entradaRasp.dadosRecebidosLoop.controle;
}

void Controle::_setDefaultParametrosAjusteControle() {
  paramAjusteControle.Kr = 0.02;
  paramAjusteControle.deltaEstrangMin = 4.0;
  paramAjusteControle.deltaEstrangMax = 4.0;
  paramAjusteControle.minCCVolume = 0.95;
  paramAjusteControle.maxCCVolume = 1.05;
  paramAjusteControle.fatorCCVolume = 1.0;
  paramAjusteControle.minCCPressao = 0.95;
  paramAjusteControle.maxCCPressao = 1.05;
  paramAjusteControle.fatorCCPressao = 1.0;
}

void Controle::_resetEstadoControle() {
  estadoControle.setFluxo = 0.0;
  estadoControle.setPressaoIns = 0.0;
  estadoControle.setPressaoExp = 0.0;
  estadoControle.corrCCVolume = 1.0;
  estadoControle.corrCCPressao = 1.0;
  estadoControle.contIterSetFluxo = 0;
  estadoControle.contIterSetPressaoIns = 0;
}
