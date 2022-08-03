#ifndef SCPRESSAOINS_H
#define SCPRESSAOINS_H

#include "Pid.h"

class SCPressaoIns {
	public:
		SCPressaoIns();
    void setup();
		float loop();
    void resetPid();

  private:
    bool _isInicioInspiracao();
    void _ajustePid();
    void _setDefaultParametrosPid();
};

#endif
