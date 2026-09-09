# PHASE_E_REPORT.md — Windows First Native Target

## 1. What was inspected
- Track A output for `01_hello.ض`, `02_arithmetic.ض`, `06_factorial.ض` (runtime headers `Daad/Runtime/*`, stdlib).
- Available cross toolchain: `x86_64-w64-mingw32-g++` (MinGW-w64). No MSVC/Windows machine here.

## 2. Problems found
None blocking: generated C++20 is plain standard code (iostream/string/vector) + project headers;
it cross-compiles cleanly. (Track B x86 output is Linux-SysV-only by design — NOT used for Windows.)

## 3. Root causes
N/A (green path). The architecture decision from PHASE D holds: Track A is the Windows vehicle
because C++ is platform-neutral; no Windows-specific backend code was needed.

## 4. Changes made
- `Demo/build-windows.sh` (new): `.ض → C++ → PE32+ static EXE` with checks at each stage.
- No compiler changes for this phase.

## 5. Files changed
`Demo/build-windows.sh`; evidence in `Operational-Evidence/WIN64/`.

## 6. Tests added
The script itself is the test: it fails loudly if any stage fails and verifies PE + DLL list.

## 7. Commands executed
```bash
bash Demo/build-windows.sh Examples/01_hello.ض /tmp/opencode/demo_win_hello.exe
bash Demo/build-windows.sh Examples/02_arithmetic.ض /tmp/opencode/demo_win_arith.exe
# (manual) factorial cross-compile with -static, DLL/import inspection
```

## 8. Real execution results
- `01_hello.ض` → `demo_win_hello.exe`: **PE32+ x86-64 console**, DLLs: KERNEL32 + msvcrt only.
- `02_arithmetic.ض` → same result class, same 2 system DLLs.
- `06_factorial.ض` (functions + loops) → `win_fact.exe` 2.5 MB static, same 2 DLLs.
- Runtime execution on Windows NOT possible here (no Windows/Wine) — honesty note: PE validity,
  subsystem (console), machine type, and import table are machine-verified; first-boot on real
  Windows remains the acceptance step for the interview machine.

## 9. Known limitations
1. Execution proven by construction (valid PE + console subsystem + stock DLLs), not by boot.
2. GUI apps out of scope (no Win32 bindings — roadmap).
3. `dhad build --target` unified CLI does NOT exist; the documented interface is per-component
   CLIs + `Demo/build-windows.sh`. Do not invent flags.

## 10. Remaining bugs
None new. N2/N3 (int-literal print, huge-int store) also apply to cross-built binaries (shared backend paths).

## 11. Next phase
F/G/H: honest roadmaps (no Apple/Android toolchains in this environment — design + gates only).
