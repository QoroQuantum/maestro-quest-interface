#include <atomic>
#include <shared_mutex>

#ifdef MAESTROQUEST_MPI
#include <mpi.h>
#endif

#include "Interface.h"
#include "MaestroQuest.h"



static std::atomic_bool isInitialized{ false };
static std::unique_ptr<MaestroQuest> maestroInstance;
static std::shared_mutex g_instanceMutex;
static bool weOwnQuESTEnv = false;

#ifdef _WIN32
__declspec(dllexport)
#endif
void Initialize()
{
	std::unique_lock<std::shared_mutex> lk(g_instanceMutex);
	if (!isInitialized.exchange(true)) {
#ifdef MAESTROQUEST_MPI
		// Detect whether MPI is already initialized by the host application.
		// If so, we must NOT call finalizeQuESTEnv() later, as that would
		// tear down the host's MPI environment.
		int mpiAlreadyInit = 0;
		MPI_Initialized(&mpiAlreadyInit);
		weOwnQuESTEnv = !mpiAlreadyInit;
#else
		weOwnQuESTEnv = true;
#endif
		initQuESTEnv();
		maestroInstance = std::make_unique<MaestroQuest>();
	}
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void Finalize()
{
	std::unique_lock<std::shared_mutex> lk(g_instanceMutex);
	if (isInitialized.exchange(false)) {
		if (maestroInstance) {
			maestroInstance->DestroyAll();
			maestroInstance.reset();
		}
		// Only tear down the QuEST/MPI environment if we were the ones
		// who initialized it. Otherwise the host application owns MPI.
		if (weOwnQuESTEnv) {
			finalizeQuESTEnv();
		}
	}
}

#ifdef _WIN32
__declspec(dllexport)
#endif
unsigned long int CreateSimulator(int nrQubits)
{
	std::shared_lock<std::shared_mutex> lk(g_instanceMutex);
	if (!isInitialized || !maestroInstance) return 0;

	return maestroInstance->CreateSimulator(nrQubits);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void DestroySimulator(unsigned long int simHandle)
{
	std::shared_lock<std::shared_mutex> lk(g_instanceMutex);
	if (!isInitialized || !maestroInstance) return;
	maestroInstance->DestroySimulator(simHandle);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
unsigned long int CloneSimulator(void* sim)
{
	std::shared_lock<std::shared_mutex> lk(g_instanceMutex);
	if (!isInitialized || !maestroInstance) return 0;
	return maestroInstance->CloneSimulator(sim);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void* GetSimulator(unsigned long int simHandle)
{
	std::shared_lock<std::shared_mutex> lk(g_instanceMutex);
	if (!isInitialized || !maestroInstance) return nullptr;
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
	MaestroQuest::ApplySdg(sim, qubit);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplyTdg(void* sim, int qubit)
{
	if (!sim) return;
	MaestroQuest::ApplyTdg(sim, qubit);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplySx(void* sim, int qubit)
{
	if (!sim) return;
	MaestroQuest::ApplySx(sim, qubit);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplySxDg(void* sim, int qubit)
{
	if (!sim) return;
	MaestroQuest::ApplySxDg(sim, qubit);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplyK(void* sim, int qubit)
{
	if (!sim) return;
	MaestroQuest::ApplyK(sim, qubit);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplyU(void* sim, int qubit, double theta, double phi, double lambda, double gamma)
{
	if (!sim) return;
	MaestroQuest::ApplyU(sim, qubit, theta, phi, lambda, gamma);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplyCU(void* sim, int control, int target, double theta, double phi, double lambda, double gamma)
{
	if (!sim) return;
	MaestroQuest::ApplyCU(sim, control, target, theta, phi, lambda, gamma);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplyCP(void* sim, int control, int target, double angle)
{
	if (!sim) return;
	MaestroQuest::ApplyCP(sim, control, target, angle);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplyCSx(void* sim, int control, int target)
{
	if (!sim) return;
	MaestroQuest::ApplyCSx(sim, control, target);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
void ApplyCSxDg(void* sim, int control, int target)
{
	if (!sim) return;
	MaestroQuest::ApplyCSxDg(sim, control, target);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
int GetAmplitudes(void* sim, void* buffer, unsigned long long int bufSize)
{
	if (!sim || !buffer || bufSize == 0) return 0;
	return MaestroQuest::GetAmplitudes(sim, buffer, bufSize);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
int GetAmplitude(void* sim, long long int index, void* outAmp, unsigned long long int bufSize)
{
	if (!sim || !outAmp || bufSize == 0) return 0;
	return MaestroQuest::GetAmplitude(sim, index, outAmp, bufSize);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
int IsDoublePrecision()
{
	return MaestroQuest::IsDoublePrecision();
}

