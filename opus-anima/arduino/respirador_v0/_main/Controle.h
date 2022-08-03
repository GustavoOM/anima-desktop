#ifndef CONTROLE_H
#define CONTROLE_H

#include "_global.h"

class Controle {
	public:
		Controle();
    void setup();
		void loop();
		void iniciaInspiracao(ParametrosVentilacao* param);
		void iniciaPausaIns();
		void iniciaExpiracao();
    void paraRespiracao();
    void cortaFluxoIns();
		void setFiO2Fixo(FiO2FixoEnum fio2Fixo, bool estado);
		void setFalhaSensorPiloto(bool estado);
		void notificaNovosParametrosControle();
		void recarregaParametrosControle();

	private:
		void _reiniciaEstado(ParametrosVentilacao* param);
		void _recarregaParametrosVentilacao(ParametrosVentilacao* param);
		void _setDefaultParametrosAjusteControle();
    void _resetEstadoControle();

		bool _deveRecarregarParametrosControle;
		bool _isFiO2Fixo21;
		bool _isFiO2Fixo100;
};

#endif
