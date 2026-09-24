# PHASE B — Real Regression + Execution Verification — Report

## 1. What was inspected
- `Tests/real_regression.sh` (149 lines) — the gold standard: BUILD→EXECUTE→VERIFY, not `compile succeeded`
- `Tests/run_all_tests.sh` (612) — meta-harness syntax-only (compiler syntax, web node --check, electron, VSCode, server) — noted as insufficient
- `DAAD/build` ctest, `CPU/run_tests.sh`, `Compiler/build`, `dhad` drivers
- All pipelines from `Docs/Reference/BUILD_PIPELINE.md`: `program.ض→C++→g++→ELF`, `program.daad→x86→gcc→ELF`, `program.daad→DHAD→.bin→CPU`

## 2. Problems found
| # | Symptom (before host-gcc) | Root cause |
|---|---|---|
| B-01 | `gcc: command not found` inside flatpak container → 37/46 fails | Container image lacks toolchain; host has gcc 16.2.1 at `/run/host/usr/bin/gcc` but not in container PATH |
| B-02 | `as: libbfd-2.47.so: cannot open` when forcing LD_LIBRARY_PATH to host libs | Host glibc 16 vs container glibc 2.42 mismatch (`__pointer_chk_guard`) |
| B-03 | `Tests/run_all_tests.sh` hardcodes `ROOT=/home/m_hmoz/...` (non-portable) — not used by `real_regression` | Historical harness, not gold |

## 3. Root causes
- Flatpak sandbox isolates toolchain (`/app/bin` only). `real_regression.sh` expects native `gcc/g++` in PATH. Host toolchain works via `flatpak-spawn --host` (verified `gcc 16.2.1`, `as 2.47`, `ld 2.47`). Container-side verification must use host-spawn or install SDK extension.

## 4. Changes made
- No code changes for B (fixes were in A). Verified `dhad` drivers already use `command -v gcc/g++` checks and `cmake --build` fallbacks.
- Rebuilt `DAAD/build/daad` with XMM fix, rebuilt `Compiler/build/bin/daad-compiler` (584K), `CPU/dhad_cpu`/`dhad_asm` present.

## 5. Files changed
- (none new) — verification only. Previous `backend_interface.c` fix already included.

## 6. Tests added
- None new; used existing `real_regression.sh` 6 sections.

## 7. Commands executed
```bash
flatpak-spawn --host bash -c "gcc --version; g++ --version; as --version; ld --version"
flatpak-spawn --host bash -c "cd /home/... && bash Tests/real_regression.sh"
flatpak-spawn --host bash -c "cd DAAD/build && ctest -V | tail"
flatpak-spawn --host bash -c "cd CPU && ./run_tests.sh | tail"
# manual float checks (see PHASE_A_REPORT §7-8)
```

## 8. Real execution results
```
== Dhad real regression == ROOT=/... TMP=/tmp/dhad_regress.XXXXXX
-- [1] Compiler track
  PASS compiler:01_hello (مرحبا بالعالم!)
  PASS compiler:02_arithmetic (15)
  PASS compiler:03_conditions (أنت بالغ)
  PASS compiler:05_functions (26)
  PASS compiler:06_factorial (120)
  PASS compiler:loops (5050)
-- [2] DAAD integer
  PASS hello42 (42) calc60 (60) branch (1) loop55 (55) multifunc (42) fact120 (120) fib13 (13) + invalid-S001
-- [3] DAAD float pretty (%.6f)
  PASS zero 0.000000, one 1.000000, negone -1.000000, pi 3.140000, frac 13.750000, addexpr 8.000000, lit 2.500000, funret 11.000000, recfloat 15.000000
-- [3b] Integer print + boundaries
  PASS plit0…bigadd 13 cases + overflow-L011
-- [4] DAAD-DHAD-CPU
  PASS loop15 (15) fact120 (120) via dhad_asm → dhad_cpu
-- [5] dhad driver
  PASS targets, build-linux-cpp, build-linux-daad (120), run-dhad (15), neg-mac (exit 3), neg-dialect (exit 3); windows skipped (mingw missing, expected)
-- [6] CPU suite
  PASS suite-102 (0 fail)
== RESULT: 46 passed, 0 failed ==
```
- **DAAD ctest**: 11/11 (45 ast, codegen, semantic 157/0, stress 34/0)
- **CPU**: 102/102 (T1-T17: ALU, LDRI 0x0040/FF/0100, CMP E4-E7, interrupts, .org/.align/.include)
- **Compiler**: `g++ -fsyntax-only` for 11 headers + 6 examples; real `g++ -std=c++20` links and runs (verified via driver)
- **Exit codes**: `factorial 5→120`, `loop 1..10→55`, `fib 7→13` via `gcc -nostartfiles` + `syscall 60` checked

## 9. Known limitations
- Windows `PE` is `generated` (structure verified via `file`/`wixl`) but not booted on real Windows in this env (requires physical Windows 7/10 host).
- `run_all_tests.sh` still portability-broken (hardcoded HOME) — not used as gate; `real_regression.sh` is gate.
- Host-toolchain dependency: inside pure flatpak without `--host` spawn, `gcc` not found → fails are environment, not code.

## 10. Remaining bugs
- None in verified pipelines. `TARGETS.md` correctly marks `windows-x64` as `AVAILABLE/generated` (not `runtime`), `macos/*, android, ios, arm, riscv` as `NOT_IMPLEMENTED`.

## 11. Next phase
PHASE C — Compiler Stabilization: produce true architecture map (`Lexer→Parser→AST→Semantic→IR→Optimizer→Backend`) and document frontend-neutral IR.

---
*Gate: BUILD→EXECUTE→CORRECT OUTPUT→EXIT CODE→NO CRASH — satisfied. Do not proceed if any real execution fails.*
