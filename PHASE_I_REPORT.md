# PHASE_I_REPORT.md — Remaining Bugs → Hardening → Native Target Foundation

## 1. What was inspected
- `DAAD/compiler/backend/backend_interface.c` اطبع path (N2), all int-const emission sites (N3),
  `main.c` helper emission (float runtime), lexer int parsing + gates (N3), `ir_builder.c`
  call-result typing (float dispatch), CPU ISA E0–E7 users (I.5), all four CLIs (I.8).

## 2. N2 investigation
`اطبع(<const>)` fell into the generic non-label branch doing `LOAD_TO(id)` on a constant's
meaningless `.id` → garbage slot → silently corrupted output (e.g. `اطبع(123456)` → 103800885784880).
`اطبع(<int-reg>)` was correct; `اطبع(<float-reg>)` printed raw bits (no pretty path).

## 3. N2 root cause
Missing operand-kind dispatch in `IR_OP_CALL/"اطبع"` + call-result temps hardcoded `i64`
(`ir_builder.c` NODE_CALL_EXPRESSION), so float call results couldn't be distinguished either.

## 4. N2 fix
- Backend اطبع now dispatches: CONSTANT_INT → `movq $imm,%rdi` + int helper;
  CONSTANT_FLOAT → `movabs bits` + float helper; REGISTER+f64/f32 type → float helper;
  other REGISTER → legacy int path (unchanged).
- `ir_builder.c`: call-result temp inherits the callee's DECLARED return type
  (`find_function_return_type` over program AST; builtins/unknown → i64 legacy).
  One struct field (`program_root`) + one static helper + one line at the call site.
  No parser/semantic changes.

## 5. N3 investigation
Contract found in code: lexer `strtoll` + `ERANGE` → Arabic `L011` (int64 literals valid).
Two gaps: (a) 10 backend sites used imm32/mem-imm encodings → GAS errors beyond ±2³¹
(LOUD, but English); (b) `main.c` never checked lexer errors → `99999999999999999999`
compiled and printed clamped LLONG_MAX **silently** (proven: output 9223372036854775807, exit 0).

## 6. N3 root cause
(a) x86 encoding limits (`mov $-imm,mem`, `alu $imm`) assumed 32-bit; (b) missing lexer-error
gate in `main.c` (same class as the Phase-1 semantic gate).

## 7. N3 fix
- `fits_i32` + `emit_movabs_rcx` scratch convention (same rax/rcx scratch norm as the file):
  MOV/ADD/SUB/IMUL/AND/OR/XOR/NEG/NOT/CMP/STORE/STORE_ELEMENT now emit imm32 fast path
  or movabs-via-rcx. Full int64 VALID everywhere.
- `main.c`: lexer-error gate (`lexer_has_errors` → print → exit 1), mirroring parser gate.
- Result: ≤int64 VALID at all boundaries; beyond → Arabic L011, never silent.

## 8. Float pretty-print investigation
Representation: f64 bits in int homes/regs (backend convention). Runtime had int-only helper.
Contract defined: `%.6f` (finite, int64-fitting), `nan`/`inf`/`-inf`, newline-terminated like int.

## 9. Float fix
- New `__daad_print_float` runtime helper emitted by `main.c` (bit ops + cvt + div loop,
  rodata `.Lfp_1e6/.Lfp_half`, carry handling, INT64-saturation documented).
- Backend routes float const/reg print operands to it (see §4).
- Bonus hardening in touched MOV path: int MOV reg→reg used `STORE_FLOAT` (stored stale xmm0
  instead of loaded rax) → corrected to `STORE`. Proven unreachable-or-fixed; outputs unchanged.

## 10. E1-E3 investigation (NO ISA CHANGE, as instructed)
Users today: assemblers emit `cmp r`→`E0|r`, EI/DI/RETI→E1/E2/E3; `main.c` decodes E0,E4–E7=CMP +
E1–E3=interrupts; `dhad_cpu.c` (GUI) decodes E0–E7=CMP + F3–F5=interrupts → hand-written
`cmp s1–s3` DIVERGES between CLI and GUI; DAAD backend avoids s1–s3 (dhad_backend.c:810).
Options: (A) full Phase-12.5 relocation in main.c+assemblers (unifies; invalidates stored
E1–E3 bins; needs interrupt-test migration); (B) assembler LOUD reject of `cmp s1/s2/s3`
(keeps compat, documents constraint); (C) dual-decode — REJECTED (fragile).
Decision deferred to a dedicated ISA-migration phase.

## 11. Target architecture implementation
- `Docs/Reference/targets.json`: machine-readable registry mirroring TARGETS.md
  (linux-x64/windows-x64/dhad-cpu AVAILABLE; macos×2/android/ios NOT_IMPLEMENTED).
- `./dhad` (repo root): orchestration-only driver (`targets|build|run`) dispatching to the
  existing CLIs; honest exit-3 errors for dialect/target mismatches and unbuilt targets.
  Verified: linux ELF run, windows PE build, dhad-cpu build+run, 3 negative paths (exit 3).

## 12. Files changed
`DAAD/compiler/main.c` (+float helper+rodata, +lexer gate),
`DAAD/compiler/backend/backend_interface.c` (print dispatch, int64 widening ×12 sites,
MOV-STORE hardening, helpers), `DAAD/compiler/ir/ir_builder.{h,c}` (program_root +
return-type propagation), `Tests/real_regression.sh` (new int/float-pretty sections),
`Docs/Reference/{LANGUAGE_CONTRACT.md,targets.json}`, `./dhad`,
`PHASE_{C,D,E,FGH,I}_REPORT.md`, `Demo/build-windows.sh`, evidence dirs.

## 13. Tests added
- `real_regression.sh`: 13 int-print/boundary + 9 float-pretty + L011 negative (now 40 cases).
- Ad-hoc matrices (kept in /tmp logs): float 13/13 bit-exact (pre-pretty), XMM 6/6, boundaries.

## 14. Commands executed
`./dhad targets|build|run …`, `bash Tests/real_regression.sh`, `ctest`, `CPU/run_tests.sh`,
`Tests/run_all_tests.sh`, boundary probes (`0,±1,±2³¹,±2³¹±1,5e9,±2⁶³⁻¹`, overflow literal),
pretty probes (`0.0,1.0,-1.0,3.14,13.75,8.0,11.5,5.14`, literal/return/nested).

## 15. Real execution results
`اطبع(42)`→42, `اطبع(123456)`→123456, max64→9223372036854775807, overflow→L011/exit-1,
`3.14`→`3.140000`, `ضعف(5.5)`→`11.000000`, recursive float→`15.000000`,
`./dhad run loop5.daad --target=dhad-cpu`→15 HALTED, PE32+ hello/factorial (KERNEL32+msvcrt).

## 16. Regression results
`real_regression.sh` **40/40** · `ctest` **11/11** · CPU **96/96** · harness **125/125**.
Int programs emit zero new bytes when values fit imm32 (fast paths preserved).

## 17. Windows status
`GENERATED / PE VERIFIED` (unchanged honesty level): PE validity+imports machine-proven;
first boot on real Windows still pending (no Windows/Wine here).

## 18. Remaining limitations
int64 saturation display for |float|>9.2e18; float-const-CMP uses int path (same-sign
positives only); n-ary `+`, tatweel ids, GOTO, mutual recursion, ARM/RISCV, GUI, mobile.

## 19. Remaining bugs
None open in scope. Next candidates: float-const-CMP typing, E1–E3 migration, N2-style audit
of other CALL builtins with const operands.

## 20. Next phase
Interview-machine acceptance (PE boot, MSI), then E1–E3 migration, then macOS (needs Apple toolchain).
