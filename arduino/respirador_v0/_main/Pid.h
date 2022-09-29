#ifndef PID_H
#define PID_H

#include "_global.h"

class Pid {
	public:
    Pid();
    void setup(ParametrosPid *param, EstadoPid *estado);
		void loop();
		void moverIntegral();
    void resetEstadoPid();

  private:
    void _zeraSaida();
    void _autoKp();
    void _checaTt();
    ParametrosPid *_param;
    EstadoPid *_estado;
};

#endif
