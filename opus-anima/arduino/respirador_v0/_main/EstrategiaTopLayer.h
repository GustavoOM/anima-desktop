#ifndef ESTRATEGIA_TOP_LAYER_H
#define ESTRATEGIA_TOP_LAYER_H

#include "_global.h"

class EstrategiaTopLayer {
  public:
    EstrategiaTopLayer();
    void setup();
    void loop();
    void notifyStopVentilacao(bool isFromAlarm);
    void notifyRestartVentilacao();
    void notifyFiO2Fixo21();
    void desativaFiO2Fixo21();
    void notifyFiO2Fixo100();
    void desativaFiO2Fixo100();
    void notifyFalhaSensorFluxoExp();
    void stopVentilacao();
    ParametrosVentilacao params;
    void notifyImediatoModoBackup();
    void notifyDesligaModoBackupNoProximoCiclo();
    void notifySwitchToExp();
    EstrategiaStatesEnum getState();
    bool isModoBackupAtivo();

  private:
    Estrategia *_estrategia;
    EstratTopLayerStates _state;

    MandEspEnum _trigger;    // triger que iniciou o ciclo atual
    bool _mustStopNowFlg;
    bool _isStoppedFlg; 
    bool _falhaSensorFluxoExpflg;
    bool _modoBackupFlg;
    bool _runModoBackup;
    bool _fio2Fixo21;
    bool _fio2Fixo100;
    bool _restartBlocked;

    //-----------------------------------------------
    void reloadParamsIfExists();
    void iniciaNovoCiclo();
    void step();
    void actionForInicial();
    void actionForInicioCiclo();
    void actionForEmCiclo();
};

#endif
