#ifndef ENSAIO_H
#define ENSAIO_H

#include "_global.h"

#define PRESSAO_ENSAIO_VALV_SEG 50
#define PERIODO_ENSAIO_VALV_SEG 5000

class Ensaio {
	public:
		Ensaio();
        void setup();
        bool ensaiaSensorFluxo(SensFluxEnum sensFlux, ParametrosSensorFluxo *dados);
        bool ensaiaValvulaAOP(ValvulasAOPEnum valv, float* dadosValv);
        bool ensaiaValvulaExalacao(float* curvaExalacaoPiloto, float* curvaExalacaoSistema, float* limiares);
        bool ensaiaValvulaSeguranca(int* tResposta);
        bool ensaiaCircuitoPaciente(float* volumeVazado);
        void simulaSensores();

    private:
        bool _ajustaFluxoBase(bool usarSensorFluxoCalib);
        bool _ajustaPilotoBase();
        void _leSensoresPorTempo(int periodo);
        void _acionaValvulaAOP(ValvulasAOPEnum valv, int pwm);
        float _medeValvulaAOP(ValvulasAOPEnum valv);
        void _acionaValvParaSensFlux(SensFluxEnum sensFlux, int pwm);
        float _medeAdcSensFlux(SensFluxEnum sensFlux);
        float _medeSensFlux(SensFluxEnum sensFlux);

        char _charBuf[100];
        int _pwmFluxoBase;
        int _pwmPilotoBase;
        int _tLeitura;
};

#endif
