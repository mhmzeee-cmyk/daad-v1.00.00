# Phase 3 Batch 3 — Conditional Jump & Backend Memory Fix Report

**Date:** 2026-08-11  
**Status:** PASS

## Summary

Fixed three critical codegen bugs in `backend_interface.c` that broke all conditional branches (if/else, while, for, comparisons).

---

## Bug #1: Conditional Jumps Emitted Redundant `cmpq $0, %rcx`

**Root cause:** The conditional jump handler (`IR_OP_JE/JNE/JG/JL/JGE/JLE`) emitted a hardcoded `cmpq $0, %rcx` instruction to test the flag register, which **overwrote the flags** set by the preceding `IR_OP_CMP`. This meant every comparison result was lost before the branch was taken.

**Fix:** Removed the redundant `cmpq $0, %rcx` from the conditional jump handler entirely. The flags from `IR_OP_CMP` are now used directly by the branch instruction.

**File:** `compiler/backend/backend_interface.c:535-562`

---

## Bug #2: `IR_OP_CMP` Produced No Result Register

**Root cause:** `IR_OP_CMP` only set CPU flags via `cmpq` but never stored a boolean (0/1) result into the result register. This made the CMP result register hold uninitialized data. When combined with the conditional jump's redundant `cmpq $0, %rcx`, this was masked — but once Bug #1 was fixed, the CMP result register became the sole input to branch decisions, revealing this gap.

**Fix:** Added `sete/setne/setg/setl/setge/setle %al` + `movzbl %al, %eax` after the `cmpq` in the CMP handler, storing a boolean result into the register allocation result. Also converted the CMP operand loads from raw `SLOT(r)` to `LOAD_TO()` to respect the register allocator.

**File:** `compiler/backend/backend_interface.c:505-527`

---

## Bug #3: `IR_OP_STORE`/`IR_OP_LOAD` Ignored Register Allocation

**Root cause:** `IR_OP_STORE` and `IR_OP_LOAD` used `SLOT(r)` (stack slot) to load/store addresses, even when the register allocator assigned the register to a physical register (e.g., `%rdi`). This caused reads from uninitialized stack slots → STATUS_ACCESS_VIOLATION crashes in all control-flow programs.

**Fix:** Changed both handlers to use `LOAD_TO()` macro, which checks the register allocator first and loads from the physical register when assigned, falling back to the stack slot only when spilled.

**File:** `compiler/backend/backend_interface.c:571-598`

---

## Bug #4: `IR_OP_RET` Missing Fall-Through Epilogue Label

**Root cause:** Functions with `RET` in the then-branch of an if/else had no epilogue label, causing the else-branch's `jmp .L_funcname_epilogue` to reference a non-existent label → assembler error.

**Fix:** Added `jmp .L_funcname_epilogue` before the `RET` result store, and added the `.L_funcname_epilogue:` label before the function epilogue code in `emit_x86_function()`.

**Files:** `compiler/backend/backend_interface.c:384-396, 132`

---

## Bug #5: `movzbl %al, %rax` Assembler Error

**Root cause:** The `movzbl` (move zero-extend byte to long) instruction uses the `l` (32-bit) suffix. Using `%rax` (64-bit) as destination is invalid in AT&T syntax.

**Fix:** Changed to `movzbl %al, %eax` — zero-extending to `%eax` implicitly zero-extends to `%rax` in x86-64.

**File:** `compiler/backend/backend_interface.c:524`

---

## Conditional Jump Fix (Bug #6)

The initial fix removed the redundant flag test but didn't load operands properly. The IR `ir_inst_cond_jmp` stores both operands (left, right) and the jump targets (label_true, label_false). The x86 backend needs to:

1. Load left operand into `%rax` (handle both register and constant kinds)
2. Load right operand into `%rcx` (handle both register and constant kinds)
3. `cmpq %rcx, %rax`
4. Branch on the compare_op

**Fix:** Updated conditional jump handler to check `operands[].kind` before loading — using `LOAD_TO` for registers and `movq $imm` for constants.

**File:** `compiler/backend/backend_interface.c:535-562`

---

## E2E Results

| Metric | Before | After |
|--------|--------|-------|
| CTest (11 tests) | 11/11 PASS | 11/11 PASS |
| E2E PASS | 36 | 54 |
| E2E COMPILE_FAIL | 19 | 19 (all pre-existing) |
| E2E LINK_FAIL | 3 | 0 |
| if/else correct | 0/14 | **14/14** |
| Crashes (STATUS_ACCESS_VIOLATION) | 8 | **0** |

### Newly Fixed (18 tests):
- `if_true`, `if_false` — basic if/else branching
- `if_gt_t`, `if_gt_f`, `if_lt_t`, `if_lt_f`, `if_le_t`, `if_ge_t`, `if_ne_t` — comparison operators
- `cmp_eq_t`, `cmp_eq_false`, `cmp_gt_t`, `cmp_gt_f`, `cmp_lt_t`, `cmp_lt_f`, `cmp_ge_t`, `cmp_le_t`, `cmp_ne_t` — standalone comparison expressions

### Pre-existing Failures (unchanged, not codegen):
- **Parse errors (2):** `for_simple`, `for_compile` — FOR loop syntax not implemented
- **Semantic errors (17):** float arithmetic (2), memory/variable operations (7), while loops (6), mutual recursion (1), var_add (1)
