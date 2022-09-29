#ifndef RESPIRADOR_H
#define RESPIRADOR_H

#include "_global.h"

class Respirador {
  public:
    Respirador();
    void setup();
    void loop();
    void setLoop(bool deveLoopar);

    DadosTemporizacao dadosTempo;
    long contIter;

  private:
    void _verificaOperacao();
    
    unsigned long _tInicioIteracao;
    unsigned long _tInicioCiclo;
    bool _deveLoopar;
    bool _deveCalibrarFluxoEx;
};

#endif
