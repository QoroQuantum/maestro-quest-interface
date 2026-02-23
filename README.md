# Maestro QuEST Interface

[![Built on Ubuntu](https://github.com/QoroQuantum/maestro-quest-interface/actions/workflows/cmake-single-platform.yml/badge.svg)](https://github.com/QoroQuantum/maestro-quest-interface/actions/workflows/cmake-single-platform.yml)

A shared library that wraps the [QuEST](https://github.com/QuEST-Kit/QuEST) quantum circuit simulator and exposes a stable C ABI so that [Maestro](https://github.com/QoroQuantum/maestro) can load it dynamically as a simulation backend.

## Overview

`maestroquest` bridges Maestro's simulator-agnostic orchestration layer and the high-performance QuEST statevector simulator. At runtime, Maestro opens the compiled shared library (`libmaestroquest.so` / `maestroquest.dll` / `libmaestroquest.dylib`) and calls the exported C functions to:

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

CMake automatically fetches and builds QuEST from source. The build produces:

| Artifact | Location |
|---|---|
| `libmaestroquest.so` (Linux) / `libmaestroquest.dylib` (macOS) / `maestroquest.dll` (Windows) | `build/` |
| `tests` executable | `build/` |

### Running the Tests

```bash
cd build
ctest --output-on-failure
```

Or run the test binary directly:

```bash
./build/tests          # Linux / macOS
build\Release\tests    # Windows
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
/path/to/maestro-quest-interface/build/libmaestroquest.so
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
if (!questLib.Init("/path/to/libmaestroquest.so")) {
    std::cerr << "Failed to load maestroquest library\n";
}
```

**Option B – copy the library to a standard search path and use the bare name:**

```bash
# Linux example
sudo cp build/libmaestroquest.so /usr/local/lib/
sudo ldconfig

# Then in code:
questLib.Init("libmaestroquest.so");
```

**Option C – add the build directory to `LD_LIBRARY_PATH` (Linux / macOS) at runtime:**

```bash
export LD_LIBRARY_PATH=/path/to/maestro-quest-interface/build:$LD_LIBRARY_PATH
# macOS: export DYLD_LIBRARY_PATH=...
```

### Step 4 – Use the QuEST backend via Maestro's Python API

After loading the library, Maestro automatically routes circuits to the QuEST backend when it is selected. Using Maestro's Python bindings:

```python
import maestro

# Simulate a 2-qubit Bell circuit using the QuEST backend
result = maestro.simulate(
    circuit,                        # a Qiskit QuantumCircuit
    backend="quest",                # select the QuEST backend
    lib_path="/path/to/libmaestroquest.so"
)
print(result.get_counts())
```

Refer to the [Maestro tutorial](https://github.com/QoroQuantum/maestro/blob/main/TUTORIAL.md) and [Python examples](https://github.com/QoroQuantum/maestro/tree/main/examples) for full usage details.

## C API Reference

The following functions are exported by `maestroquest`. They form the contract that Maestro's `QuestLibSim` adapter depends on.

### Lifecycle

| Function | Description |
|---|---|
| `void Initialize()` | Initializes the QuEST environment. Must be called once before any other function. |
| `void Finalize()` | Finalizes the QuEST environment and frees all resources. |

### Simulator Management

| Function | Description |
|---|---|
| `unsigned long int CreateSimulator(int nrQubits)` | Creates a new qubit register initialized to \|0⟩. Returns an opaque handle. |
| `void DestroySimulator(unsigned long int handle)` | Destroys the qubit register identified by `handle`. |
| `unsigned long int CloneSimulator(void* sim)` | Clones an existing register and returns a handle to the copy. |
| `void* GetSimulator(unsigned long int handle)` | Returns a raw pointer to the qubit register for gate operations. |

### State Queries

| Function | Description |
|---|---|
| `int GetNumQubits(void* sim)` | Returns the number of qubits in the register. |
| `double GetQubitProbability0(void* sim, int qubit)` | Probability of measuring qubit in state \|0⟩. |
| `double GetQubitProbability1(void* sim, int qubit)` | Probability of measuring qubit in state \|1⟩. |
| `double GetOutcomeProbability(void* sim, long long int outcome)` | Probability of a specific basis state. |
| `double GetExpectationValue(void* sim, const char* pauliStr)` | Expectation value of a Pauli string (e.g. `"ZZI"`). |
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
| `ApplyX` | Pauli-X (NOT) |
| `ApplyY` | Pauli-Y |
| `ApplyZ` | Pauli-Z |
| `ApplyH` | Hadamard |
| `ApplyS` | S |
| `ApplySdg` | S† |
| `ApplyT` | T |
| `ApplyTdg` | T† |
| `ApplySx` | √X |
| `ApplySxDg` | (√X)† |
| `ApplyK` | K (Hy) |
| `ApplyP(sim, qubit, angle)` | Phase shift P(θ) |
| `ApplyRx(sim, qubit, angle)` | Rotation Rx(θ) |
| `ApplyRy(sim, qubit, angle)` | Rotation Ry(θ) |
| `ApplyRz(sim, qubit, angle)` | Rotation Rz(θ) |
| `ApplyU(sim, qubit, theta, phi, lambda, gamma)` | General U gate |

### Two-Qubit Gates

| Function | Gate |
|---|---|
| `ApplyCX` | CNOT |
| `ApplyCY` | Controlled-Y |
| `ApplyCZ` | Controlled-Z |
| `ApplyCS` | Controlled-S |
| `ApplyCT` | Controlled-T |
| `ApplyCH` | Controlled-H |
| `ApplySwap` | SWAP |
| `ApplyCP(sim, ctrl, tgt, angle)` | Controlled phase |
| `ApplyCRx / ApplyCRy / ApplyCRz` | Controlled rotations |
| `ApplyCSx / ApplyCSxDg` | Controlled √X / (√X)† |
| `ApplyCU(sim, ctrl, tgt, theta, phi, lambda, gamma)` | Controlled-U |

### Three-Qubit Gates

| Function | Gate |
|---|---|
| `ApplyCCX` | Toffoli (CCX) |
| `ApplyCSwap` | Fredkin (controlled-SWAP) |

## License

This project is licensed under the GNU General Public License v3.0.

A copy of the license text is available in the [LICENSE](LICENSE) file and at <https://www.gnu.org/licenses/gpl-3.0.en.html>.
