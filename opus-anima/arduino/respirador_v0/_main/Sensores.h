#ifndef SENSORES_H
#define SENSORES_H

class Sensores {
    public:
        Sensores();
        void leSensores();
        void leSensoresAlimentacao();
        void leSensoresPressao();
        void leSensoresFluxo();
        void zeraSensores();
        void zeraSensoresPressao();
        void zeraSensoresFluxo();
        void calibraFluxoEx();

    private:
        float _leSensorAlimentacaoEletrica(int pino);
        float _leSensorAlimentacaoGas(int pino);
        void _leSensorPressao(int idSensor, ADS1220 *conversor, Filtro *filtro);
        void _leSensorFluxo(int idSensor, ADS1220 *conversor, Filtro *filtroFluxo, Filtro* filtroAdc);
        void _leSensorFluxoCalibracao();
        float _calculaFluxo(int idSens, float adc);
        void _defineMedidas();
        void _defineMedidasRasp();
        void _zeraSensorPressao(int idSensor, ADS1220 *conversor);
        void _zeraSensorFluxo(int idSensor, ADS1220 *conversor);

        bool _sensoresZerados;
        
        Filtro  *_filtroP3,
                *_filtroP4,
                *_filtroP5,
                *_filtroF1,
                *_filtroF2,
                *_filtroF4,
                *_filtroFCal,
                *_filtroAdcF1,
                *_filtroAdcF2,
                *_filtroAdcF4,
                *_filtroFluxoArRasp,
                *_filtroFluxoO2Rasp,
                *_filtroFluxoInsRasp,
                *_filtroFluxoExpRasp,
                *_filtroFluxoPacRasp,
                *_filtroPressaoSisRasp,
                *_filtroPressaoPilRasp; 
};

#endif
