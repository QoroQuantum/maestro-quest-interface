#pragma once

#ifndef _MAESTROQUEST_H_
#define _MAESTROQUEST_H_

#include <limits>
#include <mutex>
#include <unordered_map>
#include <numeric>
#include <memory>
#define _USE_MATH_DEFINES
#include <math.h>

#include "quest/include/quest.h"

class MaestroQuest
{
public:
	MaestroQuest() = default;
	// Disable copy semantics
	MaestroQuest(const MaestroQuest&) = delete;
	MaestroQuest& operator=(const MaestroQuest&) = delete;

	// Enable move semantics
	MaestroQuest(MaestroQuest&&) = default;
	MaestroQuest& operator=(MaestroQuest&&) = default;

	unsigned long int CreateSimulator(int nrQubits) {
		if (nrQubits <= 0) return 0;

		auto quregPtr = std::make_unique<Qureg>();
		*quregPtr = createForcedQureg(nrQubits);

		std::lock_guard<std::mutex> lock(simulatorsMutex);

		// Find a free handle, skipping any that are still in use
		do {
			if (curHandle == std::numeric_limits<unsigned long int>::max())
				curHandle = 0;
			++curHandle;
		} while (simulators.count(curHandle));

		const unsigned long int handle = curHandle;
		simulators[handle] = std::move(quregPtr);

		return handle;
	}

	void DestroySimulator(unsigned long int simHandle) {
		std::lock_guard<std::mutex> lock(simulatorsMutex);

		auto it = simulators.find(simHandle);
		if (it != simulators.end()) {
			destroyQureg(*(it->second.get()));
			simulators.erase(it);
		}
	}

	/// Destroy all remaining simulators. Called during Finalize to prevent leaks.
	void DestroyAll() {
		std::lock_guard<std::mutex> lock(simulatorsMutex);
		for (auto& [handle, qureg] : simulators) {
			destroyQureg(*qureg);
		}
		simulators.clear();
	}

	unsigned long int CloneSimulator(void* sim) {
		if (!sim) return 0;

		Qureg* qureg = static_cast<Qureg*>(sim);
		auto quregPtr = std::make_unique<Qureg>();
		*quregPtr = createCloneQureg(*qureg);

		std::lock_guard<std::mutex> lock(simulatorsMutex);

		// Find a free handle, skipping any that are still in use
		do {
			if (curHandle == std::numeric_limits<unsigned long int>::max())
				curHandle = 0;
			++curHandle;
		} while (simulators.count(curHandle));

		const unsigned long int newHandle = curHandle;
		simulators[newHandle] = std::move(quregPtr);
		return newHandle;
	}

	Qureg* GetSimulator(unsigned long int simHandle) {
		std::lock_guard<std::mutex> lock(simulatorsMutex);
		auto it = simulators.find(simHandle);
		if (it == simulators.end())
			throw std::runtime_error("Simulator handle not found");

		return it->second.get();
	}

	static int GetNumQubits(void* sim) {
		if (!sim) return 0;

		Qureg* qureg = static_cast<Qureg*>(sim);
		return qureg->numQubits;
	}

	static double GetQubitProbability0(void* sim, int qubit) {
		if (!sim) return 0.0;
		Qureg* qureg = static_cast<Qureg*>(sim);
		return calcProbOfQubitOutcome(*qureg, qubit, 0);
	}

	static double GetQubitProbability1(void* sim, int qubit) {
		if (!sim) return 0.0;
		Qureg* qureg = static_cast<Qureg*>(sim);
		return calcProbOfQubitOutcome(*qureg, qubit, 1);
	}

	static double GetOutcomeProbability(void* sim, long long int outcome) {
		if (!sim) return 0.0;
		Qureg* qureg = static_cast<Qureg*>(sim);
		return calcProbOfBasisState(*qureg, outcome);
	}

	static double GetExpectationValue(void* sim, const char* pauliStr) {
		if (!sim || !pauliStr) return 0.0;
		Qureg* qureg = static_cast<Qureg*>(sim);
		std::string pauli(pauliStr);
		if (pauli.empty() || static_cast<int>(pauli.length()) > qureg->numQubits)
			return 0.0;

		std::vector<int> qubits(pauli.length());
		std::iota(qubits.begin(), qubits.end(), 0); 

		PauliStr p = getPauliStr(pauli, qubits);
		return calcExpecPauliStr(*qureg, p);
	}

	static int Measure(void* sim, int qubit) {
		if (!sim) return 0;
		Qureg* qureg = static_cast<Qureg*>(sim);
		return applyQubitMeasurement(*qureg, qubit);
	}

	static long long int MeasureQubits(void* sim, int* qubits, int numQubits) {
		if (!sim) return 0;
		Qureg* qureg = static_cast<Qureg*>(sim);
		return applyMultiQubitMeasurement(*qureg, qubits, numQubits);
	}

	static void ApplyP(void* sim, int qubit, double angle) {
		if (!sim) return;
		Qureg* qureg = static_cast<Qureg*>(sim);
		applyPhaseShift(*qureg, qubit, angle);
	}

	static void ApplyX(void* sim, int qubit) {
		if (!sim) return;
		Qureg* qureg = static_cast<Qureg*>(sim);
		applyPauliX(*qureg, qubit);
	}

	static void ApplyY(void* sim, int qubit) {
		if (!sim) return;
		Qureg* qureg = static_cast<Qureg*>(sim);
		applyPauliY(*qureg, qubit);
	}

	static void ApplyZ(void* sim, int qubit) {
		if (!sim) return;
		Qureg* qureg = static_cast<Qureg*>(sim);
		applyPauliZ(*qureg, qubit);
	}

	static void ApplyH(void* sim, int qubit) {
		if (!sim) return;
		Qureg* qureg = static_cast<Qureg*>(sim);
		applyHadamard(*qureg, qubit);
	}

	static void ApplyS(void* sim, int qubit) {
		if (!sim) return;
		Qureg* qureg = static_cast<Qureg*>(sim);
		applyS(*qureg, qubit);
	}

	static void ApplyT(void* sim, int qubit) {
		if (!sim) return;
		Qureg* qureg = static_cast<Qureg*>(sim);
		applyT(*qureg, qubit);
	}

	static void ApplyRx(void* sim, int qubit, double angle) {
		if (!sim) return;
		Qureg* qureg = static_cast<Qureg*>(sim);
		applyRotateX(*qureg, qubit, angle);
	}

	static void ApplyRy(void* sim, int qubit, double angle) {
		if (!sim) return;
		Qureg* qureg = static_cast<Qureg*>(sim);
		applyRotateY(*qureg, qubit, angle);
	}

	static void ApplyRz(void* sim, int qubit, double angle) {
		if (!sim) return;
		Qureg* qureg = static_cast<Qureg*>(sim);
		applyRotateZ(*qureg, qubit, angle);
	}

	static void ApplyCS(void* sim, int control, int target) {
		if (!sim) return;
		Qureg* qureg = static_cast<Qureg*>(sim);
		applyControlledS(*qureg, control, target);
	}

	static void ApplyCT(void* sim, int control, int target) {
		if (!sim) return;
		Qureg* qureg = static_cast<Qureg*>(sim);
		applyControlledT(*qureg, control, target);
	}

	static void ApplyCH(void* sim, int control, int target) {
		if (!sim) return;
		Qureg* qureg = static_cast<Qureg*>(sim);
		applyControlledHadamard(*qureg, control, target);
	}

	static void ApplySwap(void* sim, int qubit1, int qubit2) {
		if (!sim) return;
		Qureg* qureg = static_cast<Qureg*>(sim);
		applySwap(*qureg, qubit1, qubit2);
	}

	static void ApplyCX(void* sim, int control, int target) {
		if (!sim) return;
		Qureg* qureg = static_cast<Qureg*>(sim);
		applyControlledPauliX(*qureg, control, target);
	}

	static void ApplyCY(void* sim, int control, int target) {
		if (!sim) return;
		Qureg* qureg = static_cast<Qureg*>(sim);
		applyControlledPauliY(*qureg, control, target);
	}

	static void ApplyCZ(void* sim, int control, int target) {
		if (!sim) return;
		Qureg* qureg = static_cast<Qureg*>(sim);
		applyControlledPauliZ(*qureg, control, target);
	}

	static void ApplyCRx(void* sim, int control, int target, double angle) {
		if (!sim) return;
		Qureg* qureg = static_cast<Qureg*>(sim);
		applyControlledRotateX(*qureg, control, target, angle);
	}

	static void ApplyCRy(void* sim, int control, int target, double angle) {
		if (!sim) return;
		Qureg* qureg = static_cast<Qureg*>(sim);
		applyControlledRotateY(*qureg, control, target, angle);
	}

	static void ApplyCRz(void* sim, int control, int target, double angle) {
		if (!sim) return;
		Qureg* qureg = static_cast<Qureg*>(sim);
		applyControlledRotateZ(*qureg, control, target, angle);
	}

	static void ApplyCSwap(void* sim, int control, int qubit1, int qubit2) {
		if (!sim) return;
		Qureg* qureg = static_cast<Qureg*>(sim);
		applyControlledSwap(*qureg, control, qubit1, qubit2);
	}

	static void ApplyCCX(void* sim, int control1, int control2, int target) {
		if (!sim) return;
		Qureg* qureg = static_cast<Qureg*>(sim);
		applyMultiControlledPauliX(*qureg, {control1, control2}, target);
	}

	static void ApplySdg(void* sim, int qubit) {
		if (!sim) return;
		Qureg* qureg = static_cast<Qureg*>(sim);
		applyDiagMatr1(*qureg, qubit, sdgMat());
	}

	static void ApplyTdg(void* sim, int qubit) {
		if (!sim) return;
		Qureg* qureg = static_cast<Qureg*>(sim);
		applyDiagMatr1(*qureg, qubit, tdgMat());
	}

	static void ApplySx(void* sim, int qubit) {
		if (!sim) return;
		Qureg* qureg = static_cast<Qureg*>(sim);
		applyCompMatr1(*qureg, qubit, sxMat());
	}

	static void ApplySxDg(void* sim, int qubit) {
		if (!sim) return;
		Qureg* qureg = static_cast<Qureg*>(sim);
		applyCompMatr1(*qureg, qubit, sxdgMat());
	}

	static void ApplyK(void* sim, int qubit) {
		if (!sim) return;
		Qureg* qureg = static_cast<Qureg*>(sim);
		applyCompMatr1(*qureg, qubit, kMat());
	}

	static void ApplyU(void* sim, int qubit, double theta, double phi, double lambda, double gamma) {
		if (!sim) return;
		Qureg* qureg = static_cast<Qureg*>(sim);

		const FLOAT_TYPE t2 = theta * 0.5;
		const std::complex<FLOAT_TYPE> a = std::polar(1., gamma) * std::complex<FLOAT_TYPE>(cos(t2), 0.);
		const std::complex<FLOAT_TYPE> b = -std::polar(1., gamma + lambda) * sin(t2);
		const std::complex<FLOAT_TYPE> c = std::polar(1., gamma + phi) * sin(t2);
		const std::complex<FLOAT_TYPE> d = std::polar(1., gamma + phi + lambda) * cos(t2);

		CompMatr1 Umat = getInlineCompMatr1({ 
			{a, b},
			{c, d}
			});
		applyCompMatr1(*qureg, qubit, Umat);
	}

	static void ApplyCP(void* sim, int control, int target, double angle) {
		if (!sim) return;
		Qureg* qureg = static_cast<Qureg*>(sim);

		DiagMatr1 pmat = getInlineDiagMatr1({ {1.,0.}, std::polar(1., angle) });

		applyControlledDiagMatr1(*qureg, control, target, pmat);
	}

	static void ApplyCSx(void* sim, int control, int target) {
		if (!sim) return;
		Qureg* qureg = static_cast<Qureg*>(sim);
		applyControlledCompMatr1(*qureg, control, target, sxMat());
	}

	static void ApplyCSxDg(void* sim, int control, int target) {
		if (!sim) return;
		Qureg* qureg = static_cast<Qureg*>(sim);
		applyControlledCompMatr1(*qureg, control, target, sxdgMat());
	}

	static void ApplyCU(void* sim, int control, int target, double theta, double phi, double lambda, double gamma) {
		if (!sim) return;
		Qureg* qureg = static_cast<Qureg*>(sim);
		const FLOAT_TYPE t2 = theta * 0.5;
		const std::complex<FLOAT_TYPE> a = std::polar(1., gamma) * std::complex<FLOAT_TYPE>(cos(t2), 0.);
		const std::complex<FLOAT_TYPE> b = -std::polar(1., gamma + lambda) * sin(t2);
		const std::complex<FLOAT_TYPE> c = std::polar(1., gamma + phi) * sin(t2);
		const std::complex<FLOAT_TYPE> d = std::polar(1., gamma + phi + lambda) * cos(t2);

		CompMatr1 Umat = getInlineCompMatr1({
			{a, b},
			{c, d}
			});
		applyControlledCompMatr1(*qureg, control, target, Umat);
	}

	static int GetAmplitudes(void* sim, void* buffer, unsigned long long int bufSize) {
		if (!sim || !buffer || bufSize == 0) return 0;
		Qureg* qureg = static_cast<Qureg*>(sim);
		const size_t numAmps = qureg->numAmps;
		if (bufSize < numAmps * sizeof(qcomp)) return 0; // buffer too small

		getQuregAmps(static_cast<qcomp*>(buffer), *qureg, 0, numAmps);
		return 1;
	}

	static int GetAmplitude(void* sim, long long int index, void* outAmp, unsigned long long int bufSize) {
		if (!sim || !outAmp) return 0;
		Qureg* qureg = static_cast<Qureg*>(sim);
		const size_t numAmps = qureg->numAmps;
		if (index < 0 || static_cast<size_t>(index) >= numAmps) return 0; // index out of bounds
		else if (bufSize < sizeof(qcomp)) return 0; // buffer too small
		qcomp val = getQuregAmp(*qureg, index);
		
		*static_cast<qcomp*>(outAmp) = val;

		return 1;
	}

	static int IsDoublePrecision() {
		return sizeof(FLOAT_TYPE) == sizeof(double) ? 1 : 0;
	}

private:
	unsigned long int curHandle = 0;
	std::mutex simulatorsMutex;
	std::unordered_map<unsigned long int, std::unique_ptr<Qureg>> simulators;

	// Lazy-initialized gate matrices — deferred to first use so they are
	// constructed after initQuESTEnv() has been called (safe for GPU builds).
	static const DiagMatr1& sdgMat() {
		static DiagMatr1 m = getInlineDiagMatr1({ std::complex<FLOAT_TYPE>(1.,0.), std::complex<FLOAT_TYPE>(0., -1.)});
		return m;
	}
	static const DiagMatr1& tdgMat() {
		static DiagMatr1 m = getInlineDiagMatr1({ std::complex<FLOAT_TYPE>(1.,0.), std::polar<FLOAT_TYPE>(1., -M_PI / 4.) });
		return m;
	}
	static const CompMatr1& sxMat() {
		static CompMatr1 m = getInlineCompMatr1({ {{0.5,0.5},{0.5,-0.5}}, {{0.5,-0.5},{0.5,0.5}} });
		return m;
	}
	static const CompMatr1& sxdgMat() {
		static CompMatr1 m = getInlineCompMatr1({ {{0.5,-0.5},{0.5,0.5}}, {{0.5,0.5},{0.5,-0.5}} });
		return m;
	}
	static const CompMatr1& kMat() {
		static CompMatr1 m = getInlineCompMatr1({ {{1. / sqrt(2.),0.},{0.,-1. / sqrt(2.)}}, {{0.,1. / sqrt(2.)},{-1. / sqrt(2.),0.}} });
		return m;
	}
};

#endif // _MAESTROQUEST_H_