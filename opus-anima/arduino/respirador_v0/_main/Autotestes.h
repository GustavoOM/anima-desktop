#ifndef AUTOTESTES_H
#define AUTOTESTES_H

#include "_global.h"

#define ERRO_PONTO_PILOTO_MAX 10
#define ERRO_PONTO_SISTEMA_MAX 10
#define ERRO_MEDIO_PILOTO_MAX 5
#define ERRO_MEDIO_SISTEMA_MAX 5

#define PRESSAO_TESTE_SEGURANCA 50
#define TEMPO_QUEDA_PRESSAO 1000

#define PRESSAO_TESTE_VAZAMENTO 50
#define VOLUME_VAZADO_ACEITAVEL 0.0667

class Autotestes {
	public:
    Autotestes();
    void realizaAutotestes();

    DadosAutotestes dados;

  private:
    long _testeComunicacao();
    long _testeAlimentacaoEletrica();
    long _testeBateria();
    long _testeAlimentacaoAr();
    long _testeAlimentacaoO2();
    long _testeConversores();
    long _testeSensoresPressao();
    long _testeSensoresFluxo();
    long _testeValvulaAr();
    long _testeValvulaO2();
    long _testeValvulaPiloto();
    long _testeValvulaExalacao();
    long _testeValvulaSeguranca();
    long _testeCircuitoPaciente();

    long _executaTeste(AutotestesEnum teste);
    bool _verificaFalhaValvula(ValvulasAOPEnum valv);
};

#endif
