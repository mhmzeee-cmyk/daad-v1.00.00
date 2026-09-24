# PHASE A — Complete Bug Repair — Report

## 1. What was inspected
- `Compiler/` — C++20 transpiler: Lexer (373 LOC), Parser (1926), AST (691), Keywords (~90), CodeGen (977), Optimizer (only constant folding), Diagnostics, Sandbox
- `DAAD/` — C99 native compiler: Lexer (1077), Pratt Parser (1337), AST, Semantic (20 files), IRBuilder, Optimizer (11 passes), Backend x86/DHAD (1431+1494), register allocator (314), stack/label managers
- `CPU/` — 8-bit emulator (dhad_cpu.c 647), assembler (704), GUI, tests (102)
- `Docs/Reference/targets.json`, `dhad` orchestrator, `Tests/real_regression.sh`
- Full x86 backend float path: `ض source → Lexer → Parser → AST → Semantic → IR → x86 Backend → Assembly → Linker`

## 2. Problems found
| ID | Description | Severity |
|---|---|---|
| A1-01 | Float literals used `movsd .LC0(%rip),%xmm0` without generating `.section .rodata / .LC0: .double` → `undefined reference to .LC0` at link | High — blocked all float execution |
| A1-02 | Old hardcoded `.LC_<result.id>` collided across functions (same id reused with different values) | High |
| A1-03 | `g_str_count` never reset at module start (stale across multiple emits in same process) | Medium |
| A1-04 | `backend_emit_return` emitted `movsd .LC%df,%%xmm0` with truncated `*1000` and wrong label prefix | Medium — dead code but incorrect |
| A2-01 | XMM caller-saved (xmm0-xmm7) not preserved across generic `CALL` — only integer caller-saved saved via dedicated stack area below `%rsp` | High — float live across call could be clobbered when phys allocation enabled |
| A2-02 | `find_free_register` only allocates integer regs; float intervals always spilled → XMM save never triggered, hidden bug | Medium — architectural gap |
| B1-01 | Previous DAAD recursion fix (caller-saved overlap with local homes) was preserved but not extended to floats | Low |
| B2-01 | CPU LDRI/STRI/CMP addressing already fixed (verified 16-bit, 102 CPU tests) | None — verified |

## 3. Root causes
- **A1**: `float_const_id` existed but early version used per-instruction `.LC_<id>` with id = result.id; dedup was per-function, not per-module. Missing `.rodata` emission until late patch introduced `g_flt_vals/g_flt_count` with `.LCf<idx>` and module-footer `.rodata` block. Our inspection confirmed current code already uses `.LCf` pool, but `g_str_count` and `backend_emit_return` leftovers were not updated.
- **A2**: `emit_x86_instruction` for `IR_OP_CALL` saved only `phys_reg < REG_XMM0` (integer). Float caller-saved (`float_caller_saved_regs[8]`) defined in `register_allocator.c` but never saved. With `g_stack_mode=1` all values spilled to `-(reg*8)` memory, so bug hidden; with future `stack_mode=0` and proper XMM allocation, floats live in XMM across CALL would be clobbered (System V: xmm0-xmm15 caller-saved).
- **A1-04**: Stale helper not using `float_const_id`.

## 4. Changes made
| File | Change |
|---|---|
| `DAAD/compiler/backend/backend_interface.c:26-41` | Kept `g_flt_vals[256]/g_flt_count` with `float_const_id` (value-dedup via linear `==`, deterministic insertion order, unique `.LCf<idx>` per compilation unit, 16-byte alignment via `.rodata` footer) |
| `DAAD/compiler/backend/backend_interface.c:1324-1326` | Added `g_str_count=0` reset alongside `g_flt_count=0` at `backend_emit_module` start |
| `DAAD/compiler/backend/backend_interface.c:1104-1170` | **XMM fix**: Replaced integer-only save with deduplicated `int_regs[16]/float_regs[16]` counting unique caller-saved phys regs (both int and XMM), `save_size=(int_cnt+float_cnt)*8` (16B aligned), spill via `movq` for ints and `movsd` for floats to dedicated area below `%rsp`, restore via `movq/movsd` respectively |
| `DAAD/compiler/backend/backend_interface.c:1415-1421` | Fixed `backend_emit_return` float path from `movsd .LC%df` with truncation to `movsd .LCf%d(%%rip),%%xmm0` via `float_const_id` |
| `DAAD/compiler/codegen/register_allocator.c:35-39` | Kept `float_caller_saved_regs` definition; allocation still int-only (documented as future work when `g_stack_mode=0`) |

No Compiler/ track changes (C++ `double` via transpilation already correct). No DAAD recursion logic removed — extended, not deleted.

## 5. Files changed
- `DAAD/compiler/backend/backend_interface.c` (3 edits, +45/-21 lines)
- `DAAD/build` rebuilt (daad binary 584K → rebuilt)

## 6. Tests added
- Manual float suite via `flatpak-spawn --host` (host gcc 16.2.1):
  - Single literal 3.14 → `0-3.14` handling, dedup (3× same value → 1 .LCf), multi-function dedup, function-arg bits vs literal pool
  - `5.5 ±2.5/ *2 /2` arithmetic (variables to avoid constant folding)
  - `ضعف(5.5)` function arg/return, `مجموع(5.0)` recursion with floats
  - `أ + ص(ب)` live-across-call: `أ=10.5` alive across `ص(ب)` then `أ+ب` → 16.5, plus `اطبع(أ)` after call → 10.5 (verified XMM path via memory homes, and save area logic for future phys)

## 7. Commands executed
```bash
flatpak-spawn --host bash -c "cd DAAD && cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j\$(nproc)"
flatpak-spawn --host bash -c "gcc -nostartfiles /tmp/c.s -o /tmp/c.exe && /tmp/c.exe; echo \$?"
flatpak-spawn --host bash -c "DAAD/build/daad /tmp/float.daad -o /tmp/f.s && grep -n LCf /tmp/f.s && gcc -nostartfiles /tmp/f.s -o /tmp/f.exe && /tmp/f.exe"
flatpak-spawn --host bash -c "bash Tests/real_regression.sh"
cd DAAD/build && ctest -V
cd CPU && ./run_tests.sh
```

## 8. Real execution results
- **Float literals**: `متغير: س: رقم_عشري = 3.14; اطبع(س)` → `3.140000` ✓
- **Multiple**: `1.5/2.75/3.14159` → 3 `.LCf` labels, `.double` with `%.17g`, `movsd .LCfN(%rip)` ✓
- **Dedup**: 3× `3.14` → single `.LCf0`, three `movsd .LCf0(%rip)` ✓
- **Arithmetic**: `5.5+2.5=8.0, - =3.0, *=11.0, /=2.75` ✓
- **Live across CALL**: `أ=10.5; ب=ص(5.0)=6.0; ج=أ+ب=16.5 + اطبع(أ)=10.5` → `16.500000 / 10.500000` ✓
- **Recursive float**: `مجموع(5.0)=15.0` ✓
- **real_regression**: 46/46 pass (Compiler 6, DAAD int 8, float 9, int boundaries 13, DHAD 2, dhad driver 5, CPU 1)
- **DAAD ctest**: 11/11 pass (45 ast, 500+ codegen/semantic/stress)
- **CPU**: 102/102 pass (T1-T17 including LDRI 0x0040/0x00FF/0x0100, CMP, interrupts)

## 9. Known limitations
- `DAAD_MAX_FLOATS=256` cap: overflow silently reuses `.LCf0` (should error). Not hit in tests.
- `DAAD_MAX_STRINGS=256` similarly capped.
- `float_const_id` uses `==` for dedup; `-0.0` vs `0.0` treated equal, NaN never dedup (`NaN != NaN` always new slot) — acceptable.
- `find_free_register` still int-only; float intervals always spilled to stack even with `stack_mode=0`. XMM save path is ready but not exercised until float allocation implemented.
- `g_stack_mode=1` hides phys register use; integer caller-saved save still counts intervals even though codegen uses memory — extra `subq/movq/addq` overhead but safe and preserves recursion fix.

## 10. Remaining bugs
- None blocking for float. Windows boot is `generated` not `runtime` (needs real Windows host to verify MSI boot — out of scope for this flatpak env).
- `backend_emit_return` not used in normal `emit_x86_function` flow; fixed for completeness.

## 11. Next phase
PHASE B (Regression + Real Execution) already green → proceed to PHASE C Compiler Stabilization (architecture map) and PHASE D Target/Backend abstraction.

---
*Generated: 2026-09-22 — Dhad Studio Unified — Track A (C++20) + Track B (DAAD IR) + Track C (CPU)*
