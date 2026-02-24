/**
 * Maestro + QuEST Integration — C++ Example
 * ==========================================
 *
 * Demonstrates using the QuEST simulator backend through Maestro's
 * C interface. This is the C++ equivalent of quest_example.py.
 *
 * Covers:
 *   1. Initializing the QuEST library via Maestro's Factory
 *   2. Running a Bell-state circuit via QASM with SimpleExecute
 *   3. Estimating Pauli expectation values with SimpleEstimate
 *   4. GHZ State (4 qubits)
 *
 * Build:
 *   This example must be compiled against the Maestro library (libmaestro).
 *
 *   cmake -B build -DCMAKE_BUILD_TYPE=Release \
 *         -DMAESTRO_DIR=/path/to/maestro
 *   cmake --build build --target quest_example
 *
 * Run (set library paths so both libmaestro and libmaestroquest are found):
 *
 *   # macOS
 *   export DYLD_LIBRARY_PATH=/path/to/maestro/build:/path/to/maestro-quest-interface/build
 *   ./build/quest_example
 *
 *   # Linux
 *   export LD_LIBRARY_PATH=/path/to/maestro/build:/path/to/maestro-quest-interface/build
 *   ./build/quest_example
 */

// Only include Maestro's C interface — no C++ headers needed.
#include "maestrolib/Interface.h"

#include <cstdlib>
#include <iostream>

// Forward-declare the QuEST init function from libmaestro.
// This avoids pulling in the full Factory.h header chain
// (which requires Eigen, Boost, QCSim, Aer, etc.).
namespace Simulators {
class SimulatorsFactory {
 public:
  static bool InitQuestLibrary();
};
}  // namespace Simulators

// SimulatorType::kQuestSim   = 3
// SimulationType::kStatevector = 0
static constexpr int kQuestSim    = 3;
static constexpr int kStatevector = 0;

static const char *SEP =
    "------------------------------------------------------------";

// ----------------------------------------------------------------
// 1. Bell State via QASM (SimpleExecute)
// ----------------------------------------------------------------
void bell_state_qasm() {
    std::cout << SEP << "\n"
              << "1. Bell State via QASM (SimpleExecute)\n"
              << SEP << "\n";

    unsigned long int handle = CreateSimpleSimulator(2);
    RemoveAllOptimizationSimulatorsAndAdd(handle, kQuestSim, kStatevector);

    const char *qasm =
        "OPENQASM 2.0;\n"
        "include \"qelib1.inc\";\n"
        "qreg q[2];\n"
        "creg c[2];\n"
        "h q[0];\n"
        "cx q[0], q[1];\n"
        "measure q -> c;\n";

    const char *config = "{\"shots\": 1000}";

    char *result = SimpleExecute(handle, qasm, config);
    if (result) {
        std::cout << "   Result: " << result << "\n";
        FreeResult(result);
    } else {
        std::cerr << "   SimpleExecute returned null\n";
    }

    std::cout << "   ✓ Bell state executed\n\n";
    DestroySimpleSimulator(handle);
}

// ----------------------------------------------------------------
// 2. Expectation Values via QASM (SimpleEstimate)
// ----------------------------------------------------------------
void expectation_values_qasm() {
    std::cout << SEP << "\n"
              << "2. Expectation Values via QASM (SimpleEstimate)\n"
              << SEP << "\n";

    unsigned long int handle = CreateSimpleSimulator(2);
    RemoveAllOptimizationSimulatorsAndAdd(handle, kQuestSim, kStatevector);

    const char *qasm =
        "OPENQASM 2.0;\n"
        "include \"qelib1.inc\";\n"
        "qreg q[2];\n"
        "h q[0];\n"
        "cx q[0], q[1];\n";

    const char *observables = "ZZ;XX;YY";
    const char *config = "{}";

    char *result = SimpleEstimate(handle, qasm, observables, config);
    if (result) {
        std::cout << "   Result: " << result << "\n";
        std::cout << "   Expected: ZZ=+1, XX=+1, YY=-1\n";
        FreeResult(result);
    } else {
        std::cerr << "   SimpleEstimate returned null\n";
    }

    std::cout << "   ✓ Expectation values computed\n\n";
    DestroySimpleSimulator(handle);
}

// ----------------------------------------------------------------
// 3. GHZ State (4 qubits)
// ----------------------------------------------------------------
void ghz_state_qasm() {
    std::cout << SEP << "\n"
              << "3. GHZ State (4 qubits)\n"
              << SEP << "\n";

    unsigned long int handle = CreateSimpleSimulator(4);
    RemoveAllOptimizationSimulatorsAndAdd(handle, kQuestSim, kStatevector);

    const char *qasm =
        "OPENQASM 2.0;\n"
        "include \"qelib1.inc\";\n"
        "qreg q[4];\n"
        "creg c[4];\n"
        "h q[0];\n"
        "cx q[0], q[1];\n"
        "cx q[0], q[2];\n"
        "cx q[0], q[3];\n"
        "measure q -> c;\n";

    const char *config = "{\"shots\": 2000}";

    char *result = SimpleExecute(handle, qasm, config);
    if (result) {
        std::cout << "   Result: " << result << "\n";
        FreeResult(result);
    } else {
        std::cerr << "   SimpleExecute returned null\n";
    }

    std::cout << "   ✓ GHZ state executed\n\n";
    DestroySimpleSimulator(handle);
}

// ----------------------------------------------------------------
// Main
// ----------------------------------------------------------------
int main() {
    std::cout << "\n"
              << "============================================================\n"
              << "  Maestro + QuEST Integration — C++ Example\n"
              << "============================================================\n\n";

    // Initialize the Maestro engine
    void *maestro = GetMaestroObject();
    if (!maestro) {
        std::cerr << "Failed to initialize Maestro.\n";
        return 1;
    }

    // Initialize the QuEST library (loads libmaestroquest dynamically)
    bool questOk = Simulators::SimulatorsFactory::InitQuestLibrary();
    std::cout << "QuEST library initialized: " << (questOk ? "yes" : "no")
              << "\n\n";
    if (!questOk) {
        std::cerr << "Failed to load QuEST library. Make sure libmaestroquest "
                     "is on the library search path.\n";
        return 1;
    }

    bell_state_qasm();
    expectation_values_qasm();
    ghz_state_qasm();

    std::cout << SEP << "\n"
              << "All examples completed successfully!\n"
              << SEP << "\n";

    return 0;
}
