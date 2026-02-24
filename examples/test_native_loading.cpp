/**
 * Native C++ dynamic loading test for libmaestroquest.
 *
 * This reproduces what Maestro's QuestLibSim / Factory does at runtime:
 *   1. dlopen the shared library
 *   2. dlsym the exported C functions
 *   3. Call Initialize, CreateSimulator, apply a gate, measure, Finalize
 *
 * Build:
 *   c++ -std=c++17 -o test_dlopen examples/test_native_loading.cpp -ldl
 *
 * Run:
 *   DYLD_LIBRARY_PATH=build ./test_dlopen        # macOS
 *   LD_LIBRARY_PATH=build  ./test_dlopen          # Linux
 */

#include <cstdio>
#include <cstdlib>
#include <dlfcn.h>

int main() {
    // --- 1. Open the library (same name Maestro Factory uses) ---
#ifdef __APPLE__
    const char* libName = "libmaestroquest.dylib";
#else
    const char* libName = "libmaestroquest.so";
#endif

    printf("Loading %s ...\n", libName);
    void* handle = dlopen(libName, RTLD_NOW);
    if (!handle) {
        fprintf(stderr, "FAIL: dlopen: %s\n", dlerror());
        return 1;
    }
    printf("  ✓ Library loaded\n");

    // --- 2. Resolve symbols ---
    auto Initialize        = (void(*)())                            dlsym(handle, "Initialize");
    auto Finalize          = (void(*)())                            dlsym(handle, "Finalize");
    auto CreateSimulator   = (unsigned long(*)(int))                dlsym(handle, "CreateSimulator");
    auto DestroySimulator  = (void(*)(unsigned long))               dlsym(handle, "DestroySimulator");
    auto GetSimulator      = (void*(*)(unsigned long))              dlsym(handle, "GetSimulator");
    auto GetNumQubits      = (int(*)(void*))                        dlsym(handle, "GetNumQubits");
    auto ApplyH            = (void(*)(void*, int))                  dlsym(handle, "ApplyH");
    auto ApplyCX           = (void(*)(void*, int, int))             dlsym(handle, "ApplyCX");
    auto Measure           = (int(*)(void*, int))                   dlsym(handle, "Measure");
    auto GetQubitProbability0 = (double(*)(void*, int))              dlsym(handle, "GetQubitProbability0");
    auto IsDoublePrecision = (int(*)())                             dlsym(handle, "IsDoublePrecision");

    if (!Initialize || !Finalize || !CreateSimulator || !DestroySimulator ||
        !GetSimulator || !GetNumQubits || !ApplyH || !ApplyCX || !Measure ||
        !GetQubitProbability0 || !IsDoublePrecision) {
        fprintf(stderr, "FAIL: dlsym: %s\n", dlerror());
        dlclose(handle);
        return 1;
    }
    printf("  ✓ All symbols resolved\n");

    // --- 3. Use the library ---
    Initialize();
    printf("  ✓ QuEST environment initialized\n");

    printf("  Double precision: %s\n", IsDoublePrecision() ? "yes" : "no");

    unsigned long sim_handle = CreateSimulator(2);
    void* sim = GetSimulator(sim_handle);
    printf("  Created 2-qubit simulator (handle=%lu, qubits=%d)\n",
           sim_handle, GetNumQubits(sim));

    // Build a Bell state: H(0), CX(0,1)
    ApplyH(sim, 0);
    ApplyCX(sim, 0, 1);

    double p0_q0 = GetQubitProbability0(sim, 0);
    double p0_q1 = GetQubitProbability0(sim, 1);
    printf("  Bell state P(|0⟩): q0=%.4f  q1=%.4f  (expect ~0.5)\n", p0_q0, p0_q1);

    if (p0_q0 < 0.49 || p0_q0 > 0.51 || p0_q1 < 0.49 || p0_q1 > 0.51) {
        fprintf(stderr, "FAIL: probabilities outside expected range\n");
        DestroySimulator(sim_handle);
        Finalize();
        dlclose(handle);
        return 1;
    }
    printf("  ✓ Bell state probabilities correct\n");

    // Measure
    int m0 = Measure(sim, 0);
    int m1 = Measure(sim, 1);
    printf("  Measurement: q0=%d  q1=%d  (should be equal)\n", m0, m1);
    if (m0 != m1) {
        fprintf(stderr, "FAIL: Bell state measurement mismatch\n");
        DestroySimulator(sim_handle);
        Finalize();
        dlclose(handle);
        return 1;
    }
    printf("  ✓ Bell state measurement consistent\n");

    // --- 4. Cleanup ---
    DestroySimulator(sim_handle);
    Finalize();
    dlclose(handle);

    printf("\nAll native loading tests passed!\n");
    return 0;
}
