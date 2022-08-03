#include "_global.h"


void HistBuffer::add( unsigned int t, double v) {
  _buffer[ _fim].t= t;
  _buffer[ _fim].v= v;
  _fim= (_fim+1) % HISTORY_MAX;

  // se buffer cheio (iFim=iInicio), entao remove o primeiro item
  if( _fim == _inicio) {
    _inicio= (_inicio+1) % HISTORY_MAX;
  }
}

double HistBuffer::getMediaUltimosTms( int t) {
  // buffer vazio
  if( _fim == _inicio) return 0;

  // pega t do ultimo elemento
  int i= _fim-1;
  if( i< 0)
    i= HISTORY_MAX-1;

  // ultimo elemento menos delta t do parmetro
  unsigned int tMin= _buffer[ i].t - t;

  // percorre a lista e soma todos dentro do range
  int n= 0;
  double sum= 0;
  for( int j= _inicio; j!= _fim; j= (j+1)%HISTORY_MAX) {
    if( _buffer[j].t>= tMin) {
      sum= sum+ _buffer[j].v;
      n= n+1;
    }
  } 

  // retorna a media
  if( n> 0) {
    return sum/n;
  }
  return 0;
}
