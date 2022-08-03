#ifndef HIST_BUFFER_H
#define HIST_BUFFER_H

#define HISTORY_MAX (50)

struct HistContent {
  unsigned int t;
  double v;
};

class HistBuffer
{
public:
  void add( unsigned int t, double v);
  double getMediaUltimosTms( int t);

private:
  HistContent _buffer[HISTORY_MAX];
  int _inicio;
  int _fim;
};

#endif
