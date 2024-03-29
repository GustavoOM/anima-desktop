#include "_global.h"

#define SERVERPORT 8083
#define MAX 80
#define SA struct sockaddr

//#define VERBOSE

#define NUMERO_CAMPOS_VENTILACAO (26)
#define NUMERO_CAMPOS_LIMITES_ALARMES (14)
#define NUMERO_CAMPOS_OPERACAO (7)
#define NUMERO_CAMPOS_AUTOTESTES (5)
#define NUMERO_CAMPOS_SENSORES_PRESSAO (10)
#define NUMERO_CAMPOS_SENSOR_FLUXO (43)
#define NUMERO_CAMPOS_VALVULA_EXALACAO (54)
#define NUMERO_CAMPOS_VALVULAS_AOP (23)
#define NUMERO_CAMPOS_PID (24)
#define NUMERO_CAMPOS_CONTROLE (13)
#define NUMERO_CAMPOS_DEGRAU (8)

#define TAMANHO_BLOCO_LEITOR  (512)


void EntradaRasp::setup() {
  memset(_buffer, 0, sizeof(_buffer));
  memset(_cbuffer, 0, sizeof(_cbuffer));
  memset(_cbuffer_bkp, 0, sizeof(_cbuffer_bkp));
  memset(&_statusDadosIni, 0, sizeof(_statusDadosIni));
  _posVirguChecksum = 0;
  _idMsgAtual = 0;
  _idMsgAnterior = 0;
  _msgAnteriorFoiAceita = false;
  _erroComandoInvalido = false;
  _flagCampoInvalido = false;
  dadosRecebidosLoop.operacao.stop = 7;

  
  
  // socket create and verification
  sockfd_server = socket(AF_INET, SOCK_STREAM, 0);
  //fcntl(sockfd_server, F_SETFL, O_NONBLOCK);
  if (sockfd_server == -1) {
      printf("socket creation failed...\n");
      exit(0);
  }
  else
      printf("Socket successfully created..\n");
  bzero(&servaddr_server, sizeof(servaddr_server));
  
  // assign IP, PORT
  servaddr_server.sin_family = AF_INET;
  servaddr_server.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr_server.sin_port = htons(SERVERPORT);
  
  // Binding newly created socket to given IP and verification
  if ((bind(sockfd_server, (SA*)&servaddr_server, sizeof(servaddr_server))) != 0) {
      printf("socket bind failed...\n");
      exit(0);
  }
  else
      printf("Socket successfully binded..\n");
  
  // Now server is ready to listen and verification
  if ((listen(sockfd_server, 5)) != 0) {
      printf("Listen failed...\n");
      exit(0);
  }
  else
      printf("Server listening in port %d..\n", SERVERPORT);
  len_server = sizeof(cli_server);
  
  // Accept the data packet from client and verification
  connfd_server = accept(sockfd_server, (SA*)&cli_server, (socklen_t *)&len_server);
  if (connfd_server < 0) {
      printf("server accept failed...\n");
      exit(0);
  }
  else
      printf("server accept the client...\n");
   
  // Configura o socket para ser não bloqueante
  int flags = fcntl(connfd_server, F_GETFL, 0);
  fcntl(connfd_server, F_SETFL, flags | O_NONBLOCK);
  
  int k = 0;
    for(int i = 0;i < 40000; i++){
        for(int j = 0;j < 40000; j++){
        k = i + j;
        }
    }
}

bool EntradaRasp::_recebeMensagem() {
  if (_leCaracteres()) {
    return true;
  }
  return false;
}

void EntradaRasp::loop() {
  if (_recebeMensagem()) {
    int tipoMsg = atoi(_buffer[0]+1);
    switch (tipoMsg) {
      case 11:
        _parseiaMensagemVentilacao(); break;
      case 13:
        _parseiaMensagemLimitesAlarmes(); break;
      case 15:
        _parseiaMensagemOperacao(); break;
      case 41:
        _parseiaMensagemPid(); break;
      case 43:
        _parseiaMensagemControle(); break;
      case 45:
        _parseiaMensagemDegrau(); break;
    }
  #if defined(VERBOSE) && !defined(ARDUINO_PLOTTER)
    SerialUSB.print("Mensagem parseada: "); SerialUSB.println(tipoMsg);
  #endif

    long idMsg = atol(_buffer[2]);
    _encaminhaResposta(idMsg);
  }
}

AutotestesEnum EntradaRasp::esperaRecebeComandoAutotestes() {
  AutotestesEnum comando = INDEFINIDO_TESTE;
  
  while (true) {
    if (_recebeMensagem()) {
      
      int tipoMsg = atoi(_buffer[0]+1);
      if (tipoMsg == 17) {
        comando = _parseiaMensagemAutotestes();
      #if defined(VERBOSE) && !defined(ARDUINO_PLOTTER)
        SerialUSB.print("Mensagem parseada: "); SerialUSB.println(tipoMsg);
      #endif

        long idMsg = atol(_buffer[2]);
        _encaminhaResposta(idMsg);
        break;
      } 
    }
  }

  return comando;
}

void EntradaRasp::recebeParametrosInicializacao() {

  while (!(
    _statusDadosIni.paramsSensPressaoRecebidos &&
    _statusDadosIni.paramsSensFluxRecebidos[0] &&
    _statusDadosIni.paramsSensFluxRecebidos[1] &&
    _statusDadosIni.paramsSensFluxRecebidos[2] &&
    _statusDadosIni.paramsValvExaRecebidos &&
    _statusDadosIni.paramsValvsAOPRecebidos
  )) {
    if (_recebeMensagem()) {
      int tipoMsg = atoi(_buffer[0]+1);
      switch (tipoMsg) {
        case 31:
          _parseiaMensagemSensoresPressao(); break;
        case 33:
          _parseiaMensagemSensorFluxo(); break;
        case 35:
          _parseiaMensagemValvulaExalacao(); break;
        case 37:
          _parseiaMensagemValvulasAOP(); break;
      }
    #if defined(VERBOSE) && !defined(ARDUINO_PLOTTER)
      SerialUSB.print("Mensagem parseada: "); SerialUSB.println(tipoMsg);
    #endif

      long idMsg = atol(_buffer[2]);
      _encaminhaResposta(idMsg);
    }
  }

#if defined(VERBOSE) && !defined(ARDUINO_PLOTTER)
  SerialUSB.println("Todos os parâmetros iniciais foram lidos com sucesso");
#endif
}

void EntradaRasp::_encaminhaResposta(unsigned int idMsg) {
  saidaRasp.enviaResposta(idMsg);
#if defined(VERBOSE) && !defined(ARDUINO_PLOTTER)
  SerialUSB.print("Resposta enviada: "); SerialUSB.println(idMsg);
#endif
}

bool EntradaRasp::haErroComandoInvalido() {
  return _erroComandoInvalido;
}

bool EntradaRasp::_campoVazio(char* dadoChar) {
  return dadoChar[0] == '\0';
}

bool EntradaRasp::_campoValidoFloat(char* dadoChar, TipoEntradaEnum tipoEntrada) {
  bool campoValido = false;
  if (!_campoVazio(dadoChar)) {
    float dado = atof(dadoChar);
    switch (tipoEntrada) {
      case ENT_PRESSAO:
        campoValido = 0 <= dado && dado <= 150; break;
      case ENT_FLUXO:
        campoValido = 0 <= dado && dado <= 200; break;
      case ENT_PONTO_AUTOKP:
        campoValido = 0 <= dado && dado <= 100; break;
      case ENT_PARAM_PID:
        campoValido = 0 <= dado && dado <= 200; break;
      case ENT_GANHO_BAIXO:
        campoValido = 0 <= dado && dado <= 3; break;
      case ENT_FIO2:
        campoValido = 0 <= dado && dado <= 1; break;
      case ENT_SENSIBILIDADE:
        campoValido = -30 <= dado && dado <= 30; break;
      case ENT_PWM_FLOAT:
        campoValido = 0 <= dado && dado <= 4095; break;
      case ENT_DELTA_ESTRANG:
        campoValido = 0 <= dado && dado <= 10; break;
      case ENT_PRESSAO_VENT:
        campoValido = 0 <= dado && dado <= 80; break;
    }

    if (!campoValido) {
      _flagCampoInvalido = true;
    #if defined(VERBOSE) && !defined(ARDUINO_PLOTTER)
      SerialUSB.print("Campo "); SerialUSB.print(dadoChar); SerialUSB.print(" do tipo "); SerialUSB.print(tipoEntrada);
      SerialUSB.println(" INVALIDO!");
    #endif
    }
    return campoValido;
  }
  else {
    return false;
  }
}

bool EntradaRasp::_campoValidoLong(char* dadoChar, TipoEntradaEnum tipoEntrada) {
  bool campoValido = false;
  if (!_campoVazio(dadoChar)) {
    long dado = atol(dadoChar);
    switch (tipoEntrada) {
      case ENT_PWM_LONG:
        campoValido = 0 <= dado && dado <= 4095; break;
      case ENT_ADC:
        campoValido = -80000 <= dado && dado <= 80000; break;
      case ENT_TEMPO_1S:
        campoValido = 0 <= dado && dado <= 1000; break;
      case ENT_TEMPO_10S:
        campoValido = 0 <= dado && dado <= 10000; break;
      case ENT_TEMPO_100S:
        campoValido = 0 <= dado && dado <= 100000; break;
      case ENT_BOOL7:
        campoValido = dado == 0 || dado == 7; break;
      case ENT_VOLUME_TIDAL:
        campoValido = 0 <= dado && dado <= 2000; break;
      case ENT_ENUM:
        campoValido = 0 <= dado && dado <= 20; break;
      case ENT_VOLUME_MIN:
        campoValido = 0 <= dado && dado <= 200000; break;
      case ENT_FREQUENCIA:
        campoValido = 0 <= dado && dado <= 200; break;
      case ENT_PID:
        campoValido = 0 <= dado && dado <= 4; break;
      case ENT_SENS_FLUX:
        campoValido = 0 <= dado && dado <= 2; break;
      case ENT_BOOL1:
        campoValido = dado == 0 || dado == 1; break;
    }

    if (!campoValido) {
      _flagCampoInvalido = true;
    #if defined(VERBOSE) && !defined(ARDUINO_PLOTTER)
      SerialUSB.print("Campo "); SerialUSB.print(dadoChar); SerialUSB.print(" do tipo "); SerialUSB.print(tipoEntrada);
      SerialUSB.println(" INVALIDO!");
    #endif
    }
    return campoValido;
  }
  else {
    return false;
  }
}

void EntradaRasp::_parseiaMensagemVentilacao() {
  ParametrosVentilacao dados = dadosRecebidosLoop.ventilacao;
  _flagCampoInvalido = false;
 
  if (_campoValidoLong(_buffer[3], ENT_ENUM)) { dados.modo = (ModosEnum) atol(_buffer[3]); }
  if (_campoValidoLong(_buffer[4], ENT_VOLUME_TIDAL)) { dados.vTidal = atol(_buffer[4]); }
  if (_campoValidoFloat(_buffer[5], ENT_FIO2)) { dados.fio2 = atof(_buffer[5]); }
  if (_campoValidoLong(_buffer[6], ENT_ENUM)) { dados.onda = (OndaFluxoEnum) atol(_buffer[6]); }
  if (_campoValidoLong(_buffer[7], ENT_TEMPO_10S)) { dados.tIns = atol(_buffer[7]); }
  if (_campoValidoLong(_buffer[8], ENT_FREQUENCIA)) { dados.freq = atol(_buffer[8]); }
  if (_campoValidoFloat(_buffer[9], ENT_PRESSAO_VENT)) { dados.peep = atof(_buffer[9]); }
  if (_campoValidoFloat(_buffer[10], ENT_SENSIBILIDADE)) { dados.sensInsP = atof(_buffer[10]); }
  if (_campoValidoFloat(_buffer[11], ENT_SENSIBILIDADE)) { dados.sensInpF = atof(_buffer[11]); }
  if (_campoValidoLong(_buffer[12], ENT_TEMPO_10S)) { dados.tPausaIns = atol(_buffer[12]); }
  if (_campoValidoFloat(_buffer[13], ENT_PRESSAO_VENT)) { dados.pInsLimite = atof(_buffer[13]); }
  if (_campoValidoFloat(_buffer[14], ENT_PRESSAO_VENT)) { dados.pCon = atof(_buffer[14]); }
  if (_campoValidoLong(_buffer[15], ENT_TEMPO_10S)) { dados.rTime = atol(_buffer[15]); }
  if (_campoValidoLong(_buffer[16], ENT_TEMPO_100S)) { dados.tImax = atol(_buffer[16]); }
  if (_campoValidoFloat(_buffer[17], ENT_PRESSAO_VENT)) { dados.pSup = atof(_buffer[17]); }
  if (_campoValidoFloat(_buffer[18], ENT_SENSIBILIDADE)) { dados.sensExpF = atof(_buffer[18]); }
  if (_campoValidoLong(_buffer[19], ENT_ENUM)) { dados.bkModo = (ModosEnum) atol(_buffer[19]); }
  if (_campoValidoLong(_buffer[20], ENT_TEMPO_10S)) { dados.bkTIns = atol(_buffer[20]); }
  if (_campoValidoLong(_buffer[21], ENT_FREQUENCIA)) { dados.bkFreq = atol(_buffer[21]); }
  if (_campoValidoLong(_buffer[22], ENT_VOLUME_TIDAL)) { dados.bkVTidal = atol(_buffer[22]); }
  if (_campoValidoFloat(_buffer[23], ENT_PRESSAO_VENT)) { dados.bkPcon = atof(_buffer[23]); }
  if (_campoValidoLong(_buffer[24], ENT_TEMPO_100S)) { dados.tApneia = atol(_buffer[24]); }
  
  if (_flagCampoInvalido) {
    _erroComandoInvalido = true;
    return;
  }
  dadosRecebidosLoop.ventilacao = dados;
  printf("dados.modo = %d\n", dados.modo);
  
}

void EntradaRasp::_parseiaMensagemLimitesAlarmes() {
  LimitesAlarmes dados;
  _flagCampoInvalido = false;

  if (_campoValidoFloat(_buffer[3], ENT_PRESSAO_VENT)) { dados.pInsPlatMin = atof(_buffer[3]); }
  if (_campoValidoFloat(_buffer[4], ENT_PRESSAO_VENT)) { dados.pInsMax = atof(_buffer[4]); }
  if (_campoValidoLong(_buffer[5], ENT_VOLUME_TIDAL)) { dados.vTidalInsMin = atol(_buffer[5]); }
  if (_campoValidoLong(_buffer[6], ENT_VOLUME_TIDAL)) { dados.vTidalInsMax = atol(_buffer[6]); }
  if (_campoValidoFloat(_buffer[7], ENT_PRESSAO_VENT)) { dados.pPeepMax = atof(_buffer[7]); }
  if (_campoValidoLong(_buffer[8], ENT_VOLUME_MIN)) { dados.vMinuMin = atol(_buffer[8]); }
  if (_campoValidoLong(_buffer[9], ENT_VOLUME_MIN)) { dados.vMinuMax = atol(_buffer[9]); }
  if (_campoValidoFloat(_buffer[10], ENT_FIO2)) { dados.minFio2 = atof(_buffer[10]); }
  if (_campoValidoFloat(_buffer[11], ENT_FIO2)) { dados.maxFio2 = atof(_buffer[11]); }
  if (_campoValidoLong(_buffer[12], ENT_FREQUENCIA)) { dados.freqMax = atol(_buffer[12]); }

  if (_flagCampoInvalido) {
    _erroComandoInvalido = true;
    return;
  }
  dadosRecebidosLoop.limitesAlarmes = dados;
}

void EntradaRasp::_parseiaMensagemOperacao() {
  ComandosOperacao dados = dadosRecebidosLoop.operacao;
  _flagCampoInvalido = false;

  if (_campoValidoLong(_buffer[3], ENT_BOOL7)) { dados.stop = atol(_buffer[3]); }
  if (_campoValidoLong(_buffer[4], ENT_BOOL7)) { dados.desliga = atol(_buffer[4]); }
  if (_campoValidoLong(_buffer[5], ENT_BOOL7)) { dados.calibFluxoEx = atol(_buffer[5]); }

  if (_flagCampoInvalido) {
    _erroComandoInvalido = true;
    return;
  }
  dadosRecebidosLoop.operacao = dados;
}

void EntradaRasp::_parseiaMensagemPid() {
  ParametrosPid dados;
  _flagCampoInvalido = false;

  long id;
  if (_campoValidoLong(_buffer[3], ENT_PID)) { id = atol(_buffer[3]); }
  if (_campoValidoFloat(_buffer[4], ENT_PARAM_PID)) { dados.Kp = atof(_buffer[4]); }
  if (_campoValidoFloat(_buffer[5], ENT_PARAM_PID)) { dados.Ti = atof(_buffer[5]); }
  if (_campoValidoFloat(_buffer[6], ENT_PARAM_PID)) { dados.Td = atof(_buffer[6]); }
  if (_campoValidoFloat(_buffer[7], ENT_PARAM_PID)) { dados.N = atof(_buffer[7]); }
  if (_campoValidoFloat(_buffer[8], ENT_PWM_FLOAT)) { dados.I0 = atof(_buffer[8]); }
  if (_campoValidoFloat(_buffer[9], ENT_PARAM_PID)) { dados.Tt = atof(_buffer[9]); }
  if (_campoValidoFloat(_buffer[10], ENT_PWM_FLOAT)) { dados.minn = atof(_buffer[10]); }
  if (_campoValidoFloat(_buffer[11], ENT_PWM_FLOAT)) { dados.maxx = atof(_buffer[11]); }
  if (_campoValidoFloat(_buffer[12], ENT_PONTO_AUTOKP)) { dados.pontosAutoKp[0] = atof(_buffer[12]); }
  if (_campoValidoFloat(_buffer[13], ENT_PONTO_AUTOKP)) { dados.pontosAutoKp[1] = atof(_buffer[13]); }
  if (_campoValidoFloat(_buffer[14], ENT_PONTO_AUTOKP)) { dados.pontosAutoKp[2] = atof(_buffer[14]); }
  if (_campoValidoFloat(_buffer[15], ENT_PONTO_AUTOKP)) { dados.pontosAutoKp[3] = atof(_buffer[15]); }
  if (_campoValidoFloat(_buffer[16], ENT_PONTO_AUTOKP)) { dados.pontosAutoKp[4] = atof(_buffer[16]); }
  if (_campoValidoFloat(_buffer[17], ENT_GANHO_BAIXO)) { dados.ganhosAutoKp[0] = atof(_buffer[17]); }
  if (_campoValidoFloat(_buffer[18], ENT_GANHO_BAIXO)) { dados.ganhosAutoKp[1] = atof(_buffer[18]); }
  if (_campoValidoFloat(_buffer[19], ENT_GANHO_BAIXO)) { dados.ganhosAutoKp[2] = atof(_buffer[19]); }
  if (_campoValidoFloat(_buffer[20], ENT_GANHO_BAIXO)) { dados.ganhosAutoKp[3] = atof(_buffer[20]); }
  if (_campoValidoFloat(_buffer[21], ENT_GANHO_BAIXO)) { dados.ganhosAutoKp[4] = atof(_buffer[21]); }
  if (_campoValidoLong(_buffer[22], ENT_BOOL1)) { dados.autoKpPorDeltaSet = atol(_buffer[22]); }

  if (_flagCampoInvalido) {
    _erroComandoInvalido = true;
    return;
  }
  dadosRecebidosLoop.pid[id] = dados;
  controle.notificaNovosParametrosControle();
}

void EntradaRasp::_parseiaMensagemControle() {
  ParametrosAjusteControle dados;
  _flagCampoInvalido = false;

  if (_campoValidoFloat(_buffer[3], ENT_GANHO_BAIXO)) { dados.Kr = atof(_buffer[3]); }
  if (_campoValidoFloat(_buffer[4], ENT_DELTA_ESTRANG)) { dados.deltaEstrangMin = atof(_buffer[4]); }
  if (_campoValidoFloat(_buffer[5], ENT_DELTA_ESTRANG)) { dados.deltaEstrangMax = atof(_buffer[5]); }
  if (_campoValidoFloat(_buffer[6], ENT_GANHO_BAIXO)) { dados.minCCVolume = atof(_buffer[6]); }
  if (_campoValidoFloat(_buffer[7], ENT_GANHO_BAIXO)) { dados.maxCCVolume = atof(_buffer[7]); }
  if (_campoValidoFloat(_buffer[8], ENT_GANHO_BAIXO)) { dados.fatorCCVolume = atof(_buffer[8]); }
  if (_campoValidoFloat(_buffer[9], ENT_GANHO_BAIXO)) { dados.minCCPressao = atof(_buffer[9]); }
  if (_campoValidoFloat(_buffer[10], ENT_GANHO_BAIXO)) { dados.maxCCPressao = atof(_buffer[10]); }
  if (_campoValidoFloat(_buffer[11], ENT_GANHO_BAIXO)) { dados.fatorCCPressao = atof(_buffer[11]); }

  if (_flagCampoInvalido) {
    _erroComandoInvalido = true;
    return;
  }
  dadosRecebidosLoop.controle = dados;
  controle.notificaNovosParametrosControle();
}

void EntradaRasp::_parseiaMensagemDegrau() {
  ComandosDegrauCalibracao dados;
  _flagCampoInvalido = false;

  if (_campoValidoLong(_buffer[3], ENT_ENUM)) { dados.valvula = atol(_buffer[3]); }
  if (_campoValidoLong(_buffer[4], ENT_TEMPO_10S)) { dados.periodoDeg = atol(_buffer[4]); }
  if (_campoValidoFloat(_buffer[5], ENT_PONTO_AUTOKP)) { dados.valor1Deg = atof(_buffer[5]); }
  if (_campoValidoFloat(_buffer[6], ENT_PONTO_AUTOKP)) { dados.valor2Deg = atof(_buffer[6]); }

  if (_flagCampoInvalido) {
    _erroComandoInvalido = true;
    return;
  }
  dadosRecebidosLoop.degrau = dados;
}

AutotestesEnum EntradaRasp::_parseiaMensagemAutotestes() {
  AutotestesEnum comando = INDEFINIDO_TESTE;
  if (_campoValidoLong(_buffer[3], ENT_ENUM)) { comando = (AutotestesEnum) atol(_buffer[3]); }
  return comando;
}

void EntradaRasp::_parseiaMensagemSensoresPressao() {
  ParametrosSensoresPressao dados; 
  _flagCampoInvalido = false;
  
  for (int i=3; i<6; i++) {
    if (_campoValidoLong(_buffer[i], ENT_ADC)) { dados.minAdcNoZero[i-3] = atol(_buffer[i]); }
  }
  for (int i=6; i<9; i++) {
    if (_campoValidoLong(_buffer[i], ENT_ADC)) { dados.maxAdcNoZero[i-6] = atol(_buffer[i]); }
  }

  if (_flagCampoInvalido) {
    _erroComandoInvalido = true;
    return;
  }
  paramSensoresPressao = dados;
  _statusDadosIni.paramsSensPressaoRecebidos = true;
}

void EntradaRasp::_parseiaMensagemSensorFluxo() {
  ParametrosSensorFluxo dados;
  _flagCampoInvalido = false;

  long id;
  if (_campoValidoLong(_buffer[3], ENT_SENS_FLUX)) { id = atol(_buffer[3]); }
  for (int i=4; i<22; i++) {
    if (_campoValidoLong(_buffer[i], ENT_ADC)) { dados.curvaAdc[i-4] = atol(_buffer[i]); }
  }
  for (int i=22; i<40; i++) {
    if (_campoValidoFloat(_buffer[i], ENT_FLUXO)) { dados.curvaFluxo[i-22] = atof(_buffer[i]); }
  }
  if (_campoValidoLong(_buffer[40], ENT_ADC)) { dados.minAdcNoZero = atol(_buffer[40]); }
  if (_campoValidoLong(_buffer[41], ENT_ADC)) { dados.maxAdcNoZero = atol(_buffer[41]); }

  if (_flagCampoInvalido) {
    _erroComandoInvalido = true;
    return;
  }
  switch (id) {
    case 0:
      paramSensorFluxoAr = dados; break;
    case 1:
      paramSensorFluxoO2 = dados; break;
    case 2:
      paramSensorFluxoEx = dados; break;
  }
  _statusDadosIni.paramsSensFluxRecebidos[id] = true;
}

void EntradaRasp::_parseiaMensagemValvulaExalacao() {
  ParametrosValvulaExalacao dados; 
  _flagCampoInvalido = false;
  
  for (int i=3; i<14; i++) {
    if (_campoValidoFloat(_buffer[i], ENT_PRESSAO)) { dados.curvaSistema[i-3] = atof(_buffer[i]); }
  }
  for (int i=14; i<25; i++) {
    if (_campoValidoFloat(_buffer[i], ENT_PRESSAO)) { dados.curvaPiloto[i-14] = atof(_buffer[i]); }
  }
  for (int i=25; i<34; i++) {
    if (_campoValidoFloat(_buffer[i], ENT_PRESSAO)) { dados.limiaresSistema[i-25] = atof(_buffer[i]); }
  }
  for (int i=34; i<43; i++) {
    if (_campoValidoFloat(_buffer[i], ENT_PRESSAO)) { dados.minPilotoNoLimiarSistema[i-34] = atof(_buffer[i]); }
  }
  for (int i=43; i<52; i++) {
    if (_campoValidoFloat(_buffer[i], ENT_PRESSAO)) { dados.maxPilotoNoLimiarSistema[i-43] = atof(_buffer[i]); }
  }
  if (_campoValidoFloat(_buffer[52], ENT_PRESSAO)) { dados.minPressaoPicoSistema = atof(_buffer[52]); }

  if (_flagCampoInvalido) {
    _erroComandoInvalido = true;
    return;
  }
  paramValvulaExalacao = dados;
  _statusDadosIni.paramsValvExaRecebidos = true;
}

void EntradaRasp::_parseiaMensagemValvulasAOP() {
  ParametrosValvulasAOP dados; 
  _flagCampoInvalido = false;

  for (int i=3; i<6; i++) {
    if (_campoValidoLong(_buffer[i], ENT_PWM_LONG)) { dados.pwmAberturaMin[i-3] = atol(_buffer[i]); }
  }
  for (int i=6; i<9; i++) {
    if (_campoValidoLong(_buffer[i], ENT_PWM_LONG)) { dados.pwmAberturaMax[i-6] = atol(_buffer[i]); }
  }
  for (int i=9; i<12; i++) {
    if (_campoValidoLong(_buffer[i], ENT_PWM_LONG)) { dados.pwmFechamentoMin[i-9] = atol(_buffer[i]); }
  }
  for (int i=12; i<15; i++) {
    if (_campoValidoLong(_buffer[i], ENT_PWM_LONG)) { dados.pwmFechamentoMax[i-12] = atol(_buffer[i]); }
  }
  for (int i=15; i<18; i++) {
    if (_campoValidoFloat(_buffer[i], ENT_FLUXO)) { dados.saidaPicoMin[i-15] = atof(_buffer[i]); }
  }
  for (int i=18; i<21; i++) {
    if (_campoValidoFloat(_buffer[i], ENT_FLUXO)) { dados.saidaPicoMax[i-18] = atof(_buffer[i]); }
  }
  if (_campoValidoLong(_buffer[21], ENT_TEMPO_1S)) { dados.tempoMaxValvSeguranca = atol(_buffer[21]); }
  
  if (_flagCampoInvalido) {
    _erroComandoInvalido = true;
    return;
  }
  paramValvulasAOP = dados;
  _statusDadosIni.paramsValvsAOPRecebidos = true;
}


/**
 * Le bloco de mensagem, até encontrar final de mensagem (;)
 * ou completar tamanho maximo de bloco
 */
bool EntradaRasp::_leCaracteres()
{
  //Receber do socket
  bzero(_cbuffer, MAX);
  //read não bloqueante
  read(connfd_server, _cbuffer, sizeof(_cbuffer));
  //strcpy(_cbuffer,strtok(_cbuffer, ";"));
  if(!strcmp(_cbuffer,"") == 0){
    printf("[WRITER]: %s\n", _cbuffer);
  }
  memset(_buffer, 0, sizeof(_buffer));
  // seta ponteiros
  int row = 0, col = 0;
  char c;
  // le ate encontrar ';' ou atingir limite
  int j = 0;
  for (bool done = false; !done;)
  {
    c = _cbuffer[j++];
    if (c == 0)
      break;
    if (c == ',')
    {
      // proximo token
      row++;
      col = 0;

      // checa para nao estourar _buffer
      if (row >= NUMERO_MAXIMO_CAMPOS)
        done = true;
    }
    else if (c == ';')
    {
      // fim da mensagem
      done = true;
    }
    else
    {
      // caractere normal
      // dentro do limite do token?
      if (col < TAMANHO_MAXIMO_CAMPO)
      {
        _buffer[row][col++] = c;
      }
    }
  }
  memset(_cbuffer, 0, sizeof(_cbuffer));
  return true;
} 