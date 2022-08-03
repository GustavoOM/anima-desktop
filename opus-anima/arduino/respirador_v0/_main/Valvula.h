#ifndef VALVULA_H
#define VALVULA_H

#include "Pid.h"

class Valvula {
	public:
		Valvula();
		void setup();
		void loop();
    void resetPid();

	private: 
    float _calcSetpoint();
    void _setDefaultParametrosPid();
    void _resetPid();
		int _it;
};

#endif
