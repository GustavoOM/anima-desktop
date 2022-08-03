#ifndef ESTRATEGIA_H
#define ESTRATEGIA_H

#include "_global.h"

class Estrategia {
  protected:
    EstrategiaStatesEnum _state;        // estado atual do workflow
    ParametrosVentilacao _localParams;
    bool _switchToExpFlg;

    bool triggerDetected();
    bool cicloDetected();
    bool isModoPsvOuDerivado(ModosEnum modo);
  public:
    virtual void loop();
    virtual void startCiclo( ParametrosVentilacao *params);
    virtual bool isFimDeCiclo( MandEspEnum* tipoTriggerProximo);
    void notifySwitchToExp();
    EstrategiaStatesEnum getState();
};

#endif
