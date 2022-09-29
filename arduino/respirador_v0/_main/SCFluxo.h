#ifndef SCFLUXO_H
#define SCFLUXO_H

#include "_global.h"

class SCFluxo {
	public:
		SCFluxo();
    void setup();
		void loop();
    void resetPid();
		
	private:
		void _calculaSetArO2();
    void _setDefaultParametrosPid();
};

#endif
