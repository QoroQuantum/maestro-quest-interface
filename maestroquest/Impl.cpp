#include <atomic>

#include "Interface.h"
#include "MaestroQuest.h"



static std::atomic_bool isInitialized{ false };
static std::unique_ptr<MaestroQuest> maestroInstance;

#ifdef _WIN32
__declspec(dllexport)
#endif
void Initialize()
{
	if (!isInitialized.exchange(true)) {
		initQuESTEnv();
		maestroInstance = std::make_unique<MaestroQuest>();
	}
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void Finalize()
{
	if (isInitialized.exchange(false)) {
		finalizeQuESTEnv();
		maestroInstance.reset();
	}
}

#ifdef _WIN32
__declspec(dllexport)
#endif
unsigned long int CreateSimulator(int nrQubits)
{
	if (!isInitialized) return 0;

	return maestroInstance->CreateSimulator(nrQubits);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void DestroySimulator(unsigned long int simHandle)
{
	if (!isInitialized) return;
	maestroInstance->DestroySimulator(simHandle);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
unsigned long int CloneSimulator(void* sim)
{
	if (!isInitialized) return 0;
	return maestroInstance->CloneSimulator(sim);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void* GetSimulator(unsigned long int simHandle)
{
	if (!isInitialized) return nullptr;
	return maestroInstance->GetSimulator(simHandle);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
int GetNumQubits(void* sim)
{
	if (!sim) return 0;
	return MaestroQuest::GetNumQubits(sim);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
double GetQubitProbability0(void* sim, int qubit)
{
	if (!sim) return 0;
	return MaestroQuest::GetQubitProbability0(sim, qubit);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
double GetQubitProbability1(void* sim, int qubit)
{
	if (!sim) return 0;
	return MaestroQuest::GetQubitProbability1(sim, qubit);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
double GetOutcomeProbability(void* sim, long long int outcome)
{
	if (!sim) return 0;
	return MaestroQuest::GetOutcomeProbability(sim, outcome);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
double GetExpectationValue(void* sim, const char* pauliStr)
{
	if (!sim) return 0;
	return MaestroQuest::GetExpectationValue(sim, pauliStr);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
int Measure(void* sim, int qubit)
{
	if (!sim) return 0;
	return MaestroQuest::Measure(sim, qubit);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
long long int MeasureQubits(void* sim, int* qubits, int numQubits)
{
	if (!sim) return 0;
	return MaestroQuest::MeasureQubits(sim, qubits, numQubits);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplyP(void* sim, int qubit, double angle)
{
	if (!sim) return;
	MaestroQuest::ApplyP(sim, qubit, angle);
}


#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplyX(void* sim, int qubit)
{
	if (!sim) return;
	MaestroQuest::ApplyX(sim, qubit);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplyY(void* sim, int qubit)
{
	if (!sim) return;
	MaestroQuest::ApplyY(sim, qubit);
}


#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplyZ(void* sim, int qubit)
{
	if (!sim) return;
	MaestroQuest::ApplyZ(sim, qubit);
}


#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplyH(void* sim, int qubit)
{
	if (!sim) return;
	MaestroQuest::ApplyH(sim, qubit);
}


#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplyS(void* sim, int qubit)
{
	if (!sim) return;
	MaestroQuest::ApplyS(sim, qubit);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplyT(void* sim, int qubit)
{
	if (!sim) return;
	MaestroQuest::ApplyT(sim, qubit);
}

