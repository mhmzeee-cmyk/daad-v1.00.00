# DEEP_BUG_INVESTIGATION.md — Dhad Studio Unified

> **المنهج:** reproduce → prove root cause (gdb/objdump/spec) → minimal fix → re-run same failure → regression. No mocks, no fake success, no test tampering.
> **Baseline:** `main` @ `ff5999d` + 1 prior fix (`Studio/desktop-app/main.js` paths) + prior evidence in `Operational-Evidence/` (preserved, extended with `B1-Recursion/` + `B2-DHAD-Loop/`).
> **Tools:** gdb 17.1, objdump, gcc 15.2, cmake 4.2.3 — all real execution.

---

# Executive Summary

| Bug | Title | Status |
|---|---|---|
| B1 | DAAD x86 recursion segfault (factorial 139) | **CONFIRMED → ROOT CAUSE CONFIRMED → FIX IMPLEMENTED → FIX VERIFIED** |
| B2a | DHAD `stri`/`ldri` unknown to assembler/CPU (loop → 0) | **CONFIRMED → ROOT CAUSE CONFIRMED → FIX IMPLEMENTED → FIX VERIFIED** |
| B2b | `cmp s1–s7` no-op in `dhad_cpu` binary (loop → 0, second cause) | **CONFIRMED → ROOT CAUSE CONFIRMED → FIX IMPLEMENTED → FIX VERIFIED** |
| N1 | Float constants link failure (`.LC_` undefined) | **CONFIRMED (pre-existing, proven via stash) → NOT FIXED (out of scope, larger backend work)** |
| N2 | `و` unusable as identifier (reserved AND) | **NOT A BUG (language design; parser correctly rejects)** |

**Regression:** DAAD ctest 11/11 (both build dirs) + 12/12 valid runtime cases (incl. fib double-recursion) + CPU suite 96/96 + Compiler 6/6 + unified harness 125/125 + Studio health/login + MSI static. Zero failures introduced.

---

# Baseline

- `git status` at start: `M Studio/desktop-app/main.js` only (+ untracked docs/evidence).
- Binaries rebuilt during work (now also modified in tree): `CPU/dhad_asm`, `CPU/dhad_cpu`, `CPU/dhad_gui` — they embed the fixes; listed honestly in §Files Modified.
- Prior claims re-verified, not assumed: factorial segfault reproduced (139), DHAD loop 0 reproduced, then root-caused independently.

---

# Confirmed Bugs (map)

```text
B1: factorial.daad → segfault 139 (x86)
B2a: DHAD stri/ldri → "تعليمة غير معروفة" + skipped → loop 0
B2b: cmp s4 (E4) → silent no-op → loop 0 (second independent cause, found while verifying B2a fix)
N1: float const link error (pre-existing, out of scope)
```

---

# Bug B1 — DAAD Recursion Segfault

## Reproduction (REAL, same commands)
```bash
/tmp/opencode/daad-build/daad DAAD/factorial.daad -o /tmp/opencode/repro_fact.s  # EXIT 0
gcc -nostartfiles /tmp/opencode/repro_fact.s -o /tmp/opencode/repro_fact.exe     # EXIT 0
/tmp/opencode/repro_fact.exe                                                     # → Segmentation fault, EXIT 139
```
Evidence: `Operational-Evidence/B1-Recursion/fact_BEFORE.s`, `gdb_fault.txt`.

## Root Cause (PROVEN, not "register collision" hand-waving)
- gdb: `SIGSEGV at مضروب+31: mov %rcx,(%rax)` with `rax=0x7fffff7feff8` (unmapped after deep recursion), backtrace = infinite `مضروب` frames — base case never taken.
- objdump + source correlation (`backend_interface.c:934-961`, generic `IR_OP_CALL` path):
```text
IR local homes:      -0x10 = pointer, -0x18..-0x48 = locals (n, n-1, ...)
call-site "save":    movq %rdi, -0x10(%rbp)   ← OVERWRITES pointer with saved rdi
                     movq %rsi, -0x18 ...     ← OVERWRITES locals
arg load AFTER save: movq -0x38(%rbp), %rdi   ← reads CLOBBERED slot (gets 5, not 4)
  → every recursive call receives n=5 → infinite recursion → stack exhaustion
  → eventually pointer slot holds garbage → mov (%rax) faults
```
- Precise chain: `reg X (caller-saved phys, e.g. %rdi)` → saved at stack offset `-(ir_reg*8)` = home of live local Z (pointer) → recursive `callq` with corrupted arg → corrupted value → invalid memory dereference.
- ABI context: SysV (rdi/rsi/rdx/rcx/r8/r9 args, caller saves caller-saved). Locals live in `-(1..max_reg)*8`; spill area `-(max+1+..)*8`. The save loop reused local homes as scratch — that is the defect (allocation/frame-generation/call-lowering bug in call lowering, not allocator).
- Scope note: affects ANY call with live caller-saved locals (multifunc passed only because main had no live locals). Recursion makes it fatal.

## Evidence
- `fact_BEFORE.s` lines: `movq %rdi, -16(%rbp)` … `movq -56(%rbp), %rdi; callq مضروب` (arg from clobbered slot).
- `gdb_fault.txt`: faulting `mov %rcx,(%rax)`, rax wild.
- `file`/`readelf` not needed; objdump listing captured in investigation log.

## Fix (MINIMAL, same predicate, new mechanism)
- File: `DAAD/compiler/backend/backend_interface.c`, generic `IR_OP_CALL` else-branch (~lines 933-961 → now expanded).
- Change: caller-saved live regs are preserved in a DEDICATED stack area (`subq $save_size,%rsp; movq %reg,N(%rsp)` … `addq`) instead of IR local homes. `save_size` padded to 16B (ABI alignment). Order preserved (save → args → call → result → restore). Int regs only (`< REG_XMM0`): old code emitted invalid `movq %xmmN,mem` for floats — now skipped (float-live-across-call stays a known limitation, but no invalid asm).
- Why this fix: uses the file's own established pattern (اطبع/ادخل already push/pop around calls), touches only call lowering (not allocator, not ABI, not frame layout), zero effect when no live caller-saved regs (`save_size=0` → no instructions emitted).
- No factorial special-case, no test changed, no feature disabled.

## Verification (SAME failure re-run + extensions)
- `factorial.daad` → EXIT **120** (was 139). New asm shows `subq $64,%rsp; movq %rdi,0(%rsp)…` (`fact_AFTER.s`).
- 12/12 valid runtime cases: hello42, calc60, branch1, loop55, multifunc42, div5, mul42, fact120, fact(0)=1, **fib(7)=13 (double recursion)**, nested_calls=40, fact(6)=208. (`six_args` of mine rejected at parse: `و` is AND keyword — correct behavior, 3-arg call → 6 verified instead.)
- `ctest` 11/11 in BOTH `/tmp/opencode/daad-build` and `DAAD/build`.

---

# Bug B2 — DAAD → DHAD → CPU Loop (two independent causes)

## Reproduction
```bash
daad loop(1..5) --target=dhad -o loop.ضasm   # EXIT 0
./CPU/dhad_cpu loop.ضasm                      # → 8× "تعليمة غير معروفة 'stri/ldri'", النتيجة 0 (expect 15)
```

## Root Cause B2a (ISA gap, proven by repo-wide grep + spec)
| Instruction | Definition | Assembler | Decoder | Execution | Tests |
|---|---|---|---|---|---|
| STRI | ✅ backend doc + `isa_constants.h:435` (0xF2) + `HARDWARE_SPEC:428` | ❌ MISSING (`dhad_asm.c`, `main.c` inline) | unified ⚠️ wrong (8-bit stub) / inline ❌ missing | unified ⚠️ / inline ❌ | ❌ (loop fails) |
| LDRI | ✅ `isa_constants.h:434` (0xF1) + `HARDWARE_SPEC:415` | ❌ MISSING | same | same | ❌ |

- Spec encoding (authoritative, two sources agree): **4 bytes `F0 F1/F2 OPERANDS LOREG`**, OPERANDS=[dst/src:4][hi_reg:4], LOREG=byte&0x0F; behavior `addr=(hi<<8)|lo` full 16-bit from register values.
- Nothing new invented: implemented exactly the spec. Mnemonics: `stri`/`ldri` (what backend emits) + Arabic spec names `خزن_غيرمباشر`/`اقرأ_غيرمباشر`.

## Root Cause B2b (found while verifying B2a: loop STILL 0 after assembler fix)
- Direct test: `cmp s4` with equal operands → no jump (99 instead of 1); `cmp s0` works.
- Cause: assembler emits `cmp r` as `F0 (E0|r)` (both assemblers), but `main.c` decoder had only `case 0xE0` — E4-E7 fell through to `default` (silent no-op). Unified core already handles E0-E7.
- Constraint (NOT changed): E1-E3 are EI/DI/RETI in `main.c` (backend works around it per `dhad_backend.c:810`: cmp scratch restricted to s0/s4-s7). Minimal fix = add E4-E7 only; full Phase-12.5 relocation (EI→F3) left untouched to avoid breaking interrupts.

## Evidence
- `Operational-Evidence/B2-DHAD-Loop/dhad_loop.ضasm` (backend output with `stri/ldri` + `cmp s4`).
- Direct `cmp_s4eq` before/after (99 → 1); `stri_direct` roundtrip (77); `mem1` (65).
- My own flawed hand-tests (slot-6 read, E2-as-CMP) documented in log as test-design errors, not emulator bugs — corrected versions pass.

## Fix (4 small edits, all to spec)
1. `CPU/src/dhad_asm.c`: +LDRI/STRI mnemonic blocks after `بدل/swap` (~30 lines, `asm_*` helpers).
2. `CPU/src/main.c` assembler: identical blocks (`emit/read_token/is_reg` variants).
3. `CPU/src/main.c` decoder: +`case 0xF1` (LDRI) +`case 0xF2` (STRI), 4-byte 3-reg, full 16-bit addr; +`case 0xE4-E7` (CMP).
4. `CPU/src/dhad_cpu.c`: 0xF1/0xF2 rewritten from 8-bit stub to spec (4-byte, full 16-bit) so GUI/live agree with CLI.
- Why: completes the documented ISA instead of working around it; parser-only acceptance would still execute wrong.

## Verification
- DHAD integration now: add1p2→3, vars→30, branch→1, **loop→15** (was 0), func→42, hello→42 — **zero assembler errors** (was 6-32 warnings).
- Memory addressing: roundtrip 77/65 ✅, frame arith 7+8=15 ✅, store-in-loop + load-after-loop (s4-cmp) =15 ✅.
- CPU regression: `run_tests.sh` **96/96** (incl. T6 interrupts — E1-E3 preserved). `make gui` rebuilt with unified fix, launches under xvfb EXIT 0.

---

# Newly Discovered Bugs (during investigation)

- **N1 — Float constants link failure: CONFIRMED pre-existing.** `ضعف(5.5)` program generates `.LC_` references with no rodata → `undefined reference`. Proven pre-existing via `git stash` + rebuild + same failure on ORIGINAL code. Untouched by B1/B2 (different code path: constant emission). **Next fix candidate** (needs rodata emission in x86 backend).
- **E1-E3 vs CMP-s1/s2/s3 conflict: DOCUMENTED CONSTRAINT, not fixed.** Backend already avoids (dhad_backend.c:810). Direct `قارن s1` in hand-written asm executes DI — surprising but consistent with old encoding; full relocation deferred (blast radius: interrupts + ISR tests).
- **Invalid-reg silently assembles** (`س99` → 2 bytes, ACC=0) and **empty file spins 100000 cycles** (`يعمل`): observed, harmless (no crash), recorded; not fixed (assembler diagnostics improvement, low priority).

---

# Environmental Blockers (not project bugs)

- MSI Install→Run: no Win7/Wine/VM (static verification only — SHA/PE/extract/correspondence all pass).
- Electron launch: binary not installed (paths fixed + verified via existsSync).
- Browser click-automation: Firefox profile lock (server + toolchain verified instead).
- No Java (Logisim GUI), XMM-live-across-call limitation (see B1 note).

---

# Regression Results (all REAL execution)

| Suite | Before fixes | After fixes |
|---|---|---|
| DAAD ctest (×2 build dirs) | 11/11 | **11/11** |
| DAAD runtime cases | 7 pass + 1 FAIL (recursion) + S001 | **12/12** (incl. fib double-recursion) |
| DAAD→DHAD→CPU | 5/6 exec (loop 0) | **6/6** (loop 15, zero warnings) |
| CPU `run_tests.sh` | 96/96 | **96/96** |
| Compiler 6 examples | 6/6 | **6/6** (untouched code) |
| Unified `Tests/run_all_tests.sh` | 125/125 | **125/125** |
| Server health/login | pass | **pass** (re-verified post-fix) |
| `jest authService` | 37/37 (prior phase) | not re-run (server untouched) |

---

# Remaining Risks

1. **N1 (float)** is the next real functional gap (any float-returning function fails at link). Fix = emit `.rodata` + `.LC_` labels in x86 backend (estimate: small, localized).
2. **E1-E3 encoding debt**: hand-written asm using `cmp s1/s2/s3` silently misbehaves; consider full Phase-12.5 relocation (EI/DI/RETI→F3/F4/F5) with interrupt-test migration.
3. **XMM caller-saved across generic calls** now skipped (no invalid asm, but value may not survive) — needs movsd save area when float-call tests exist.
4. **Rebuilt tracked binaries** (`CPU/dhad_*`) carry fixes — must be rebuilt on any fresh checkout (or better: gitignore them like other artifacts).
5. **Frame addresses assume S6=0xE0** (backend contract) — LDRI/STRI honor full 16-bit, so any future frame relocation works, but backend+CPU must stay in sync.

---

# Final Functional Status

- **B1 FIXED + VERIFIED** (recursion: fib/fact/nested all pass; was segfault).
- **B2a+B2b FIXED + VERIFIED** (DHAD loop 15 with zero warnings; was 0 with 8 errors).
- **No regressions** (every suite ≥ before).
- **Demo paths now fully green:** Demo B can include factorial; Demo C can include loop — the two previous "avoid" notes are lifted. MSI/Electron/browser-blockers unchanged (environmental).

---

# Bug B3 — FLOAT-RODATA (x86 float constants) — FIXED + VERIFIED

## Reproduction (before fix)
- `ضعف(5.5)` with `س * 2.5` → `movsd .LC_4(%rip), %xmm1` → `undefined reference to .LC_4` at link (no definition emitted anywhere; rodata block only handled strings).
- Deeper probe found the same class in 3 more backend-only paths: `STORE [ptr], <float>` (var init stored nothing → printed 0), `CALL f(<float>)` (arg dropped → callee got garbage), `RET <float>` (emitted nothing).
- Latent collision: two functions with different constants both referenced `.LC_4` (keyed by result.id) — a naive per-id definition would silently compute wrong values.

## Root Cause
x86 emitter referenced float-constant labels that no code ever defined, keyed them by non-unique result.id, and silently dropped float constants in STORE / CALL-arg / RET paths. (Parser/semantic/IR all correct — `--ir` shows `2.500000`/`5.500000` constants properly; ABI unchanged: f64 travels as bits in int regs/stack slots.)

## Fix (backend-only, one file: `DAAD/compiler/backend/backend_interface.c`)
1. Module-unique value-deduped table (`float_const_id`, `.LCf<idx>`); 6 `.LC_<id>` sites → `.LCf<idx>`.
2. `.section .rodata` + `.LCf<i>: .double %.17g` after functions (only when non-empty → int programs byte-identical), restoring `.section .text` for main.c's appended helpers/`_start`.
3. STORE / STORE_ELEMENT const-float via `movsd .LCf → (rax)`; CALL-arg / RET const-float via `movabs $bits` (exact, matches bits-in-int-reg ABI).
- No parser / semantic / ABI changes. No test modified.

## Verification (real DAAD dialect translations of the task programs)
- V1 `س+ص` (5.5+2.5) → prints exact 8.0 bits `4620693217682128896` ✅
- V2 `ضعف(5.5)` (5.5*2.5) → prints exact 13.75 bits `4623930179914301440` ✅ (was: link failure)
- V3 recursive float `مجموع(5.0)` → exact 15.0 bits `4624633867356078080` ✅
- Collision case (2.5 in one func, 1.5 in another) → `.LCf0=2.5`, `.LCf1=1.5`, chained `ضعف(4.0)→ثلاثة` = exact 11.5 ✅
- repo `test_float.daad` now links+runs (`المجموع:` + exact 5.14 sum bits + `س1 أكبر من س2` branch) ✅
- Regression: ctest 11/11 (×2 dirs), int programs emit zero rodata (`grep rodata = 0`), fact 120 / calc 60, CPU 96/96, harness 125/125, Compiler example, DHAD 42/15.
- Evidence: `Operational-Evidence/FLOAT-RODATA/` (4× `.s` + 4× `.output` with exact bit patterns).

## Newly noted (pre-existing, out of scope, NOT fixed)
- N2: `اطبع(<int-literal>)` prints garbage (constant operand mishandled in print path — int-side; verification uses register results, unaffected).
- N3: `movq $imm64,(mem)` assemble error for huge int constants (GAS limitation in int STORE path).
- A false trail was caught and documented: an early "wrong value" suspicion was the investigator's own hand-computed expectation; tool-computed bits (gdb/objdump/python struct) proved output exact. Lesson recorded: compute expectations with tools.
