#ifndef SAIDA_RASP_H
#define SAIDA_RASP_H

#include "_global.h"

#define TAMANHO_MAXIMO_POR_CAMPO (8)
#define NUMERO_MAXIMO_DE_CAMPOS (60)
#define BUFFER_SIZE (NUMERO_MAXIMO_DE_CAMPOS * TAMANHO_MAXIMO_POR_CAMPO)

class SaidaRasp {
  public:
    void setup();
    void loop();
    
    void enviaResposta(unsigned int idMsg);
    void enviaResultadoAutotestes(AutotestesEnum comando, int falha);
    void enviaResultadoCalibracaoFluxoEx(int progresso);
    void enviaParametrosSensoresPressao(ParametrosSensoresPressao dadosCalib);
    void enviaParametrosSensorFluxo(SensFluxEnum sensFlux, ParametrosSensorFluxo dadosCalib);
    void enviaParametrosValvulaExalacao(ParametrosValvulaExalacao dadosCalib);
    void enviaParametrosValvulasAOP(ParametrosValvulasAOP dadosCalib);
    void enviaTexto(char *texto);
  
  private:
    void _enviaDadosGraficosAlarmes();
    void _enviaDadosIndicadores();
    
    char* _adiLongNoBuffer(char* ponteiroBuf, long valor);
    char* _adiUintNoBuffer(char* ponteiroBuf, unsigned int valor);
    char* _adiFloatNoBuffer(char* ponteiroBuf, float valor);
    char* _iniCabecalhoNoBuffer(MsgSaidaEnum tipoMsg);
    void _finRodapeNoBuffer(char* p);
    void _enviaMensagem();
    char* _itochar(char *p, long d);
    char* _ftochar(char *p, float f);
    char* _utochar(char *p, unsigned int d);

    char _buffer[BUFFER_SIZE];
    char tmpb[64];
    unsigned long _tUltimoGraficosAlarmes;
    unsigned long _tUltimoIndicadores; 

    //Socket
    int sockfd_client, connfd_client;
    struct sockaddr_in servaddr_client, cli_client;
 
};

#endif
