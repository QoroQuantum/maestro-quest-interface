"""
Maestro + QuEST Integration Example
====================================

Demonstrates using the QuEST quantum simulator backend through Maestro's
Python bindings.  Covers:

  1. Library initialisation and availability check
  2. Running a Bell-state circuit via QASM with simple_execute
  3. Estimating Pauli expectation values with simple_estimate
  4. Building a circuit programmatically with the QuantumCircuit API
"""

import maestro
from maestro.circuits import QuantumCircuit

SEPARATOR = "-" * 60


def check_quest_backend():
    """Initialize and verify the QuEST backend is available."""
    print(SEPARATOR)
    print("1. Initializing QuEST backend")
    print(SEPARATOR)

    ok = maestro.init_quest()
    print(f"   init_quest() returned: {ok}")
    print(f"   is_quest_available() : {maestro.is_quest_available()}")

    if not ok:
        raise RuntimeError(
            "QuEST library could not be loaded. "
            "Make sure libmaestroquest.so/.dylib is on DYLD_LIBRARY_PATH / LD_LIBRARY_PATH."
        )
    print("   ✓ QuEST backend ready\n")


def run_bell_state_qasm():
    """Execute a Bell-state circuit from a QASM string."""
    print(SEPARATOR)
    print("2. Bell State via QASM (simple_execute)")
    print(SEPARATOR)

    qasm = """\
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
        shots=1000,
    )

    print(f"   Counts       : {result['counts']}")
    print(f"   Time (s)     : {result['time_taken']:.6f}")
    print(f"   Simulator ID : {result['simulator']}")
    print(f"   Method ID    : {result['method']}")
    print("   ✓ Bell state executed successfully\n")
    return result


def estimate_expectations():
    """Estimate Pauli expectation values for a Bell state."""
    print(SEPARATOR)
    print("3. Expectation Values via QASM (simple_estimate)")
    print(SEPARATOR)

    qasm = """\
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

    labels = ["ZZ", "XX", "YY"]
    print("   Observable  Expectation")
    for label, val in zip(labels, result["expectation_values"]):
        print(f"   {label:>10}  {val:+.6f}")
    print(f"   Time (s)  : {result['time_taken']:.6f}")
    print("   ✓ Expectation values computed\n")
    return result


def run_quantum_circuit_api():
    """Build and execute a GHZ state using the QuantumCircuit API."""
    print(SEPARATOR)
    print("4. GHZ State via QuantumCircuit API")
    print(SEPARATOR)

    n_qubits = 4
    qc = QuantumCircuit()
    qc.h(0)
    for i in range(1, n_qubits):
        qc.cx(0, i)
    qc.measure_all()

    result = qc.execute(
        simulator_type=maestro.SimulatorType.QuestSim,
        simulation_type=maestro.SimulationType.Statevector,
        shots=2000,
    )

    print(f"   {n_qubits}-qubit GHZ state")
    print(f"   Counts     : {result['counts']}")
    print(f"   Time (s)   : {result['time_taken']:.6f}")
    print("   ✓ GHZ circuit executed successfully\n")
    return result


def run_parametric_circuit():
    """Build a parameterized circuit with rotation gates."""
    print(SEPARATOR)
    print("5. Parametric Circuit (Rotation Gates)")
    print(SEPARATOR)

    import math

    qc = QuantumCircuit()
    # Prepare a variational ansatz-like circuit
    for q in range(3):
        qc.ry(q, math.pi / 4)
    qc.cx(0, 1)
    qc.cx(1, 2)
    for q in range(3):
        qc.rz(q, math.pi / 3)
    qc.measure_all()

    result = qc.execute(
        simulator_type=maestro.SimulatorType.QuestSim,
        simulation_type=maestro.SimulationType.Statevector,
        shots=2000,
    )

    print(f"   Counts     : {result['counts']}")
    print(f"   Time (s)   : {result['time_taken']:.6f}")
    print("   ✓ Parametric circuit executed successfully\n")
    return result


def main():
    print()
    print("=" * 60)
    print("  Maestro + QuEST Integration Example")
    print("=" * 60)
    print()

    check_quest_backend()
    run_bell_state_qasm()
    estimate_expectations()
    run_quantum_circuit_api()
    run_parametric_circuit()

    print(SEPARATOR)
    print("All examples completed successfully!")
    print(SEPARATOR)


if __name__ == "__main__":
    main()
