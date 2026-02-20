#pragma once

#ifndef _INTERFACE_H_
#define _INTERFACE_H_

extern "C" {
#ifdef _WIN32
	__declspec(dllexport)
#endif
		void Initialize();

#ifdef _WIN32
	__declspec(dllexport)
#endif
		void Finalize();

#ifdef _WIN32
	__declspec(dllexport)
#endif
		unsigned long int CreateSimulator(int nrQubits);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		void DestroySimulator(unsigned long int simHandle);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		unsigned long int CloneSimulator(void* sim);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		void* GetSimulator(unsigned long int simHandle);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		int GetNumQubits(void* sim);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		double GetQubitProbability0(void* sim, int qubit);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		double GetQubitProbability1(void* sim, int qubit);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		double GetOutcomeProbability(void* sim, long long int outcome);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		double GetExpectationValue(void* sim, const char* pauliStr);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		int Measure(void* sim, int qubit);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		long long int MeasureQubits(void* sim, int* qubits, int numQubits);


#ifdef _WIN32
	__declspec(dllexport)
#endif
		void ApplyP(void* sim, int qubit, double angle);


#ifdef _WIN32
	__declspec(dllexport)
#endif
		void ApplyX(void* sim, int qubit);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		void ApplyY(void* sim, int qubit);


#ifdef _WIN32
	__declspec(dllexport)
#endif
		void ApplyZ(void* sim, int qubit);


#ifdef _WIN32
	__declspec(dllexport)
#endif
		void ApplyH(void* sim, int qubit);


#ifdef _WIN32
	__declspec(dllexport)
#endif
		void ApplyS(void* sim, int qubit);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		void ApplyT(void* sim, int qubit);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		void ApplyRx(void* sim, int qubit, double angle);

#ifdef _WIN32
	__declspec(dllexport)
		#endif
		void ApplyRy(void* sim, int qubit, double angle);

#ifdef _WIN32
	__declspec(dllexport)
		#endif
		void ApplyRz(void* sim, int qubit, double angle);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		void ApplyCS(void* sim, int control, int target);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		void ApplyCT(void* sim, int control, int target);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		void ApplyCH(void* sim, int control, int target);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		void ApplySwap(void* sim, int qubit1, int qubit2);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		void ApplyCX(void* sim, int control, int target);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		void ApplyCY(void* sim, int control, int target);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		void ApplyCZ(void* sim, int control, int target);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		void ApplyCRx(void* sim, int control, int target, double angle);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		void ApplyCRy(void* sim, int control, int target, double angle);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		void ApplyCRz(void* sim, int control, int target, double angle);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		void ApplyCSwap(void* sim, int control, int qubit1, int qubit2);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		void ApplyCCX(void* sim, int control1, int control2, int target);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		void ApplySdg(void* sim, int qubit);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		void ApplyTdg(void* sim, int qubit);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		void ApplySx(void* sim, int qubit);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		void ApplySxDg(void* sim, int qubit);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		void ApplyK(void* sim, int qubit);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		void ApplyU(void* sim, int qubit, double theta, double phi, double lambda, double gamma);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		void ApplyCU(void* sim, int control, int target, double theta, double phi, double lambda, double gamma);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		void ApplyCP(void* sim, int control, int target, double angle);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		void ApplyCSx(void* sim, int control, int target);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		void ApplyCSxDg(void* sim, int control, int target);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		int GetAmplitudes(void* sim, void* buffer, size_t bufSize);

#ifdef _WIN32
	__declspec(dllexport)
#endif
		int GetAmplitude(void* sim, long long int index, void* outAmp, size_t bufSize);

#ifdef _WIN32
	__declspec(dllexport)
#endif
	int IsDoublePrecision();
}

#endif // _INTERFACE_H_
