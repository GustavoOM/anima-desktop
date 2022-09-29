#include "_global.h"

Filtro::Filtro(FiltrosEnum tipo) {
    memset(_sensorData, 0, sizeof(_sensorData));
    _contadorData = 0;

    if (tipo == FILTRO_ATMEL) {
        // Os coeficientes dependem do TAMS e da frequencia de corte
        // Neste caso, temos um filtro de ordem 10 para um TAMS de 2.5ms e
        // frequencia de corte de 100Hz
        _coefFir[0] = 0.005323;
        _coefFir[1] = -0.020371;
        _coefFir[2] = -0.052071;
        _coefFir[3] = 0.123416;
        _coefFir[4] = 0.443707;
        _coefFir[5] = 0.443707;
        _coefFir[6] = 0.123416;
        _coefFir[7] = -0.052071;
        _coefFir[8] = -0.020371;
        _coefFir[9] = 0.005323;
    } else {
        for(int i=0; i<10; i++) {
            _coefFir[i] = 0.1;
        }
    }
}

float Filtro::filtrar(float sensor){
    _valorFiltrado = 0.0;
    _sensorData[_contadorData] = sensor;

    for (int i=0; i<10; i++) {
        _valorFiltrado += _sensorData[(_contadorData - i + 10) % 10]*_coefFir[i];
    }

    _contadorData = (_contadorData + 1) % 10;
    return _valorFiltrado;
}
