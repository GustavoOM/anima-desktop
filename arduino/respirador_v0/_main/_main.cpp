#include "_global.h"
#include <malloc.h>
#include <stdlib.h>

void setup();
void loop();
void setupSimulacao();


int main(){
  setup();
  while(1){
    loop();
  }
}


void setup() {
  setupSimulacao();
  respirador.setup();
}

void loop() {
  if (respirador.contIter == 2) { sim.loop(false); }
  respirador.loop();
  if (respirador.contIter == 2) { sim.loop(true); }
}

void setupSimulacao() {
  sim.setup();
  statusSensores.valorPressao[1] = 35;
  statusSensores.valorPressao[2] = 35;
  statusSensores.tensaoBateria = 13;
  statusSensores.tensaoFonte = 13;
}