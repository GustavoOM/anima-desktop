#ifndef ESTRATEGIA_PSV_H
#define ESTRATEGIA_PSV_H

#include "_global.h"

class EstrategiaPSV : public Estrategia
{
  MandEspEnum _triggerOnExit;

  //-----------------------------------
  void refreshLocalParameters();
  void step();
  void actionForInicioInspiracao();
  void actionForEmInspiracao();
  void actionForInspiracaoFinalizada();
  void actionForInicioExpiracao();
  void actionForEmExpiracao();
  void actionForExpiracaoFinalizada();

public:
  // from Estragegia
  void loop();
  void startCiclo(ParametrosVentilacao *params);
  bool isFimDeCiclo(MandEspEnum *tipoTriggerProximo);

  //--- especifico para vcv
  EstrategiaPSV();
  void start(ParametrosVentilacao *);
};

#endif
