#ifndef ESTRATEGIA_AC_H
#define ESTRATEGIA_AC_H

#include "_global.h"

class EstrategiaAC : public Estrategia
{
  unsigned int _tResp,         // tempo total de respiracao, em ms
      _tExp;                   // tempo de expiracao, em ms
  int _pIns;                   // pressao de inspiracao, para PCV
  MandEspEnum _triggerOnExit;

  //-----------------------------------
  void refreshLocalParameters();
  void step();
  void actionForInicioInspiracao();
  void actionForEmInspiracao();
  void actionForInicioPausaInspiracao();
  void actionForEmPausaInspiracao();
  void actionForInspiracaoFinalizada();
  void actionForInicioExpiracao();
  void actionForEmExpiracao();
  void actionForExpiracaoFinalizada();

public:
  // from Estragegia
  void loop();
  void startCiclo(ParametrosVentilacao *params);
  bool isFimDeCiclo(MandEspEnum *tipoTriggerProximo);

  EstrategiaAC();
  void start(ParametrosVentilacao *);
};

#endif
