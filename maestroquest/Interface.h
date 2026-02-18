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

}

#endif // _INTERFACE_H_
