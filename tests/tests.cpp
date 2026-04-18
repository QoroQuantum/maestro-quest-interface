#include <iostream>
#include <vector>
#include <cmath>
#include <complex>
#include <string>

#include "../maestroquest/Interface.h"

// Macro helpers
#define REQUIRE_CLOSE(a, b) if(std::abs((a) - (b)) > 1e-5) { std::cerr << "Test failed at line " << __LINE__ << ": " << (a) << " != " << (b) << std::endl; return false; }
#define REQUIRE(c) if(!(c)) { std::cerr << "Test failed at line " << __LINE__ << ": " << #c << std::endl; return false; }

bool testLifecycleAndQueries() {
    REQUIRE(IsDoublePrecision() >= 0);

    auto handle = CreateSimulator(3);
    REQUIRE(handle > 0);
    
    void* sim = GetSimulator(handle);
    REQUIRE(sim != nullptr);

    REQUIRE(GetNumQubits(sim) == 3);

    auto handle2 = CloneSimulator(sim);
    REQUIRE(handle2 > 0);
    void* sim2 = GetSimulator(handle2);
    REQUIRE(sim2 != nullptr);
    REQUIRE(GetNumQubits(sim2) == 3);

    DestroySimulator(handle2);

    REQUIRE_CLOSE(GetQubitProbability0(sim, 0), 1.0);
    REQUIRE_CLOSE(GetQubitProbability1(sim, 0), 0.0);
    REQUIRE_CLOSE(GetOutcomeProbability(sim, 0), 1.0);
    REQUIRE_CLOSE(GetOutcomeProbability(sim, 1), 0.0);

    REQUIRE_CLOSE(GetExpectationValue(sim, "ZZZ"), 1.0);
    REQUIRE_CLOSE(GetExpectationValue(sim, "XXX"), 0.0);

    // Call measurement
    int meas = Measure(sim, 0);
    REQUIRE(meas == 0);
    
    std::vector<int> q = {0, 1, 2};
    long long int measOutcome = MeasureQubits(sim, q.data(), q.size());
    REQUIRE(measOutcome == 0);

    DestroySimulator(handle);
    return true;
}

bool testSingleQubitGates() {
    auto handle = CreateSimulator(1);
    void* sim = GetSimulator(handle);

    ApplyX(sim, 0);
    REQUIRE_CLOSE(GetQubitProbability0(sim, 0), 0.0);
    REQUIRE_CLOSE(GetQubitProbability1(sim, 0), 1.0);

    ApplyY(sim, 0); 
    // X -> Y -> |0> but with a global phase, prob is 1.0 for 0
    REQUIRE_CLOSE(GetQubitProbability0(sim, 0), 1.0);

    ApplyH(sim, 0);
    REQUIRE_CLOSE(GetQubitProbability0(sim, 0), 0.5);
    
    // reset to 0
    ApplyH(sim, 0);
    REQUIRE_CLOSE(GetQubitProbability0(sim, 0), 1.0);

    // Phase shift gates (should not change Z measurement probabilities)
    ApplyZ(sim, 0);
    ApplyS(sim, 0);
    ApplyT(sim, 0);
    ApplySdg(sim, 0);
    ApplyTdg(sim, 0);
    ApplyP(sim, 0, 3.14159);
    REQUIRE_CLOSE(GetQubitProbability0(sim, 0), 1.0); 

    // Rotations
    ApplyRx(sim, 0, 3.14159265358979323846); // pi -> bit flip
    REQUIRE_CLOSE(GetQubitProbability0(sim, 0), 0.0);
    ApplyRx(sim, 0, -3.14159265358979323846); // revert
    REQUIRE_CLOSE(GetQubitProbability0(sim, 0), 1.0);

    ApplyRy(sim, 0, 3.14159265358979323846);
    REQUIRE_CLOSE(GetQubitProbability0(sim, 0), 0.0);
    ApplyRy(sim, 0, -3.14159265358979323846);
    REQUIRE_CLOSE(GetQubitProbability0(sim, 0), 1.0);

    ApplyRz(sim, 0, 3.14159265358979323846); // phase only
    REQUIRE_CLOSE(GetQubitProbability0(sim, 0), 1.0);

    // K, Sx, SxDg
    ApplyK(sim, 0);
    REQUIRE_CLOSE(GetQubitProbability0(sim, 0), 0.5);

    // reset to 0
    DestroySimulator(handle);
    handle = CreateSimulator(1);
    sim = GetSimulator(handle);

    ApplySx(sim, 0);
    REQUIRE_CLOSE(GetQubitProbability0(sim, 0), 0.5);
    ApplySxDg(sim, 0);

    // reset to 0
    DestroySimulator(handle);
    handle = CreateSimulator(1);
    sim = GetSimulator(handle);

    // U
    ApplyU(sim, 0, 3.14159265358979323846, 0.0, 0.0, 0.0); // effectively Rx(pi) mapping
    REQUIRE_CLOSE(GetQubitProbability0(sim, 0), 0.0);
    
    DestroySimulator(handle);
    return true;
}

bool testTwoQubitGates() {
    auto handle = CreateSimulator(2);
    void* sim = GetSimulator(handle);

    // Test CX
    ApplyX(sim, 0); // qubit 0 is |1>
    ApplyCX(sim, 0, 1);
    REQUIRE_CLOSE(GetQubitProbability1(sim, 1), 1.0);
    
    // reset
    ApplyCX(sim, 0, 1);
    ApplyX(sim, 0);
    REQUIRE_CLOSE(GetQubitProbability1(sim, 1), 0.0);

    // CZ, CY
    ApplyX(sim, 0);
    ApplyX(sim, 1);
    ApplyCZ(sim, 0, 1); 
    ApplyCY(sim, 0, 1); 
    REQUIRE_CLOSE(GetQubitProbability1(sim, 1), 0.0); // qubit 1 flipped to 0
    // state is now |10>
    REQUIRE_CLOSE(GetQubitProbability1(sim, 0), 1.0);

    // Resetting to |00>
    ApplyX(sim, 0);

    // Swap
    ApplyX(sim, 0);
    ApplySwap(sim, 0, 1);
    REQUIRE_CLOSE(GetQubitProbability1(sim, 0), 0.0);
    REQUIRE_CLOSE(GetQubitProbability1(sim, 1), 1.0);
    ApplySwap(sim, 0, 1);
    ApplyX(sim, 0);

    // Apply controlled versions, validating they just execute properly.
    ApplyX(sim, 0);
    ApplyCS(sim, 0, 1);
    ApplyCT(sim, 0, 1);
    ApplyCH(sim, 0, 1);
    ApplyCP(sim, 0, 1, 3.1415);
    ApplyCRx(sim, 0, 1, 3.1415);
    ApplyCRy(sim, 0, 1, 3.1415);
    ApplyCRz(sim, 0, 1, 3.1415);
    ApplyCSx(sim, 0, 1);
    ApplyCSxDg(sim, 0, 1);
    ApplyCU(sim, 0, 1, 0.1, 0.2, 0.3, 0.4);
    
    DestroySimulator(handle);
    return true;
}

bool testThreeQubitGates() {
    auto handle = CreateSimulator(3);
    void* sim = GetSimulator(handle);

    // CCX
    ApplyX(sim, 0);
    ApplyX(sim, 1);
    ApplyCCX(sim, 0, 1, 2);
    REQUIRE_CLOSE(GetQubitProbability1(sim, 2), 1.0);
    
    // reset
    ApplyCCX(sim, 0, 1, 2);
    ApplyX(sim, 0);
    ApplyX(sim, 1);

    // CSwap
    ApplyX(sim, 0);
    ApplyX(sim, 1); // state |110>
    ApplyCSwap(sim, 0, 1, 2); // swaps 1 and 2 because 0 is 1. state |101>
    REQUIRE_CLOSE(GetQubitProbability1(sim, 1), 0.0);
    REQUIRE_CLOSE(GetQubitProbability1(sim, 2), 1.0);

    DestroySimulator(handle);
    return true;
}

bool testAmplitudes() {
    auto handle = CreateSimulator(1);
    void* sim = GetSimulator(handle);
    ApplyX(sim, 0);

    // Precision independent test of amplitudes structs
    if (IsDoublePrecision()) {
        struct complex_double_t { double real; double imag; };
        complex_double_t buffer[2];
        int res = GetAmplitudes(sim, buffer, sizeof(buffer));
        REQUIRE(res == 1);
        REQUIRE_CLOSE(buffer[0].real, 0.0);
        REQUIRE_CLOSE(buffer[1].real, 1.0);

        complex_double_t amp1;
        res = GetAmplitude(sim, 1, &amp1, sizeof(amp1));
        REQUIRE(res == 1);
        REQUIRE_CLOSE(amp1.real, 1.0);
    } else {
        struct complex_float_t { float real; float imag; };
        complex_float_t buffer[2];
        int res = GetAmplitudes(sim, buffer, sizeof(buffer));
        REQUIRE(res == 1);
        REQUIRE_CLOSE(buffer[0].real, 0.0);
        REQUIRE_CLOSE(buffer[1].real, 1.0);

        complex_float_t amp1;
        res = GetAmplitude(sim, 1, &amp1, sizeof(amp1));
        REQUIRE(res == 1);
        REQUIRE_CLOSE(amp1.real, 1.0);
    }

    DestroySimulator(handle);
    return true;
}

bool testCloneSimulatorNull() {
    // CloneSimulator must return 0 for a null pointer (and not crash).
    auto handle = CloneSimulator(nullptr);
    REQUIRE(handle == 0);
    return true;
}

bool testExpectationValueEdgeCases() {
    auto handle = CreateSimulator(2);
    void* sim = GetSimulator(handle);

    // Empty Pauli string → 0.0
    REQUIRE_CLOSE(GetExpectationValue(sim, ""), 0.0);

    // Null Pauli string → 0.0
    REQUIRE_CLOSE(GetExpectationValue(sim, nullptr), 0.0);

    // Pauli string longer than qubit count → 0.0
    REQUIRE_CLOSE(GetExpectationValue(sim, "ZZZ"), 0.0);

    // Sanity: valid string still works
    REQUIRE_CLOSE(GetExpectationValue(sim, "ZZ"), 1.0);

    DestroySimulator(handle);
    return true;
}

int main()
{
    Initialize();

    if(!testLifecycleAndQueries()) { std::cerr << "testLifecycleAndQueries failed" << std::endl; return 1; }
    if(!testSingleQubitGates()) { std::cerr << "testSingleQubitGates failed" << std::endl; return 1; }
    if(!testTwoQubitGates()) { std::cerr << "testTwoQubitGates failed" << std::endl; return 1; }
    if(!testThreeQubitGates()) { std::cerr << "testThreeQubitGates failed" << std::endl; return 1; }
    if(!testAmplitudes()) { std::cerr << "testAmplitudes failed" << std::endl; return 1; }
    if(!testCloneSimulatorNull()) { std::cerr << "testCloneSimulatorNull failed" << std::endl; return 1; }
    if(!testExpectationValueEdgeCases()) { std::cerr << "testExpectationValueEdgeCases failed" << std::endl; return 1; }

    Finalize();

    std::cout << "All tests passed successfully!" << std::endl;

    return 0;
}
