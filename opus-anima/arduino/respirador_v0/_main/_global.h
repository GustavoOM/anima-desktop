#ifndef GLOBAL_H
#define GLOBAL_H

//-------< includes >-----------------------------------------------------------

#include <Arduino.h>
#include "HistBuffer.h"



// ------< define operacao >----------------------------------------------------

//#define CALIBRA_AUTOMATICA
//#define CALIBRA_AR_O2_PIL
#define RESPIRADOR
//#define TERMINAL



//-------< define debug >------------------------------------------------------------

/* NÃO HABILITAR PARA UTILIZAÇÃO DA MÁQUINA COMO PRODUTO
 *  APENAS QUANDO SE ESTÁ EM DESENVOLVIMENTO OU MANUNTENÇÃO
 */
// Habilitar para simulação
#define SIMULADOR
// Habilitar para gráfico no Serial Plotter
//#define ARDUINO_PLOTTER



// Definição das portas de I/O dos atuadores
#define ioVP1 9
#define ioVP2 7
#define ioVP3 5
#define ioVSeguranca 3
#define ioAlarme 31
#define ioAlarmeExterno 22
#define ioLigaFonte 4
#define ioLigaCPU 10
#define ioSoftKey 13
#define ioRasp 42

// Definição do ADS1220
#define PGA 1                 // Programmable Gain = 1
#define VREF 2.048            // Internal reference of 2.048V
#define VFSR VREF/PGA
#define FSR (((long int)1<<23)-1)

// Define período de amostragem em [ms]
#define TAMS 2.5
#define TAMS_PID 10

// Define número de iterações por ciclo (TAMS_PID / TAMS)
#define NUM_ITER 4

#define TENSAO_BATERIA_75 (12.0)
#define TENSAO_BATERIA_50 (11.5)
#define TENSAO_BATERIA_25 (11.0)
#define MIN_TENSAO_FONTE (12.5)

#define PRESSAO_MIN_PSI_REG (30)
#define PRESSAO_MAX_PSI_REG (50)

#define SAIDA_ZERO (3.0)
#define FLUXO_BASE (4.0)
#define MINIMO_PRESSAO_PILOTO (-10)

//-------< types >-------------------------------------------------------------

enum ModosEnum
{
  MODO_UNDEFINED = 0,
  MODO_VCV = 1,
  MODO_PCV = 2,
  MODO_PSV = 3,
  MODO_SIMV_VCV_PSV = 4,
  MODO_SIMV_PCV_PSV = 5
};

enum OndaFluxoEnum
{
  FLUXO_UNDEFINED = 0,
  FLUXO_RETANGULAR = 1,
  FLUXO_SENOIDAL = 2,
  FLUXO_DESCENDENTE = 3,
  FLUXO_ASCENDENTE = 4
};

enum FaseEnum
{
  FASE_INS = 1,
  FASE_EXP = 2
};

enum class FaixaEtariaEnum
{
  INDEFINIDO = 0,
  CHILD = 1,
  ADULT = 2
};

enum class SexoEnum
{
  INDEFINIDO = 0,
  FEMALE = 1,
  MALE = 2
};

enum MandEspEnum
{
  MAND_ESP_UNDEFINED = 0,
  MANDATORIO = 1,
  ESPONTANEO = 2
};

enum class EstratTopLayerStates
{
  INICIAL = 0x1,
  INICIO_CICLO = 0x2,
  EM_CICLO = 0x4
};

enum EstrategiaStatesEnum
{
  INICIAL = 0x1,
  INICIO_INSPIRACAO = 0x2,
  EM_INSPIRACAO = 0x4,
  INICIO_PAUSA_INSPIRACAO = 0x8,
  EM_PAUSA_INSPIRACAO = 0x10, //16
  INSPIRACAO_FINALIZADA = 0x20, //32
  INICIO_EXPIRACAO = 0x40,  //64
  EM_EXPIRACAO = 0x80,  //128
  EXPIRACAO_FINALIZADA = 0x100  //256
};

enum ValvulasAOPEnum
{
  VALV_AR = 0,
  VALV_O2 = 1,
  VALV_PILOTO = 2
};

enum AutotestesEnum
{
  INDEFINIDO_TESTE = 0,
  TESTE_COMUNICACAO = 1,
  TESTE_ALIMENTACAO_ELETRICA = 2,
  TESTE_BATERIA = 3,
  TESTE_ALIMENTACAO_AR = 4,
  TESTE_ALIMENTACAO_O2 = 5,
  TESTE_CONVERSORES = 6,
  TESTE_SENSORES_PRESSAO = 7,
  TESTE_SENSORES_FLUXO = 8,
  TESTE_VALVULA_AR = 9,
  TESTE_VALVULA_O2 = 10,
  TESTE_VALVULA_PILOTO = 11,
  TESTE_VALVULA_EXALACAO = 12,
  TESTE_VALVULA_SEGURANCA = 13,
  TESTE_CIRCUITO_PACIENTE = 14,
  ENCERRAR_TESTES = 99
};

enum FiO2FixoEnum
{
  FIO2_FIXO_21 = 1,
  FIO2_FIXO_100 = 2
};

enum SensFluxEnum
{
  SENS_FLUX_AR = 0,
  SENS_FLUX_O2 = 1,
  SENS_FLUX_EX = 2
};

enum MsgSaidaEnum
{
  INDEFINIDO = -1,
  TEXTO = 0,
  MSG_RESPOSTA = 10,
  MSG_GRAFICOS_ALARMES = 20,
  MSG_INDICADORES = 22,
  MSG_RESUL_AUTOTESTES = 24,
  MSG_RESUL_CALIB_FLUXEX = 26,
  MSG_PARAM_SENSORES_PRESSAO = 32,
  MSG_PARAM_SENSOR_FLUXO = 34,
  MSG_PARAM_VALVULA_EXALACAO = 36,
  MSG_PARAM_VALVULAS_AOP = 38
};

enum FiltrosEnum
{
  FILTRO_ATMEL = 1,
  FILTRO_RASP = 2
};

enum TipoEntradaEnum
{
  ENT_PRESSAO = 1,
  ENT_FLUXO = 2,
  ENT_PWM_LONG = 3,
  ENT_ADC = 4,
  ENT_TEMPO_1S = 5,
  ENT_TEMPO_10S = 6,
  ENT_TEMPO_100S = 7,
  ENT_PONTO_AUTOKP = 8,
  ENT_PARAM_PID = 9,
  ENT_BOOL7 = 10,
  ENT_GANHO_BAIXO = 11,
  ENT_VOLUME_TIDAL = 12,
  ENT_FIO2 = 13,
  ENT_ENUM = 14,
  ENT_VOLUME_MIN = 15,
  ENT_SENSIBILIDADE = 16,
  ENT_FREQUENCIA = 17,
  ENT_PID = 18,
  ENT_SENS_FLUX = 19,
  ENT_PWM_FLOAT = 20,
  ENT_DELTA_ESTRANG = 21,
  ENT_PRESSAO_VENT = 22,
  ENT_BOOL1 = 23
};

struct LimitesAlarmes
{
  float pInsPlatMin; // limite inferior para pressão ao final da inspiração
  float pInsMax; 	// limite superir de pressão de inpiração
  unsigned int vTidalInsMin; 	// limite inferior para valor corrente
  unsigned int vTidalInsMax; 	// limite superior para volume corrente
  unsigned int freqMax; 	// limite superior para frequência
  float pPeepMax; 	// limite superior para peep
  unsigned int vMinuMin; 	// limite inferior para volume minuto
  unsigned int vMinuMax; 	// limite superior para volume minuto
  float minFio2; 	// limite inferior para fio2
  float maxFio2; 	// limite superior para fio2
};

struct ComandosOperacao
{
  unsigned int desliga;	  // indica que aparelho deve ser desligado
  unsigned int stop;	  // desliga a ventilacao ate receber novos parâmetros
  unsigned int calibFluxoEx; // deve calibrar sensor de fluxo de expiração
};

struct ParametrosVentilacao
{
  unsigned int t;			//tempo
  ModosEnum modo;			//modo de ventilação
  float fio2;			// fracao de o2 na inspiracao
  OndaFluxoEnum onda;		//onde de fluixo
  unsigned int rTime;		//rising time de pressao
  unsigned int tIns;		// tempo de inspiracao
  unsigned int tImax;		// tempo maximo de inspiracao
  unsigned int freq;		// frequencia de inspiracao por minuto
  unsigned int vTidal;	// volume tidal
  unsigned int vMinu;		// volume minuto
  unsigned int fluxo;		// fluxo contínuo
  float pCon;			// pressão controlada. Para PCV
  float pSup;			// pressão de suporte. Para PSV
  float pInsLimite; 	// limite superir de pressão de inpiração

  unsigned int tRise; // tempo de subida
  unsigned int tSupe;		// tempo superior
  float pSupe;			// pressão superior
  unsigned int tInf;		// tempo inferior
  unsigned int pinf;		// pressão inferior
  float peep;			// PEEP
  float sensInsP;		// sensibilidade de inspiração por pressão
  float sensInpF;		// sensibilidade de inspiração por fluxo
  unsigned int tPausaIns; // tempo de pausa inspiratória
  float sensExpF;		// sensibilidade de expiração por fluxo
  unsigned int tApneia; //tempo de apnéia

  /**
     Parametros do modo backup, acionados em caso de apneia no modo espontaneo (PSV)
  */
  ModosEnum bkModo;   // modo backup. VCV ou PCV
  unsigned int bkTIns;  // tempo de inpiracao
  unsigned int bkFreq;  // fequencia
  unsigned int bkVTidal;  // volume tidal, em caso de VCV
  float bkPcon;  // pressao de controle, em caso de PCV

  /**
     Perfis do paciente
  */
  FaixaEtariaEnum faixaEtaria;
  SexoEnum sexo;
  unsigned int altura;  // altura em cm
  float peso; // peso em kg
};

struct StatusCiclo
{
  unsigned long tInicioIns;
  unsigned long tInicioPausaIns;
  unsigned long tInicioExp;
  unsigned long tFimExp;
  unsigned long tInicioFluxoZero;
  unsigned long tInicioFluxoZeroExp;
  ModosEnum modo;		 //	modo ventilatório
  FaseEnum fase;		 //	fase inspiratória ou expiratória
  MandEspEnum mandEsp; //	flg de mandatório ou espontâneo
  double vTidal;
  double vTidallns;	  //	volume corrente inspiratório
  double vTidalExp;	  //	volume corrente expiratório
  double vTidalInsO2;  // volume de O2 tidal
  double vTidalInsAr;  // volume de ar tidal
  double pPico;		  //	pressão de pico
  double pPicoIns;    // pressão inspiratória de pico
  double pPlateau;	  //	pressão de plateau
  double pInsMinima;		  //	pressão minima no ciclo
  double pExpMinima;		  //	pressão minima no ciclo
  unsigned int tIns;	  //	tempo inpiratório
  unsigned int tExp;	  //	tempo expiratório
  double fluxoInspPico; //	Pico de fluxo inspiratório
  double fluxoExpPico; //	Pico de fluxo inspiratório
  double peepTotal;	  //	PEEP total
  double autoPeep;	  //	Auto-PEEP
  double pTotal;
  double tStartSomaPressao;
  MandEspEnum triggerOnExit;
  unsigned int tInicioTriggerInsF;
  unsigned int tInicioTriggerInsP;
  unsigned int tInicioTriggerExp;
  double pPlateau50ms;    // plateau calculado como média dos ultimos 50ms

  double fluxo;  // fluxo no loop corrente

  /**
     Historico de pressao
  */
  HistBuffer histPIns;
  HistBuffer histPExp;
};

#define MAX_CICLO_PACIENTE (60)

/**
   Estrutura para armazenar informacoes durante um certo periodo x, em geral 1 minuto
*/
struct HistoricoStatusPaciente {
  unsigned int t;
  double vIns;		  //	volume inspiratorio
  double vExp;		  //	volume expiratorio
};

struct StatusPaciente
{
  unsigned int tInicioPaciente;
  FaixaEtariaEnum faixaEtaria;
  SexoEnum sexo;
  unsigned int altura;  // altura em cm
  float peso; // peso em kg
  unsigned int hInicio;   // aponta para o primeiro elemento, ou quando igual a hFim, é lista vazia
  unsigned int hFim;  // aponta para o elemento seguinte ao ultimo elemento. Ou seja,
  // para a proxima posicao de escrita. O ultimo elemento não é preenchido
  // porque senao lista vazia e lista cheia ficam indistinguiveis
  HistoricoStatusPaciente hist[MAX_CICLO_PACIENTE];  // buffer ciclico para guardar valores necessariso do ultimo minuto

  // unsigned long tInicioMinLoop;
  double vMinIns;		  //	volume inspirado da ultima janela de 1 minuto
  double vMinExp;		  //	volume expirado da ultima janela de 1 minuto
  double vTidalIns;	  //	volume tidal inspirado do ultimo ciclo
  double vTidalExp;	  //	volume tidal expirado do ultimo ciclo
  double fio2; // proporcao de oxigenio
  double pPico;		  //	pressão de pico
  double pPlateau;	  //	pressão de plateau, calculado no fluxo zero
  double pMed;		  //	pressão média
  double pBase;		  //	pressão de base
  double pInsMinima;		  //	pressão minima no ciclo
  double pExpMinima;		  //	pressão minima no ciclo
  unsigned int tIns;	  //	tempo inpiratório
  unsigned int tExp;	  //	tempo expiratório
  // conta numero de triggers espontaneo consecutivo.
  // usado em alarmes
  unsigned int nTriggerEspConsecut;
  double relTempIE;	  //	relação tempo inspiração/expiração
  double fluxoInspPico; //	Pico de fluxo inspiratório
  double cstat;		  //	complacencia estática
  double cdyn;		  //	complacencia dinâmica
  double rinsp;		  //	resistência inpiratória estática
  double rexp;		  //	resistência expiratória estática
  double peepTotal;	  //	PEEP total
  double autoPeep;	  //	Auto-PEEP
  double peep;		  //  	PEEP
  int freq;		  //  	frequencia
  double pPlateau50ms;    // plateau calculado como média dos ultimos 50ms
  double peep50ms; // peep calculado como média dos ultimos 50ms

};

struct StatusAlarmes
{
  bool alarmesAtivos[32];

  bool isInicioIns;
  bool isInicioExp;
  bool analiseObstrucaoPorTempo;
  bool cicloOkPressaoInsAlertaVcv;
  bool cicloOkPressaoInsAlta;
  bool isEmVentilacao;
  bool isPrimeirosCincoSegundos;
  bool isPrimeiroMinuto;

  int contadorObstrucaoPorVolume;
  int contadorAutoPeep;
  int contadorFrequenciaAlta;
  int contadorInversaoIE;
  int contadorvMinBaixo;
  int contadorvMinAlto;
  int contadorVazamentoCircuito;
  int contadorDesconexaoCircuito;
  int contadorConvFluxoAr;
  int contadorConvFluxoO2;
  int contadorConvFluxoEx;
  int contadorConvPressaoIns;
  int contadorConvPressaoExp;
  int contadorConvPressaoPil;

  FaseEnum faseAnterior;
  float fio2GraveO2Buffer;
  float fio2GraveArBuffer;
  unsigned long inicioObstrucaoPorTempo;
};


// Parâmetros do Controle setados pelo usuário
struct ParametrosCicloControle
{
  OndaFluxoEnum tipoCurva;
  float volume;
  float fio2;
  float fluxoBase;
  float pressaoCon;
  float pressaoSup;
  float peep;
  float pressaoMax;
  float tIns;
  float tSubida;
};


// Parâmetros do Controle definidos por calibração
struct ParametrosAjusteControle
{
  float Kr;
  float deltaEstrangMin;
  float deltaEstrangMax;
  float minCCVolume;
  float maxCCVolume;
  float fatorCCVolume;
  float minCCPressao;
  float maxCCPressao;
  float fatorCCPressao;
};


// Variáveis de estado do Controle
struct EstadoControle
{
  ModosEnum modoAtual;
  ModosEnum modoAnterior;
  EstrategiaStatesEnum faseAtual;
  EstrategiaStatesEnum faseAnterior;

  float setFluxo;
  float setPressaoIns;
  float setPressaoExp;
  float corrCCVolume;
  float corrCCPressao;

  long contIterSetFluxo;
  long contIterSetPressaoIns;

  bool isFiO2Fixo21;
  bool isFiO2Fixo100;
  bool isSensorPilotoFalho;
};


// Parâmetros dos PIDs definidos por calibração
struct ParametrosPid
{
  float Kp;
  float Ti;
  float Td;
  float N;
  float Tt;
  float I0;
  float minn;
  float maxx;

  float pontosAutoKp[5];
  float ganhosAutoKp[5];
  bool autoKpPorDeltaSet;
  float histerese;
};


// Variáveis de estado dos PIDs
struct EstadoPid
{
  float ganho;
  float integral;
  float setpointAtual;
  float sensorAtual;
  float setpointAnterior;
  float sensorAnterior;
  float derivativoAnterior;
  float refDelta;

  float saidaProp;
  float saidaInte;
  float saidaDeri;
  float saidaCalc;
  float saidaSatu;
  float saidaSemCompHist;
  float saidaAtual;
  float saidaAnterior;
};


struct ComandosDegrauCalibracao
{
  int valvula;
  int periodoDeg;
  float valor1Deg;
  float valor2Deg;
};


struct DadosRecebidosLoop
{
  ParametrosVentilacao ventilacao;
  LimitesAlarmes limitesAlarmes;
  ComandosOperacao operacao;
  ParametrosPid pid[5];
  ParametrosAjusteControle controle;
  ComandosDegrauCalibracao degrau;
};

struct ParametrosSensoresPressao
{
  long minAdcNoZero[3];
  long maxAdcNoZero[3];
};

struct ParametrosSensorFluxo
{
  long curvaAdc[18];
  float curvaFluxo[18];
  long minAdcNoZero;
  long maxAdcNoZero;
};


struct ParametrosValvulaExalacao
{
  float curvaSistema[11];
  float curvaPiloto[11];
  float limiaresSistema[9];
  float minPilotoNoLimiarSistema[9];
  float maxPilotoNoLimiarSistema[9];
  float minPressaoPicoSistema;
};

struct ParametrosValvulasAOP
{ 
  long pwmAberturaMin[3];
  long pwmAberturaMax[3];
  long pwmFechamentoMin[3];
  long pwmFechamentoMax[3];
  float saidaPicoMin[3];
  float saidaPicoMax[3];
  int tempoMaxValvSeguranca;
};


struct StatusDadosInicializacao
{
  bool paramsSensPressaoRecebidos;
  bool paramsSensFluxRecebidos[3];
  bool paramsValvExaRecebidos;
  bool paramsValvsAOPRecebidos;
  bool paramsPidRecebidos[5];
  bool paramsControleRecebidos;
};


// Estado de operação do gráfico no Serial Plotter do Arduino
struct ModoGrafico
{
  int curvasAtuais[4];
  int curvasAnteriores[4];
  EstadoPid *pidRef;
};


// Dados coletados durante os testes iniciais
struct DadosAutotestes
{
  int pwmAbertura[3];
  int pwmFechamento[3];
  float saidaPico[3];

  float curvaExalacaoSistema[11];
  float curvaExalacaoPiloto[11];
  
  int tempoValvulaSeguranca;
};


struct StatusSensores
{
  float tensaoFonte;
  float tensaoBateria;
  float fluxoCalibracao;
  /*
    [0] => ---
    [1] => P1: Pressão do Regulador de Ar
    [2] => P2: Pressão do Regulador de O2
    [3] => P3: Pressão de Inalação
    [4] => P4: Pressão de Exalação
    [5] => P5: Pressão Piloto
  */
  float valorPressao[6];
  long adcPressao[6];
  float offsetPressao[6];
  bool erroTimeoutPressao[6];
  bool erroOffsetPressao[6];
  bool falhaConversorPressao[6];

  /*
    [0] => ---
    [1] => F1: Fluxo de Ar
    [2] => F2: Fluxo de O2
    [3] => F3: Fluxo de Inalação
    [4] => F4: Fluxo de Exalação
  */
  float valorFluxo[5];
  long adcFluxo[5];
  float offsetFluxo[5];
  bool erroTimeoutFluxo[5];
  bool erroOffsetFluxo[5];
  bool falhaConversorFluxo[5];
};


struct Medidas
{
  float fluxoAr;
  float fluxoO2;
  float fluxoIns;
  float fluxoExp;
  float fluxoPac;
  float pressaoSis;
  float pressaoIns;
  float pressaoPil;
};

struct DadosTemporizacao
{
  unsigned long tUtilIter[4];
  unsigned long tTotalIter[4];
  unsigned long tUtilCiclo;
  unsigned long tTotalCiclo;
};

//-------< includes >-----------------------------------------------------------

#include "Estrategia.h"
#include "EstrategiaAC.h"
#include "EstrategiaPSV.h"
#include "EstrategiaTopLayer.h"
#include "Controle.h"
#include "Alarmes.h"
#include "EntradaRasp.h"
#include "SaidaRasp.h"
#include "Respirador.h"
#include "Metricas.h"
#include "ModoVCV.h"
#include "ModoPCV.h"
#include "Pid.h"
#include "SCFluxo.h"
#include "SCPressaoExp.h"
#include "SCPressaoIns.h"
#include "SimuladorLogico.h"
#include "ADS1220.h"
#include "Filtro.h"
#include "Sensores.h"
#include <SPI.h>
#include "Valvula.h"
#include "Autotestes.h"
#include "Ensaio.h"

//-------< types >--------------------------------------------------------------

struct ObjetosCalibrador
{
  Pid *pid;
  ParametrosPid *paramPid;
  EstadoPid *estadoPid;
  int valvulaAnterior;
};



//-------< global variables >--------------------------------------------------

// Pwm de entrada das válvulas
extern int pwmValvAr;
extern int pwmValvO2;
extern int pwmValvPil;
extern bool valvSegAberta;

extern ADS1220 sensorP3;
extern ADS1220 sensorP4;
extern ADS1220 sensorP5;
extern ADS1220 sensorP6;
extern ADS1220 sensorF1;
extern ADS1220 sensorF2;
extern ADS1220 sensorF3;
extern ADS1220 sensorF4;

extern bool paradaMaquina;

//-------< global types >--------------------------------------------------

// Metricas
extern StatusPaciente statusPaciente;
extern StatusCiclo statusCiclo;

// Controle
extern ParametrosCicloControle paramCicloControle;
extern ParametrosAjusteControle paramAjusteControle;
extern EstadoControle estadoControle;

// PIDs
extern ParametrosPid paramPidFluxoAr;
extern ParametrosPid paramPidFluxoO2;
extern ParametrosPid paramPidPressaoPil;
extern ParametrosPid paramPidPressaoExp;
extern ParametrosPid paramPidPressaoIns;
extern EstadoPid estadoPidFluxoAr;
extern EstadoPid estadoPidFluxoO2;
extern EstadoPid estadoPidPressaoPil;
extern EstadoPid estadoPidPressaoExp;
extern EstadoPid estadoPidPressaoIns;

// Outros
extern StatusSensores statusSensores;
extern Medidas medidas;
extern Medidas medidasRasp;
extern ComandosDegrauCalibracao paramCal;
extern ObjetosCalibrador objetosCal;
extern ModoGrafico grafico;
extern ParametrosSensoresPressao paramSensoresPressao;
extern ParametrosSensorFluxo paramSensorFluxoAr;
extern ParametrosSensorFluxo paramSensorFluxoO2;
extern ParametrosSensorFluxo paramSensorFluxoEx;
extern ParametrosValvulaExalacao paramValvulaExalacao;
extern ParametrosValvulasAOP paramValvulasAOP;


//-------< global classes >--------------------------------------------------

// Camada da Estrategia
extern Respirador respirador;
extern Alarmes alarmes;
extern EstrategiaTopLayer estrategiaTop;
extern EstrategiaAC estrategiaAC;
extern EstrategiaPSV estrategiaPSV;
extern EntradaRasp entradaRasp;
extern SaidaRasp saidaRasp;
extern Metricas metricas;

// Camada do Controle
extern Controle controle;
extern ModoVCV modoVCV;
extern ModoPCV modoPCV;
extern SCFluxo scFluxo;
extern SCPressaoExp scPressaoExp;
extern SCPressaoIns scPressaoIns;
extern Pid pidFluxoAr;
extern Pid pidFluxoO2;
extern Pid pidPressaoExp;
extern Pid pidPressaoPil;
extern Pid pidPressaoIns;

// Outros
extern SimuladorLogico sim;
extern Sensores sensores;
extern Autotestes autotestes;
extern Valvula valvulaCal;
extern Ensaio ensaio;


//-------< functions >---------------------------------------------------------

// Funções para atuar nas válvulas
extern void valvulaFluxoArVP1(int periodo);
extern void valvulaFluxoOxVP2(int periodo);
extern void valvulaOxExalacaoVP3(int perido);
extern void valvulaSeguranca(bool abre);

// Utils
extern float mapFloat(float x, float in_min, float in_max, float out_min, float out_max);
extern float mapCurvaFloat(float pontoX, float* curvaX, float* curvaY, int tamCurva);
extern void desligaValvulas();
extern boolean passouTempoMillis(unsigned long inicio, unsigned long intervalo);
extern boolean passouTempoMicros(unsigned long inicio, unsigned long intervalo);
extern float saturaFloat(float valor, float minimo, float maximo);
extern unsigned long calculaChecksum(char* msgBuf, int fimMsg);

#endif
