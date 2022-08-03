#include "_global.h"

SimuladorLogico::SimuladorLogico() { }

void SimuladorLogico::setup() {
	_rpac = 20;
	_cpac = 30;
	_sum_fpac = 0.0;

  // Zera as variáveis de entrada das funções de transferência
	_initArray(_inA, 2);
	_initArray(_inB, 3);
	_initArray(_inC, 2);
	_initArray(_inD, 3);
	_initArray(_inE, 4);

  // Zera as variáveis de saída das funções de transferência
	_initArray(_outA, 2);
	_initArray(_outB, 3);
	_initArray(_outC, 2);
	_initArray(_outD, 3);
	_initArray(_outE, 4);
}

// Executa uma iteração do simulador; [update] define se a execução
// alterará ou não as variáveis de estado do simulador
void SimuladorLogico::loop(bool update) {
	float far, fo2, rexp, pboc, fpac;

	float correnteValvAr = pwmValvAr*200.0/4095.0;
	float correnteValvO2 = pwmValvO2*200.0/4095.0;
	float correnteValvEx = pwmValvPil*1200.0/4095.0;

  // São passados os endereços das variáveis nas quais serão armazenados
  // os valores de retorno dos métodos
	_stepValvAr(correnteValvAr, &far, update);
	_stepValvO2(correnteValvO2, &fo2, update);
	_stepValvEx(correnteValvEx, &rexp, update);
  float fins = far + fo2;
  
	_stepSistResp(fins, rexp, &pboc, &fpac, update);

  // Atualiza sensores
	medidas.fluxoAr = far;
	medidas.fluxoO2 = fo2;
	medidas.fluxoIns = far + fo2;
	medidas.fluxoExp = (far + fo2) - fpac;
	medidas.fluxoPac = fpac;
	medidas.pressaoSis = pboc;
  medidas.pressaoIns = pboc;

	medidasRasp.fluxoAr = medidas.fluxoAr;
	medidasRasp.fluxoO2 = medidas.fluxoO2;
	medidasRasp.fluxoIns = medidas.fluxoIns;
	medidasRasp.fluxoExp = medidas.fluxoExp;
	medidasRasp.fluxoPac = medidas.fluxoPac;
	medidasRasp.pressaoSis = medidas.pressaoSis;
  medidasRasp.pressaoIns = medidas.pressaoIns;
}

// Resposta em fluxo da válvula proporcional de ar de acordo com a corrente de entrada e do estado atual
void SimuladorLogico::_stepValvAr(float ivar, float* far, bool update) {
	// Função de transferência de 1ª ordem
	float inputA = ivar;
	float outputA = 0.19781*_inA[1] + 0.67032*_outA[1];

	// Função de transferência de 2ª ordem
	float inputB = ivar;
	float temp1 = (9.12877/1000.0)*_inB[1] - (9.12877/1000.0)*_inB[2];
	float temp2 = 1.82458*_outB[1] - 0.83110*_outB[2];
	float outputB = temp1 + temp2;

  // Saturação de saída
	float far_nsat = outputA + outputB;
	if (far_nsat < 0.0) {
		*far = 0.0;
	} else if (far_nsat > 130.0) {
		*far = 130.0;
	} else {
		*far = far_nsat;
	}

  // Atualização das variáveis de estado
	if (update) {
		_inA[1] = inputA;
		_outA[1] = outputA;

		_inB[2] = _inB[1];
		_outB[2] = _outB[1];
		
		_inB[1] = inputB;
		_outB[1] = outputB;
	}
}

// Resposta em fluxo da válvula proporcional de O2 de acordo com a corrente de entrada e o estado atual
void SimuladorLogico::_stepValvO2(float ivo2, float* fo2, bool update) {
	// Função de transferência de 1ª ordem
	float inputC = ivo2;
	float outputC = 0.19781*_inC[1] + 0.67032*_outC[1];

	// Função de transferência de 2ª ordem
	float inputD = ivo2;
	float temp1 = (9.12877/1000.0)*_inD[1] - (9.12877/1000.0)*_inD[2];
	float temp2 = 1.82458*_outD[1] - 0.83110*_outD[2];
	float outputD = temp1 + temp2;

  // Saturação de saída
	float fo2_nsat = outputC + outputD;
	if (fo2_nsat < 0.0) {
		*fo2 = 0.0;
	} else if (fo2_nsat > 130.0) {
		*fo2 = 130.0;
	} else {
		*fo2 = fo2_nsat;
	}

  // Atualização das variáveis de estado
	if (update) {
		_inC[1] = inputC;
		_outC[1] = outputC;

		_inD[2] = _inD[1];
		_outD[2] = _outD[1];
		
		_inD[1] = inputD;
		_outD[1] = outputD;
	}
}

// Resposta em resistência à expiração da válvula de exalação de acordo com a corrente de entrada e o estado atual
void SimuladorLogico::_stepValvEx(float ivex, float* rexp, bool update) {
  
	// Função de transferência
	float inputE = ivex;
	float temp1 = 0.20690*_inE[1] - 0.37610*_inE[2] + 0.17049*_inE[3];
  float temp2 = 2.49490*_outE[1] - 2.05416*_outE[2] + 0.55711*_outE[3];
	float outputE = temp1 + temp2;
  // Normalização da resposta da FT 
	float temp3 = outputE*(0.55711/0.17049);
	*rexp = _lookupValvEx(temp3);

  // Atualização das variáveis de estado
	if (update) {
		_inE[3] = _inE[2];
		_outE[3] = _outE[2];
		
		_inE[2] = _inE[1];
		_outE[2] = _outE[1];
		
		_inE[1] = inputE;
		_outE[1] = outputE;
	}
}

// Lookup table da resistência à expiração em função da corrente de entrada na válvula
// de exalação em regime permanente
float SimuladorLogico::_lookupValvEx(float input) {
	if (input < 0) {
    return 0.0;
	} else if (input <= 200) {
		return (0.375*input);
	} else if (input <= 700) {
		return (1.35*input - 195.0);
	} else if (input <= 1000) {
		return (5.8333*input - 3333.3);
	} else if (input <= 1200) {
		return (29987.5*input - 29985000.0);
	} else {
    return 6000000.0;
	}
}

// Resposta do sistema respiratório de acordo com o fluxo inspiratório, a resistência
// à expiração e o estado atual
void SimuladorLogico::_stepSistResp(float fins, float rexp, float* pboc, float* fpac, bool update) {
  float temp1 = _cpac*(rexp/60.0)*fins - _sum_fpac;
	float temp2 = _cpac*((_rpac/60.0) + (rexp/60.0));
	*fpac = temp1 / temp2;
  
	float palv = _sum_fpac / _cpac;
	float temp3 = ((fins*(_rpac/60.0) + palv)*(rexp/60.0));
	float temp4 = ((_rpac/60.0) + (rexp/60.0));
	*pboc = temp3 / temp4;

	if (update) {
		_sum_fpac += (1000.0/60.0)*(TAMS_PID/1000.0)*(*fpac);
	}
}

// Zera arrays
void SimuladorLogico::_initArray(float* arr, int n) {
	for (int i=0; i<n; i++) {
		arr[i] = 0.0;
	}
}
