# Maestro QuEST Interface

[![CMake on Ubuntu](https://github.com/QoroQuantum/maestro-quest-interface/actions/workflows/cmake-single-platform.yml/badge.svg)](https://github.com/QoroQuantum/maestro-quest-interface/actions/workflows/cmake-single-platform.yml)

A shared library that wraps the [QuEST](https://github.com/QuEST-Kit/QuEST) quantum circuit simulator and exposes a stable C ABI so that [Maestro](https://github.com/QoroQuantum/maestro) can load it dynamically as a simulation backend.

## Overview

`maestroquest` bridges Maestro's simulator-agnostic orchestration layer and the high-performance QuEST statevector simulator. At runtime, Maestro opens the compiled shared library (`libmaestroquest.so` / `libmaestroquest.dylib` / `maestroquest.dll`) and calls the exported C functions to:

- Create and destroy qubit registers
- Apply single-, two-, and three-qubit quantum gates
- Measure individual qubits or multi-qubit registers
- Query state probabilities, amplitudes, and expectation values

QuEST itself is fetched automatically from source during the build – no manual QuEST installation is required.

## Prerequisites

| Dependency | Minimum version | Notes |
|---|---|---|
| C++ compiler (GCC, Clang, MSVC) | C++17 | |
| CMake | 3.20 | |
| Git | any | needed by CMake's `FetchContent` to download QuEST |

**Ubuntu / Debian:**

```bash
sudo apt-get install build-essential cmake git
```

**Fedora / RHEL:**

```bash
sudo dnf install gcc-c++ cmake git
```

**macOS:**

```bash
brew install cmake
```

**Windows:** Install Visual Studio 2019 or later with the *Desktop development with C++* workload, and ensure CMake and Git are on your `PATH`.

## Building

```bash
git clone https://github.com/QoroQuantum/maestro-quest-interface.git
cd maestro-quest-interface

cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

> **macOS note:** QuEST enables multithreading (OpenMP) by default. If OpenMP is not installed, either install it (`brew install libomp && export OpenMP_ROOT=$(brew --prefix)/opt/libomp`) or disable it:
>
> ```bash
> cmake -B build -DCMAKE_BUILD_TYPE=Release -DENABLE_MULTITHREADING=OFF
> ```

### HPC / Distributed Builds (MPI & GPU)

For HPC environments that require distributed (MPI) or GPU-accelerated simulation, use the `MAESTROQUEST_MPI` and `MAESTROQUEST_GPU` build options:

```bash
# MPI only (distributed state vector across nodes)
cmake -B build -DCMAKE_BUILD_TYPE=Release -DMAESTROQUEST_MPI=ON

# GPU only
cmake -B build -DCMAKE_BUILD_TYPE=Release -DMAESTROQUEST_GPU=ON

# Both MPI and GPU
cmake -B build -DCMAKE_BUILD_TYPE=Release -DMAESTROQUEST_MPI=ON -DMAESTROQUEST_GPU=ON
```

These flags are forwarded to QuEST's build system and will automatically:

- Find and link against your system's MPI installation (for `-DMAESTROQUEST_MPI`)
- Enable GPU compilation via CUDA or HIP (for `-DMAESTROQUEST_GPU`)

> **MPI-aware initialization:** When built with `-DMAESTROQUEST_MPI=ON`, the library automatically detects whether MPI has already been initialized by the host application (e.g. via `mpirun` / `srun`). If so, `Initialize()` joins the existing MPI environment and `Finalize()` will **not** call `MPI_Finalize`, leaving MPI lifecycle management to the host. This is safe for embedding in larger MPI applications on HPC clusters.

**Running with MPI:**

```bash
# The library detects the existing MPI environment automatically
mpirun -np 4 ./your_application
```

CMake automatically fetches and builds QuEST from source. The build produces:

| Artifact | Location |
|---|---|
| `libmaestroquest.so` (Linux) / `libmaestroquest.dylib` (macOS) / `maestroquest.dll` (Windows) | `build/` |
| `tests` executable | `build/` (Linux / macOS) or `build/Release/tests.exe` (Windows) |

> **Note:** The CI pipeline currently builds the project but does not run tests. To verify correctness, run the tests locally as described below.

### Running the Tests

```bash
./build/tests             # Linux / macOS
build\Release\tests.exe   # Windows
```

A passing run prints:

```
All tests passed successfully!
```

## Integrating with Maestro

Maestro dynamically loads `maestroquest` through its `QuestLibSim` adapter
([`Simulators/QuestLibSim.h`](https://github.com/QoroQuantum/maestro/blob/main/Simulators/QuestLibSim.h)).
Follow these steps to enable the QuEST backend in a Maestro installation.

### Step 1 – Build maestroquest

Follow the [Building](#building) section above to produce the shared library. Note the path to the compiled library file, for example:

```
/path/to/maestro-quest-interface/build/libmaestroquest.so      # Linux
/path/to/maestro-quest-interface/build/libmaestroquest.dylib   # macOS
```

### Step 2 – Install / build Maestro

Clone and build Maestro according to its [installation guide](https://github.com/QoroQuantum/maestro/blob/main/INSTALL.md):

```bash
git clone https://github.com/QoroQuantum/maestro.git
cd maestro
./build.sh
```

### Step 3 – Point Maestro to the library

The `QuestLibSim` loader expects the full path (or a library name resolvable by the dynamic linker) to `libmaestroquest`.

**Option A – pass the path directly in C++ code:**

```cpp
#include "Simulators/QuestLibSim.h"

Simulators::QuestLibSim questLib;
if (!questLib.Init("/path/to/libmaestroquest.dylib")) {   // .so on Linux
    std::cerr << "Failed to load maestroquest library\n";
}
```

**Option B – copy the library to a standard search path:**

```bash
# Linux
sudo cp build/libmaestroquest.so /usr/local/lib/
sudo ldconfig
```

**Option C – set the library search path at runtime:**

```bash
# Linux
export LD_LIBRARY_PATH=/path/to/maestro-quest-interface/build:$LD_LIBRARY_PATH

# macOS
export DYLD_LIBRARY_PATH=/path/to/maestro-quest-interface/build:$DYLD_LIBRARY_PATH
```

### Step 4 – Use the QuEST backend via C++

The QuEST backend is exposed as simulator type `3` (`kQuestSim`) in Maestro and supports statevector simulation only.

```cpp
#include "maestrolib/Interface.h"

// Forward-declare the QuEST init function from libmaestro
namespace Simulators {
class SimulatorsFactory {
 public:
  static bool InitQuestLibrary();
};
}

int main() {
    // Initialize Maestro and load the QuEST library
    GetMaestroObject();
    Simulators::SimulatorsFactory::InitQuestLibrary();

    // Create a simulator and select the QuEST statevector backend
    unsigned long int handle = CreateSimpleSimulator(2);
    RemoveAllOptimizationSimulatorsAndAdd(handle, /*kQuestSim=*/3, /*kStatevector=*/0);

    // Execute a Bell-state circuit via QASM
    const char *qasm =
        "OPENQASM 2.0;\n"
        "include \"qelib1.inc\";\n"
        "qreg q[2]; creg c[2];\n"
        "h q[0]; cx q[0], q[1];\n"
        "measure q -> c;\n";

    char *result = SimpleExecute(handle, qasm, "{\"shots\": 1000}");
    // result is a JSON string: {"counts":{"00":505,"11":495}, ...}
    FreeResult(result);
    DestroySimpleSimulator(handle);
}
```

See [`examples/quest_example.cpp`](examples/quest_example.cpp) for a full working example including expectation-value estimation.

### Step 5 – Use the QuEST backend via Python

The QuEST backend is fully exposed in the Maestro Python bindings as `SimulatorType.QuestSim`. It supports statevector simulation only.

#### Library management

Before using the QuEST simulator, initialize the library once at startup:

```python
import maestro

# Initialize the QuEST shared library (call once)
maestro.init_quest()

# Check whether the library loaded successfully
if maestro.is_quest_available():
    print("QuEST backend is ready")
```

> **Note:** `init_quest()` returns `True` on success. If it returns `False`, make sure `libmaestroquest` is on the library search path (see [Step 3](#step-3--point-maestro-to-the-library)).

#### Running circuits with `simple_execute`

```python
import maestro

maestro.init_quest()

qasm = """
OPENQASM 2.0;
include "qelib1.inc";
qreg q[2];
creg c[2];
h q[0];
cx q[0], q[1];
measure q -> c;
"""

result = maestro.simple_execute(
    qasm,
    simulator_type=maestro.SimulatorType.QuestSim,
    simulation_type=maestro.SimulationType.Statevector,
    shots=1000
)

print(result['counts'])      # e.g. {'00': 502, '11': 498}
print(result['time_taken'])  # execution time in seconds
```

#### Estimating expectation values with `simple_estimate`

```python
import maestro

maestro.init_quest()

qasm = """
OPENQASM 2.0;
include "qelib1.inc";
qreg q[2];
h q[0];
cx q[0], q[1];
"""

result = maestro.simple_estimate(
    qasm,
    "ZZ;XX;YY",
    simulator_type=maestro.SimulatorType.QuestSim,
    simulation_type=maestro.SimulationType.Statevector,
)

print(result['expectation_values'])  # [1.0, 1.0, -1.0]
```

#### Using the `QuantumCircuit` API

```python
from maestro.circuits import QuantumCircuit
import maestro

maestro.init_quest()

qc = QuantumCircuit()
qc.h(0)
qc.cx(0, 1)
qc.measure_all()

result = qc.execute(
    simulator_type=maestro.SimulatorType.QuestSim,
    simulation_type=maestro.SimulationType.Statevector,
    shots=1000
)
print(result['counts'])
```

> **Important:** `QuestSim` only supports `SimulationType.Statevector`. Requesting any other simulation type (e.g. `MatrixProductState`, `Stabilizer`) will raise an error.

Refer to the [Maestro tutorial](https://github.com/QoroQuantum/maestro/blob/main/TUTORIAL.md) and [examples](https://github.com/QoroQuantum/maestro/tree/main/examples) for full usage details.

## Examples

The [`examples/`](examples/) directory contains runnable examples for both Python and C++.

### Python

```bash
# Set the library search path so Maestro can find libmaestroquest
export DYLD_LIBRARY_PATH=/path/to/maestro-quest-interface/build:$DYLD_LIBRARY_PATH  # macOS
export LD_LIBRARY_PATH=/path/to/maestro-quest-interface/build:$LD_LIBRARY_PATH      # Linux

python examples/quest_example.py
```

Covers library initialisation, QASM-based circuit execution, expectation-value estimation, the `QuantumCircuit` API, and parametric rotation circuits.

### C++

The C++ example requires a built Maestro installation. Build with:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DMAESTRO_DIR=/path/to/maestro
cmake --build build --target quest_example
```

Run (both `libmaestro` and `libmaestroquest` must be on the library search path):

```bash
# macOS
export DYLD_LIBRARY_PATH=/path/to/maestro/build:/path/to/maestro-quest-interface/build
./build/quest_example

# Linux
export LD_LIBRARY_PATH=/path/to/maestro/build:/path/to/maestro-quest-interface/build
./build/quest_example
```

Covers QASM-based circuit execution and expectation-value estimation through Maestro's orchestration layer.

## C API Reference

The following functions are exported by `maestroquest`. They form the contract that Maestro's `QuestLibSim` adapter depends on.

### Lifecycle

| Function | Description |
|---|---|
| `void Initialize()` | Initializes the QuEST environment. Must be called once before any other function. When built with `-DMAESTROQUEST_MPI`, automatically detects an existing MPI context and joins it instead of calling `MPI_Init`. |
| `void Finalize()` | Destroys all remaining simulators and frees their resources. If this library initialized the QuEST/MPI environment, finalizes it; otherwise leaves MPI lifecycle to the host. |

### Simulator Management

| Function | Description |
|---|---|
| `unsigned long int CreateSimulator(int nrQubits)` | Creates a new qubit register initialized to \|0⟩. Returns an opaque handle. |
| `void DestroySimulator(unsigned long int simHandle)` | Destroys the qubit register identified by `simHandle`. |
| `unsigned long int CloneSimulator(void* sim)` | Clones an existing register (raw pointer) and returns a handle to the copy. Returns 0 if `sim` is null. |
| `void* GetSimulator(unsigned long int simHandle)` | Returns a raw pointer to the qubit register for gate operations. |

### State Queries

| Function | Description |
|---|---|
| `int GetNumQubits(void* sim)` | Returns the number of qubits in the register. |
| `double GetQubitProbability0(void* sim, int qubit)` | Probability of measuring qubit in state \|0⟩. |
| `double GetQubitProbability1(void* sim, int qubit)` | Probability of measuring qubit in state \|1⟩. |
| `double GetOutcomeProbability(void* sim, long long int outcome)` | Probability of a specific basis state. |
| `double GetExpectationValue(void* sim, const char* pauliStr)` | Expectation value of a Pauli string (e.g. `"ZZI"`). The string length must not exceed the number of qubits in the register. |
| `int GetAmplitudes(void* sim, void* buffer, unsigned long long int bufSize)` | Copies all state-vector amplitudes into `buffer`. Returns 1 on success. |
| `int GetAmplitude(void* sim, long long int index, void* outAmp, unsigned long long int bufSize)` | Copies a single amplitude at `index` into `outAmp`. Returns 1 on success. |
| `int IsDoublePrecision()` | Returns 1 if the library was built with double precision, 0 for single. |

### Measurement

| Function | Description |
|---|---|
| `int Measure(void* sim, int qubit)` | Measures a single qubit; collapses the state. Returns 0 or 1. |
| `long long int MeasureQubits(void* sim, int* qubits, int numQubits)` | Measures multiple qubits; returns the outcome as a bit-string integer. |

### Single-Qubit Gates

| Function | Gate |
|---|---|
| `ApplyX(sim, qubit)` | Pauli-X (NOT) |
| `ApplyY(sim, qubit)` | Pauli-Y |
| `ApplyZ(sim, qubit)` | Pauli-Z |
| `ApplyH(sim, qubit)` | Hadamard |
| `ApplyS(sim, qubit)` | S |
| `ApplySdg(sim, qubit)` | S† |
| `ApplyT(sim, qubit)` | T |
| `ApplyTdg(sim, qubit)` | T† |
| `ApplySx(sim, qubit)` | √X |
| `ApplySxDg(sim, qubit)` | (√X)† |
| `ApplyK(sim, qubit)` | K (Hy) |
| `ApplyP(sim, qubit, angle)` | Phase shift P(θ) |
| `ApplyRx(sim, qubit, angle)` | Rotation Rx(θ) |
| `ApplyRy(sim, qubit, angle)` | Rotation Ry(θ) |
| `ApplyRz(sim, qubit, angle)` | Rotation Rz(θ) |
| `ApplyU(sim, qubit, theta, phi, lambda, gamma)` | General U gate |

### Two-Qubit Gates

| Function | Gate |
|---|---|
| `ApplyCX(sim, ctrl, tgt)` | CNOT |
| `ApplyCY(sim, ctrl, tgt)` | Controlled-Y |
| `ApplyCZ(sim, ctrl, tgt)` | Controlled-Z |
| `ApplyCS(sim, ctrl, tgt)` | Controlled-S |
| `ApplyCT(sim, ctrl, tgt)` | Controlled-T |
| `ApplyCH(sim, ctrl, tgt)` | Controlled-H |
| `ApplySwap(sim, qubit1, qubit2)` | SWAP |
| `ApplyCP(sim, ctrl, tgt, angle)` | Controlled phase |
| `ApplyCRx(sim, ctrl, tgt, angle)` | Controlled Rx |
| `ApplyCRy(sim, ctrl, tgt, angle)` | Controlled Ry |
| `ApplyCRz(sim, ctrl, tgt, angle)` | Controlled Rz |
| `ApplyCSx(sim, ctrl, tgt)` | Controlled √X |
| `ApplyCSxDg(sim, ctrl, tgt)` | Controlled (√X)† |
| `ApplyCU(sim, ctrl, tgt, theta, phi, lambda, gamma)` | Controlled-U |

### Three-Qubit Gates

| Function | Gate |
|---|---|
| `ApplyCCX(sim, ctrl1, ctrl2, tgt)` | Toffoli (CCX) |
| `ApplyCSwap(sim, ctrl, qubit1, qubit2)` | Fredkin (controlled-SWAP) |

## License

This project is licensed under the GNU General Public License v3.0.

A copy of the license text is available in the [LICENSE](LICENSE) file and at <https://www.gnu.org/licenses/gpl-3.0.en.html>.
