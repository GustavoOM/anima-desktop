#ifndef SCPRESSAOEXP_H
#define SCPRESSAOEXP_H

#include "Pid.h"

class SCPressaoExp {
	public:
		SCPressaoExp();
    void setup();
		void loop();
    void resetPid();

	private:
    bool _isTransicaoFase();
    void _ajusteEstrangulamento();
    void _setDefaultParametrosPid();
};

#endif
