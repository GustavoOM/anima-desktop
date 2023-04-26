
#ifndef ENTRADA_RASP_H
#define ENTRADA_RASP_H

#define NUMERO_MAXIMO_CAMPOS (65)
#define TAMANHO_MAXIMO_CAMPO (10)
#define TAMANHO_MAXIMO_BUFFER_CHAR (NUMERO_MAXIMO_CAMPOS * TAMANHO_MAXIMO_CAMPO + 1)

#include <string>

class EntradaRasp {

  public:
    void setup();
    void loop();
    void recebeParametrosInicializacao();
    AutotestesEnum esperaRecebeComandoAutotestes();
    std::string esperaRecebeTexto();
    bool haErroComandoInvalido();
    DadosRecebidosLoop dadosRecebidosLoop;

  private:
    void _parseiaMensagemVentilacao();
    void _parseiaMensagemLimitesAlarmes();
    void _parseiaMensagemOperacao();
    void _parseiaMensagemPid();
    void _parseiaMensagemControle();
    void _parseiaMensagemDegrau();

    AutotestesEnum _parseiaMensagemAutotestes();
    void _parseiaMensagemSensoresPressao();
    void _parseiaMensagemSensorFluxo();
    void _parseiaMensagemValvulaExalacao();
    void _parseiaMensagemValvulasAOP();

    bool _campoVazio(char* dadoChar);
    bool _campoValidoFloat(char* dadoChar, TipoEntradaEnum tipoEntrada);
    bool _campoValidoLong(char* dadoChar, TipoEntradaEnum tipoEntrada);
    bool _recebeMensagem();
    bool _leCaracteres();
    void _encaminhaResposta(unsigned int idMsg);

    char _buffer[NUMERO_MAXIMO_CAMPOS][TAMANHO_MAXIMO_CAMPO + 1];
    char _cbuffer[TAMANHO_MAXIMO_BUFFER_CHAR];
    char _cbuffer_bkp[TAMANHO_MAXIMO_BUFFER_CHAR];
    StatusDadosInicializacao _statusDadosIni;
    int _posVirguChecksum;
    bool _erroComandoInvalido;
    bool _flagCampoInvalido;

    unsigned int _idMsgAtual;
    unsigned int _idMsgAnterior;
    bool _msgAnteriorFoiAceita;

    //Socket
    int sockfd_server, connfd_server, len_server;
    struct sockaddr_in servaddr_server, cli_server;


};

#endif
