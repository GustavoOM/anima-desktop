#ifndef MODOPCV_H
#define MODOPCV_H

#include "_global.h"

class ModoPCV {
  public:
    ModoPCV();
    void loop();
  
  private:
    float _geraSetpointPressaoIns();
    float _calculaCorrecaoSet();
    void _correcaoSetCicloCiclo();
};

#endif
