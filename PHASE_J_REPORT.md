# PHASE_J_REPORT.md — ISA Stabilization → Unified Build → Cross-Platform Foundation

## 1. Baseline
`d5e9b95`, tree with prior fixes; J.0: 40/40 + 96/96 + 11/11 + 125/125 GREEN → proceeded.

## 2. ISA inventory
Grep-built across CPU/DAAD/Tests/Examples/Docs for E0–E7/F1–F5: encoders (both assemblers),
decoders (`main.c` vs `dhad_cpu.c`), DAAD emitter, tests, stored `.bin` files.
Key finds: (a) 5 orphaned `ldri_*.ضasm` spec-conformant fixtures (T26a) never wired to any suite;
(b) zero suite files use `cmp s1–s3`; (c) stored `.bin`s predate LDRI (regenerable, unrelied-upon).

## 3. E1-E3 decision
**OPTION B**: freeze ISA + LOUD assembler reject of `cmp s1/s2/s3` (Arabic diagnostic naming
0xE1–E3 as interrupt-reserved + suggesting s0/s4–s7), in BOTH assemblers. Rationale: zero users
of the ambiguous form (backend avoids it); relocation (A) would invalidate stored bins +
interrupt tests for no current benefit; dual-decode (C) rejected as nondeterministic.
Full migration design in `PHASE_J_ISA_MIGRATION_PLAN.md` (DESIGN COMPLETE, IMPLEMENTATION DEFERRED).

## 4. Float CMP investigation
Two stacked defects: (i) CONSTANT_FLOAT `operands[0]` never materialized (read return-address
slot — silently wrong, masked when garbage was a tiny denormal); (ii) `setl/setg` after
`ucomisd` architecturally wrong (`<` always false). Fixed: const-dispatch for operands[0] in
ADD/SUB/MUL/DIV/CMP + unsigned `setb/seta/setbe/setae` on float path (int path untouched).
Matrix: 13 cases (`<,>,==,!=,<=,>=`, const/const, var/const, const/var, var/var, negatives,
zero, + int control) — ALL PASS. NaN: deterministic x86-UCOMISD behavior, language-UNSPECIFIED (contract).

## 5. Builtin audit
- اطبع: const-int ✓ const-float ✓ var-int ✓ var-float ✓ call-int/float ✓ nested ✓ string ✓
- ادخل: S008 arity ✓, int read ✓ (`S007` correctly rejects strings)
- افتح/اغلق: paths+modes work after string fix ✓ (valid handle, file created)
- اكتب/اقرأ_ملف: remapped to documented `(file,buf,size)` → `fread/fwrite(buf,1,size,f)` ✓
  (found + fixed: previously scrambled registers + missing stream arg)
- احجز/حرر/غيّر_حكم(شدة) ✓ (spelling verified byte-level: shadda required, S001 otherwise correct)
- Roundtrip integrity: 123456789 → LE bytes on disk → 123456789 ✓
- Needed enablers (same class, minimal): string table WITHOUT forced `\n` (print output
  byte-identical via dedicated newline write) + `&var` returning the alloca address
  (was loading content → segfault).
- Out of scope (pre-existing, documented): array alloca sizing/decay, `test_arrays_structs` link failure.

## 6. Target registry changes
`targets.json`: ADDITIVE `proof`/`proof_detail` (`runtime` vs `generated` vs `none`);
status values unchanged (consumers unbroken); added `linux-x64-cpp` (explicit Track-A Linux).

## 7. Target driver architecture
`./dhad` rewritten around `driver_*()` functions (validate→compile→link→verify):
`driver_cpp` (shared C++ emission stage — explicitly NOT called a formal IR),
`driver_linux_cpp`, `driver_windows` (PE check), `driver_daad`, `driver_dhad_cpu`.
Main flow holds no target-specific code beyond the dispatch table. No compiler rewritten.

## 8. Windows driver
Same proven steps (now inside `driver_windows`); `Demo/build-windows.sh` KEPT (not deleted).
`hello.ض`/`02_arithmetic`/`06_factorial` → static PE32+ (KERNEL32+msvcrt only), re-verified.

## 9. Linux driver
Preserved exactly: Track-B SysV + custom `_start` + `gcc -nostartfiles`; Track-A `.ض→g++→ELF`
kept as the distinct `linux-x64-cpp` id (backward compatible: `.ض+linux-x64` still works).

## 10. DHAD driver
Preserved: `.daad → --target=dhad → dhad_asm → .bin → dhad_cpu`; dialect mismatch → exit-3
message (legacy `.ضasm` = internal format, `.ضب` = nonexistent — documented truthfully).

## 11. Files changed
`CPU/{run_tests.sh,tests/cmp_s4.ضasm,src/main.c,src/dhad_asm.c}` (+rebuilt tracked binaries),
`DAAD/compiler/backend/backend_interface.c` (float const-op0, ucomisd setcc, string table,
fread/fwrite mapping), `DAAD/compiler/ir/ir_builder.c` (`&var` address),
`Docs/Reference/{ISA_COMPATIBILITY.md,targets.json,TARGETS.md,BUILD_PIPELINE.md,LANGUAGE_CONTRACT.md}`,
`./dhad`, `Tests/real_regression.sh`, `PHASE_J_ISA_MIGRATION_PLAN.md`, `PHASE_J_REPORT.md`.

## 12. Tests added
CPU T17 (5×LDRI, incl. 0x0100 16-bit proof) + T4h (CMP s4); regression +7 `./dhad` cases
(targets/build×3/run/negatives×2) → suite now **47**.

## 13. Commands executed
`bash Tests/real_regression.sh`, `ctest`, `CPU/run_tests.sh` (CWD=CPU), harness,
`./dhad targets|build|run` (8 combos incl. 3 honest refusals), float-CMP matrix (13),
builtin probes (print/input/file/alloc), `file`/`objdump -p` (PE).

## 14. Real outputs
`42/15/120` (CPU), `3.140000/11.000000/15.000000` (float), `123456789` file roundtrip,
`fopen` handle nonzero, `210` challenges (server untouched), PE32+ imports.

## 15. Regression results
**47/47 · 11/11 · 102/102 · 125/125.** Small-value codegen byte-identical (fast paths kept).

## 16. Windows status
`GENERATED / PE VERIFIED` (unchanged, honest): structure+imports machine-proven; boot pending.

## 17. macOS status
`NOT_IMPLEMENTED` + new `blocked` framing ready (no Apple toolchain here — no code written).

## 18. Android status
`NOT_IMPLEMENTED` (no NDK/SDK; no fake APK — nothing created).

## 19. iOS status
`NOT_IMPLEMENTED` (no Xcode/signing; nothing created).

## 20. Remaining issues
Array subsystem (sizing/decay, broken arrays test); float-const-CMP edge: mixed int/float
operand typing relies on builder inference (verified for tested shapes); E1–E3 migration
deferred by design; `dhad_asm` exit-code-on-error quirk (pre-existing, error-recovery design).

## 21. Next phase
Interview-machine acceptance (PE boot + MSI install/run) → E1–E3 migration execution →
macOS when toolchain exists. Definition of done (10 items) ALL met, architecture expandable.
