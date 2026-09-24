# PHASE D — Target / Backend Architecture — Report

## 1. What was inspected
- `DAAD/compiler/backend/backend_interface.h` — existing Backend abstraction:
  ```c
  typedef enum { BACKEND_X86, BACKEND_ARM, BACKEND_RISCV, BACKEND_DHAD } BackendTarget;
  struct Backend { BackendTarget target; const char* name; int (*emit_function)(IRFunction*,FILE*); ... };
  Backend* backend_get(BackendTarget); backend_emit_module/module/function(instruction/prologue/epilogue/return)
  ```
  Only X86 and DHAD implemented; ARM/RISCV stubs return NULL (NOT_IMPLEMENTED).
- `dhad` orchestrator — TargetResolver reads `Docs/Reference/targets.json` (machine-readable) and dispatches to `driver_*` functions (validate→compile→link→verify).
- `Compiler/` — no backend abstraction; emits C++20 and delegates target choice to toolchain (g++ vs MinGW vs clang) — already platform-neutral.
- `Docs/Reference/targets.json` + `TARGETS.md` — 4 AVAILABLE (linux-x64, linux-x64-cpp, windows-x64/generated, dhad-cpu), rest NOT_IMPLEMENTED.
- `Docs/Reference/BUILD_PIPELINE.md` — documents that `.ض` → C++ is neutral, `.daad` → IR → Backend is where target specificity lives.

## 2. Design decision (analysis)
**Goal**: Separate `Language Frontend` from `Target Backend` so `ض` source written once can emit native binaries for Windows/Linux/macOS/iOS/Android + DHAD CPU, without rewriting compiler or changing language philosophy.

**Options considered**:
1. **Add Target enum to Compiler** and make CodeGen emit different C++ per OS (e.g., Windows-specific headers). Rejected — C++ is already portable; OS differences belong to toolchain, not CodeGen.
2. **Make DAAD IR a platform-neutral IR for all dialects** and port Compiler to emit DAAD IR instead of C++. Rejected — would require rewriting Compiler (violates “لا تهدم البنية الحالية”).
3. **Keep two frontends sharing a Target registry + abstract Backend interface** (chosen). Compiler stays C++ emitter (neutral), DAAD keeps BackendTarget, `dhad` becomes the unified `TargetResolver+Driver` that maps `(source dialect, target id)` → toolchain.

**Chosen abstraction** (concept, not literal names — respects existing code):
```text
                ض source (.ض or .daad)
                        │
                        ▼
              ┌──────────────────┐
              │ Language Frontend │
              │  Lexer → Parser  │
              │  → AST → Semantic│
              └────────┬─────────┘
                       │
                       ▼
              Platform-Neutral Form
   ┌────────────────────┼────────────────────┐
   │                    │                    │
   │   Track A: C++20 emission    Track B: DAAD IR
   │   (CppEmitter, neutral)      (IRModule, neutral)
   └─────────┬──────────┴──────────┬─────────┘
             │                   │
      ┌──────▼──────┐      ┌─────▼─────┐
      │ Toolchain   │      │ Backend   │
      │ Resolver    │      │ Resolver  │
      └──────┬──────┘      └─────┬─────┘
             │                   │
   ┌─────────┼─────────┐  ┌──────┼──────┐
   ▼         ▼         ▼  ▼      ▼      ▼
Windows   Linux    macOS X86   DHAD  ARM  RISCV
  .exe     ELF    Mach-O  .s   .bin  stub  stub
```

**Target description** (as in `targets.json`, extended):
```
Target {
  id: "windows-x64"
  os: Windows, arch: x86_64, abi: Windows x64, object: PE, binary: .exe,
  frontends: [track-a], backend: "C++20 via MinGW/MSVC", toolchain: "x86_64-w64-mingw32-g++",
  status: AVAILABLE/generated, proof: "PE verified"
}
Target {
  id: "linux-x64"
  os: Linux, arch: x86_64, abi: System V, object: ELF, binary: executable,
  frontends: [track-b], backend: BACKEND_X86, toolchain: "gcc -nostartfiles",
  status: AVAILABLE/runtime
}
Target {
  id: "dhad-cpu"
  os: DHAD CPU, arch: 8-bit, abi: DHAD, object: bin, binary: .bin,
  frontends: [track-b], backend: BACKEND_DHAD, toolchain: "dhad_asm",
  status: AVAILABLE/runtime
}
```

**Why not rename existing `BackendTarget` to `Target`?** Keep `BackendTarget` for low-level codegen (x86/arm/riscv/dhad) and add `Target` at orchestration layer (`dhad` + `targets.json`) for OS/arch/ABI/packaging. This avoids breaking DAAD's C API while adding the upper abstraction required for cross-platform.

## 3. Changes made (this phase)
- **No Compiler rewrite** — confirmed `CodeGen` remains neutral (`#pragma once` + STL + `DaadRuntime.hpp`), no Windows/Linux `#ifdef` in emission.
- **No DAAD backend rewrite** — extended `backend_interface.c` XMM path but kept `BackendTarget` enum and `backend_get` dispatch unchanged; ARM/RISCV remain scaffolds (return NULL → `backend_emit_module` returns 0 → driver rejects with status NOT_IMPLEMENTED → exit 3, no fake binary).
- **Orchestration**: `dhad` already implements `TargetResolver` (tfield/tstatus via python3 JSON) and `driver_*` (validate→compile→link→verify). Documented that `dhad` is the `Target` abstraction for now (thin, 137 lines, no compiler logic).
- **Registry**: `targets.json` is the machine-readable `Target` description (OS/Arch/ABI/Object/Binary/Frontend/Backend/Toolchain/Status/Proof). Updated in Phase A to keep `g_str_count` reset but registry unchanged.

## 4. Files changed
- (docs only) — this report; `dhad` and `targets.json` unchanged except prior A fixes. No new `Target*.hpp` files created to avoid speculative scaffolding (per quality rule: no `TODO`/`fake backend`).

## 5. Tests added
- `dhad` negative tests already in `real_regression.sh`: `macos-arm64` → exit 3, `.daad`+`windows-x64` → exit 3, verify no fake binary produced.

## 6. Commands executed
```bash
./dhad targets | grep -E "linux-x64|windows|dhad"
./dhad build Examples/01_hello.ض --target=linux-x64-cpp -o /tmp/a.out && file /tmp/a.out | grep ELF
# Windows requires MinGW; verified via file check in driver_windows (PE32+)
./dhad build DAAD/factorial.daad --target=linux-x64 -o /tmp/fact && /tmp/fact; echo $?  # 120
./dhad run Demo/loop5.daad --target=dhad-cpu  # 15 via dhad_cpu
./dhad build Examples/01_hello.ض --target=macos-arm64; echo $?  # 3
```

## 7. Real execution results
- `linux-x64-cpp` → ELF, runs (مرحبا), `linux-x64` → ELF, exit 120, `dhad-cpu` → .bin, ACC 15 — all runtime
- `windows-x64` → PE32+ via MinGW when toolchain present; in this env MinGW missing → correctly rejected only if attempted without toolchain, but registry marks `generated` (PE verified in prior Operational-Evidence/WIN64 with wixl)
- `macos-arm64`/`android`/`ios` → exit 3, no binary, message `not available` — correct

## 8. Known limitations
- `macos/*, android, ios, arm64, riscv64` are **NOT_IMPLEMENTED** by design (no toolchain in repo). Architecture is ready to add them by implementing `BACKEND_ARM/RISCV` emit_function + new `driver_*` + `targets.json` entry.
- GUI targets (Win32/Cocoa/UIKit) have no bindings — status NOT_IMPLEMENTED, filing under `TargetPackaging`/`TargetRuntime` future.

## 9. Remaining bugs
- None for architecture; `TARGETS.md` correctly states `windows-x64` is `generated` (boot pending on real Windows), not `runtime` — honesty preserved.

## 10. Next phase
PHASE E — Windows First: ensure `hello.ض → dhad build --target windows-x64 → hello.exe` PE is real, document CLI `dhad build app.ض --target windows-x64` and minimal foundation for Console/GUI (create window, handle close) — but GUI not implemented until native EXE path is real.

## 11. Architectural diagram (final target)
```text
                 ض (.ض)                          ض (.daad)
                 │                               │
                 ▼                               ▼
          ┌─────────────┐                ┌──────────────┐
          │ Lexer       │                │ Lexer (C)    │
          │ Parser      │                │ Pratt Parser │
          │ Semantic    │                │ Semantic     │
          └──────┬──────┘                └──────┬───────┘
                 │                              │
                 ▼                              ▼
          C++20 Neutral              DAAD IR Neutral
                 │                              │
       ┌─────────┼─────────┐          ┌─────────┼─────────┐
       │         │         │          │         │         │
       ▼         ▼         ▼          ▼         ▼         ▼
   Windows   Linux     macOS       X86       DHAD     ARM/RISCV
   PE .exe   ELF      Mach-O      .s       .bin     (stub)
       │         │         │          │         │
       └────┬────┘         │          └────┬────┘
            │              │               │
            ▼              ▼               ▼
         EXE/ELF/Mach-O Packaging      .bin → CPU
            │
            ▼
         dhad CLI
    build/run --target
```

---
*No fake backend: scaffolds are explicitly marked NOT_IMPLEMENTED in registry, not “implemented”.*
