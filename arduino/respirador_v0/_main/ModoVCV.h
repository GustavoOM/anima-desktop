#ifndef MODOVCV_H
#define MODOVCV_H

#include "_global.h"

class ModoVCV {
	public:
		ModoVCV();
		void loop();

  private:
    float _geraSetpointFluxo();
		float _calculaCorrecaoSet();
    void _correcaoSetInicial();
		void _correcaoSetCicloCiclo();
};

#endif
