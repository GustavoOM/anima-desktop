#ifndef SIMULADORLOGICO_H_
#define SIMULADORLOGICO_H_

class SimuladorLogico {
	public:
		SimuladorLogico();
		void setup();
		void loop(bool update);

	private:
		void _stepValvAr(float ivar, float* far, bool update);
		void _stepValvO2(float ivo2, float* fo2, bool update);
		void _stepValvEx(float ivex, float* rexp, bool update);
		void _stepSistResp(float fins, float rexp, float* pboc, float* fpac, bool update);
		float _lookupValvEx(float input);
    float _convValvAr(float pwm);
    float _convValvO2(float pwm);
		void _initArray(float* arr, int n);
		float _rpac, _cpac, _sum_fpac;
		float _inA[2], _inB[3], _inC[2], _inD[3], _inE[4];
		float _outA[2], _outB[3], _outC[2], _outD[3], _outE[4];
};

#endif
