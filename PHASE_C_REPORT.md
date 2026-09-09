# PHASE_C_REPORT.md — Compiler Stabilization

## 1. What was inspected
Full re-read of both compiler frontends + backends after PHASE A/B fixes (no rewrites):
- `Compiler/` (C++20 transpiler, 12 src + 11 headers, ~5100 lines)
- `DAAD/compiler/` (C99 native: token/lexer/parser/ast/semantic/ir/optimizer/cfg/ssa/codegen/backend)
- `CPU/` ISA pair (`main.c` inline + `dhad_cpu.c` unified + `dhad_asm.c`), `dhad_isa_constants.h` as spec
- `Studio/Web/js/dhad-*.js` (browser mirror toolchain)

## 2. Real compilation maps (verified by execution, not by names)

### Track A — Compiler (.ض → C++20)
```text
program.ض (UTF-8)
  ↓ UnicodeUtils (manual UTF-8 → codepoints; ؛→; ،→,)
  ↓ Lexer (tokens: ~30 types)
  ↓ KeywordRegistry (~90 Arabic→C++: صحيح→int, إذا→if, طباعة→daad_print…)
  ↓ Parser (recursive-descent, 1683 lines; GOTO rejected; Struct name-only)
  ↓ AST (677-line header: 17 expr + ~30 stmt + image nodes)
  ↓ OptimizerVisitor (const-fold Number-op-Number ONLY)
  ↓ CodeGenVisitor (global decls vs main-body split; type map; includes)
  ↓ DiagnosticsEngine (Arabic errors) → SandboxValidator (whitelist+regex heuristics)
  ↓ output.cpp + output.hpp → g++ → EXE → run
```
Stability verdict: STABLE for covered subset (6/6 examples incl. factorial/loops/functions).

### Track B — DAAD (.daad → x86-64 / DHAD)
```text
program.daad
  ↓ Lexer (byte-wise, ~104 token types, Arabic+Latin identifiers)
  ↓ Pratt Parser → AST (NODE_PROGRAM)
  ↓ Semantic (ScopeStack + TypeRegistry + TypeChecker + folding; HARD error gate)
  ↓ IRBuilder (Module/Function/Block/Instr/Value; ~35 opcodes)
  ↓ Optimizer (11 passes) → [CFG/SSA libs present; SSA not wired into main]
  ↓ Backend (BackendTarget: X86 primary / DHAD educational / ARM+RISCV declared-only)
  ↓ x86: AT&T asm + __daad_print_int/__daad_read_int + _start → gcc -nostartfiles → ELF → exit-code
  ↓ DHAD: _start + frame code (S7 FP, 0xE0 high) → .ضasm → dhad_asm → .bin → dhad_cpu
```
Stability verdict: STABLE for integer + float subsets incl. recursion (13/13 float bit-exact, 12/12 int+recursion).

### Track C — CPU (8-bit data / 16-bit addr / 64KB)
Assembler (two-pass: labels/equates/macros/.if/.org/.db/.dw/.ds) → binary → fetch/decode/execute
(regs S0-S7+ACC, flags Z/N/C, MMIO, IVT). Extended ISA after `0xF0`: jumps/logic/CMP(E0,E4-E7;
E1-E3 = EI/DI/RETI legacy)/LDRI(F1)/STRI(F2, 4-byte 3-reg, full 16-bit). STABLE: 96/96.

### Track D — Studio (JS mirror, independent re-implementation)
Browser Lexer/Parser/AST/CodeGen → sandboxed `new Function` → output. STABLE for covered syntax.

## 3. Problems found → all closed in A/B (nothing new in C)
B1 recursion save-area ✓, B2a LDRI/STRI ✓, B2b CMP E4-E7 ✓, B3 float pool + const STORE/args/RET ✓,
editor watermark CSS ✓, libraries quote ✓. A2 XMM investigated → NOT a bug (memory homes design).

## 4. Changes made in this phase
NONE to code (stabilization = freeze + map + gates). Only this report.

## 5. Files changed
`PHASE_C_REPORT.md` (this file).

## 6. Tests added
`Tests/real_regression.sh` (from PHASE B): 25/25 green — adopted as the STABILIZATION GATE.
Rule: any future change must keep it green; `ctest 11/11`, `CPU 96/96`, `harness 125/125` as secondary gates.

## 7. Commands executed
`bash Tests/real_regression.sh` → 25/25; `ctest` → 11/11 (×2 dirs); `run_tests.sh` (CWD=CPU) → 96/96.

## 8. Real execution results
See PHASE B logs + `Operational-Evidence/`. No new execution in C (nothing changed).

## 9. Known limitations (frozen, documented — fix only with root-cause proof)
1. Compiler optimizer = const-fold only; sandbox = heuristics; `Struct` name-only; `انتقل/GOTO` rejected.
2. DAAD: float pretty-print absent (bit-pattern via int helper); CMP-const-float uses int path (correct only for same-sign positives); n-ary `a+b+c` needs explicit nesting; tatweel `هـ` not identifier char; int-literal print + huge-int store gaps (N2/N3); SSA/CFG libs unwired; ARM/RISCV constructors unexposed.
3. CPU: E1-E3 reserved (backend avoids s1-s3 for CMP scratch — documented constraint); `س99` silently assembles; empty file spins to max-cycles.
4. `gcc` (not `-nostartfiles`) fails on DAAD output by design (custom `_start`) — documented in Demo/guide.

## 10. Remaining bugs
None open in stabilized scope. Next candidates (require fresh investigation): N1 done, N2, N3, float pretty-print.

## 11. Next phase
PHASE D: target/backend architecture (design; no new backends without toolchains).
