#ifndef ALARMES_H
#define ALARMES_H

#include "_global.h"

class Alarmes {
  
  public:
    void setup();
    void loop();
    unsigned int geraNumeroEnvioRasp();

  private:
    StatusAlarmes _status;
    LimitesAlarmes _param;

    void _recarregaParametros();
    void _verificaVentilacao();
    void _verificaTransicaoFase();
    void _verificaPrimeirosCincoSegundos();
    void _verificaPrimeiroMinuto();

    void _checaApneia();
    void _checaBateria();
    void _checaDesconexaoCircuito();
    void _checaVazamentoCircuito();
    void _checaFalhaComunicacao(); 
    void _checaObstrucaoPorVolume();
    void _checaObstrucaoPorPico();
    void _checaObstrucaoPorTempo();
    void _checaPeepAlta();
    void _checaAutoPeep();
    void _checaPressaoInsAlta();
    void _checaVolumeCorrenteAlto();
    void _checaBaixaPressaoArGrave();
    void _checaBaixaPressaoAr();
    void _checaBaixaPressaoO2Grave();
    void _checaBaixaPressaoO2();
    void _checaFiO2Alta();
    void _checaFiO2Baixa();
    void _checaFonteEletrica();
    void _checaFrequenciaAlta();
    void _checaPressaoInsBaixa();
    void _checaVolumeCorrenteBaixo();
    void _checaVolumeMinutoAlto();
    void _checaVolumeMinutoBaixo();
    void _checaInversaoIE();
    void _checaFalhaConversores();
    void _checaComandoInvalido();
    void _checaFalhaPressaoPiloto(); 
};

#endif
