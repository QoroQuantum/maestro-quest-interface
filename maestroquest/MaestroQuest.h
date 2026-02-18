#pragma once

#ifndef _MAESTROQUEST_H_
#define _MAESTROQUEST_H_

#include <mutex>
#include <unordered_map>
#include <numeric>
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

		if (curHandle == std::numeric_limits<unsigned long int>::max()) {
			// Handle overflow, reset to 0
			curHandle = 0;
		}
		const unsigned long int handle = ++curHandle;
		simulators[handle] = std::move(quregPtr);

		return handle;
	}

	void DestroySimulator(unsigned long int simHandle) {
		std::lock_guard<std::mutex> lock(simulatorsMutex);

		auto it = simulators.find(simHandle);
		if (it != simulators.end())
			destroyQureg(*(it->second.get()));

		simulators.erase(it);
	}

	unsigned long int CloneSimulator(void* sim) {
		Qureg* qureg = static_cast<Qureg*>(sim);
		auto quregPtr = std::make_unique<Qureg>();
		*quregPtr = createCloneQureg(*qureg);

		std::lock_guard<std::mutex> lock(simulatorsMutex);
		const unsigned long int newHandle = ++curHandle;
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


private:
	unsigned long int curHandle = 0;
	std::mutex simulatorsMutex;
	std::unordered_map<unsigned long int, std::unique_ptr<Qureg>> simulators;
};

#endif // _MAESTROQUEST_H_