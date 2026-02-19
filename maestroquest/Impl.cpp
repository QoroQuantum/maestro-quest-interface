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

#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplyRx(void* sim, int qubit, double angle)
{
	if (!sim) return;
	MaestroQuest::ApplyRx(sim, qubit, angle);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplyRy(void* sim, int qubit, double angle)
{
	if (!sim) return;
	MaestroQuest::ApplyRy(sim, qubit, angle);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplyRz(void* sim, int qubit, double angle)
{
	if (!sim) return;
	MaestroQuest::ApplyRz(sim, qubit, angle);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplyCS(void* sim, int control, int target)
{
	if (!sim) return;
	MaestroQuest::ApplyCS(sim, control, target);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplyCT(void* sim, int control, int target)
{
	if (!sim) return;
	MaestroQuest::ApplyCT(sim, control, target);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplyCH(void* sim, int control, int target)
{
	if (!sim) return;
	MaestroQuest::ApplyCH(sim, control, target);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplySwap(void* sim, int qubit1, int qubit2)
{
	if (!sim) return;
	MaestroQuest::ApplySwap(sim, qubit1, qubit2);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplyCX(void* sim, int control, int target)
{
	if (!sim) return;
	MaestroQuest::ApplyCX(sim, control, target);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplyCY(void* sim, int control, int target)
{
	if (!sim) return;
	MaestroQuest::ApplyCY(sim, control, target);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplyCZ(void* sim, int control, int target)
{
	if (!sim) return;
	MaestroQuest::ApplyCZ(sim, control, target);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplyCRx(void* sim, int control, int target, double angle)
{
	if (!sim) return;
	MaestroQuest::ApplyCRx(sim, control, target, angle);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplyCRy(void* sim, int control, int target, double angle)
{
	if (!sim) return;
	MaestroQuest::ApplyCRy(sim, control, target, angle);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplyCRz(void* sim, int control, int target, double angle)
{
	if (!sim) return;
	MaestroQuest::ApplyCRz(sim, control, target, angle);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplyCSwap(void* sim, int control, int qubit1, int qubit2)
{
	if (!sim) return;
	MaestroQuest::ApplyCSwap(sim, control, qubit1, qubit2);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplyCCX(void* sim, int control1, int control2, int target)
{
	if (!sim) return;
	MaestroQuest::ApplyCCX(sim, control1, control2, target);
}


#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplySdg(void* sim, int qubit)
{
	if (!sim) return;
	MaestroQuest* msim = static_cast<MaestroQuest*>(sim);
	msim->ApplySdg(sim, qubit);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplyTdg(void* sim, int qubit)
{
	if (!sim) return;
	MaestroQuest* msim = static_cast<MaestroQuest*>(sim);
	msim->ApplyTdg(msim, qubit);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplySx(void* sim, int qubit)
{
	if (!sim) return;
	MaestroQuest* msim = static_cast<MaestroQuest*>(sim);
	msim->ApplySx(sim, qubit);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplySxDg(void* sim, int qubit)
{
	if (!sim) return;
	MaestroQuest* msim = static_cast<MaestroQuest*>(sim);
	msim->ApplySxDg(sim, qubit);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplyK(void* sim, int qubit)
{
	if (!sim) return;
	MaestroQuest* msim = static_cast<MaestroQuest*>(sim);
	msim->ApplyK(sim, qubit);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplyU(void* sim, int qubit, double theta, double phi, double lambda, double gamma)
{
	if (!sim) return;
	MaestroQuest* msim = static_cast<MaestroQuest*>(sim);
	msim->ApplyU(sim, qubit, theta, phi, lambda, gamma);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplyCU(void* sim, int control, int target, double theta, double phi, double lambda, double gamma)
{
	if (!sim) return;
	MaestroQuest* msim = static_cast<MaestroQuest*>(sim);
	msim->ApplyCU(sim, control, target, theta, phi, lambda, gamma);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplyCP(void* sim, int control, int target, double angle)
{
	if (!sim) return;
	MaestroQuest* msim = static_cast<MaestroQuest*>(sim);
	msim->ApplyCP(sim, control, target, angle);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplyCSx(void* sim, int control, int target)
{
	if (!sim) return;
	MaestroQuest* msim = static_cast<MaestroQuest*>(sim);
	msim->ApplyCSx(sim, control, target);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplyCSxDg(void* sim, int control, int target)
{
	if (!sim) return;
	MaestroQuest* msim = static_cast<MaestroQuest*>(sim);
	msim->ApplyCSxDg(sim, control, target);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
int GetAmplitudes(void* sim, void* buffer, size_t bufSize)
{
	if (!sim || !buffer || bufSize == 0) return 0;
	MaestroQuest::GetAmplitudes(sim, buffer, bufSize);
}

