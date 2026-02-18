#include <iostream>
#include <vector>

#include "../maestroquest/Interface.h"

double checkClose(double a, double b, double tol = 1e-6) {
	return std::abs(a - b) < tol;
}

int main()
{
	Initialize();

	auto handle = CreateSimulator(3);
	if (handle <= 0) {
		std::cerr << "Failed to create simulator" << std::endl;
		return 1;
	}

	void* sim = GetSimulator(handle);
	if (!sim) {
		std::cerr << "Failed to retrieve simulator" << std::endl;
		return 1;
	}

	int nrQubits = GetNumQubits(sim);
	if (nrQubits != 3) {
		std::cerr << "Number of qubits mismatch: " << nrQubits << std::endl;
		return 1;
	}

	for (int q = 0; q < nrQubits; ++q) {
		double prob0 = GetQubitProbability0(sim, q);
		if (!checkClose(prob0, 1.0)) {
			std::cerr << "Qubit " << q << " probability mismatch: " << prob0 << std::endl;
			return 1;
		}
	}

	double oprob = GetOutcomeProbability(sim, 0);
	if (!checkClose(oprob, 1.0)) {
		std::cerr << "Outcome probability mismatch: " << oprob << std::endl;
		return 1;
	}

	for (long long int outcome = 1; outcome < (1LL << nrQubits); ++outcome) {
		oprob = GetOutcomeProbability(sim, outcome);
		if (!checkClose(oprob, 0.0)) {
			std::cerr << "Outcome " << outcome << " probability mismatch: " << oprob << std::endl;
			return 1;
		}
	}

	double expVal = GetExpectationValue(sim, "ZZZ");
	if (!checkClose(expVal, 1.)) {
		std::cerr << "ZZZ expectation value mismatch: " << expVal << std::endl;
		return 1;
	}

	expVal = GetExpectationValue(sim, "XXX");
	if (!checkClose(expVal, 0.)) {
		std::cerr << "XXX expectation value mismatch: " << expVal << std::endl;
		return 1;
	}

	int meas = Measure(sim, 0);
	if (meas != 0) {
		std::cerr << "Measurement outcome mismatch: " << meas << std::endl;
		return 1;
	}

	std::vector<int> qubits = { 0, 1, 2 };
	long long int measOutcome = MeasureQubits(sim, qubits.data(), qubits.size());
	if (measOutcome != 0) {
		std::cerr << "Multi-qubit measurement outcome mismatch: " << measOutcome << std::endl;
		return 1;
	}

	DestroySimulator(handle);
	Finalize();

	std::cout << "All tests passed successfully!" << std::endl;

	return 0;
}
