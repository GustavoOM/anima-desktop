#ifndef FILTRO_H
#define FILTRO_H

class Filtro {
  public:
    Filtro(FiltrosEnum tipo);
    float filtrar(float sensor);
  private:
    int _contadorData;
    float _sensorData[10];
    float _coefFir[10];
    float _valorFiltrado;  
};

#endif
