#ifndef METRICAS_H
#define METRICAS_H

#include "_global.h"

class Metricas {
	private:
		unsigned int 
      _tLastLoop,    // marca o ultimo t de medida
			_tCurLoop      // marca o t atual de medida
			;
    
	public:
    bool printFlg= false;

		void setup();
		void loop();
    void atualizaStatusPacienteEmFimCiclo(MandEspEnum trigger);
    void calculaMatricasEachLoop();
};

#endif