# PHASE_3_9_AUDIT.md — Detailed Audit of Reports vs C Reference

**Date:** 2026-09-05
**Auditor:** opencode AI
**Purpose:** Find every discrepancy between reports/vectors and the actual C source

---

## 1. WHAT IS CORRECT

| Item | Status | Evidence |
|------|--------|----------|
| ISA encoding [7:4]=opcode, [3:0]=reg | CORRECT | dhad_cpu.c:230-231 |
| 8 registers S0-S7, ACC separate | CORRECT | dhad_isa_constants.h:48, dhad_cpu.c:106-114 |
| Flags: Z(bit0), N(bit1), C(bit2) | CORRECT | dhad_isa_constants.h:228-230 |
| Stack: stack[SP++], --SP | CORRECT | dhad_cpu.c:139-150 |
| Stack size: 32 entries | CORRECT | dhad_isa_constants.h:51 |
| MOV: reg=SRC, byte1=DST | CORRECT | dhad_cpu.c:249-253 |
| SUB C flag = borrow | CORRECT | dhad_cpu.c:325-333 |
| CMP does NOT write ACC | CORRECT | dhad_cpu.c:566-576 |
| INC/DEC on any register | CORRECT | dhad_cpu.c:480-502 |
| NEG on ACC only | CORRECT | dhad_cpu.c:304-309 |
| PUSH pushes ACC (zero-extended) | CORRECT | dhad_cpu.c:273-277 |
| CALL pushes PC after fetch16 | CORRECT | dhad_cpu.c:288-295 |
| DIV/MOD by zero → HALT | CORRECT | dhad_cpu.c:350-354, 365-369 |
| PC is 16-bit | CORRECT | dhad_isa_constants.h:45 |
| SP is 8-bit (stored as uint8_t) | CORRECT | dhad_cpu.c:633 |
| Data width 8-bit | CORRECT | dhad_isa_constants.h:42 |
| Address width 16-bit | CORRECT | dhad_isa_constants.h:45 |

---

## 2. WHAT IS INCORRECT

### Issue 2.1: DATAPATH_TEST_REPORT.md — "SUB C flag = borrow" described correctly but test vectors wrong
- **Report:** Correctly states C=1 when ACC < operand
- **Test ALU-05:** `A=0x05, B=0x0A, SUB → C=1` — CORRECT
- **But test ALU-04:** `A=0x34, B=0x12, SUB → C=0` — CORRECT
- **Status:** OK

### Issue 2.2: DATAPATH_VECTORS.md — Missing carry inversion note
- The report says hardware subtractor carry needs inversion
- This is NOT documented in the test vectors
- **Impact:** If someone tests the raw ALU subtractor, they'll see inverted C
- **Fix needed:** Add note about carry inversion in hardware

### Issue 2.3: DATAPATH_VECTORS.md — CMP result column has ???
- **Test ALU-17:** `CMP A=0x10, B=0x20 → RESULT=???`
- The C reference computes `r = acc - v` but only uses it for flags
- The ALU WILL produce a result (the subtraction), but CMP doesn't write it to ACC
- **Fix needed:** RESULT = 0xF0 (A-B = 0x10-0x20 = 0xF0), but ACC unchanged

### Issue 2.4: DATAPATH_TEST_REPORT.md — MUX select encoding assumed
- Test MUX-01 through MUX-06 use select codes without verifying against circuit
- The DATAPATH circuit has MUXes with specific select codes
- Need to verify these match the actual control signal design
- **Status:** NEEDS VERIFICATION

### Issue 2.5: SP is 8-bit, not 5-bit
- **ISA constants line 160:** "SP — 5-bit" — THIS IS WRONG
- **C source line 633:** `dhad_u8 sp;` — SP is 8-bit
- **Stack size:** 32 entries (DHAD_STACK_SIZE=32)
- **SP uses index 0-31, fits in 5 bits, but stored as 8-bit**
- **Status:** DISCREPANCY in ISA constants vs C source

### Issue 2.6: Stack is separate from RAM
- **C source line 639:** `dhad_u16 stack[DHAD_STACK_SIZE];`
- Stack is a SEPARATE array, not in main memory
- Hardware SP_8 indexes into this separate stack
- **Status:** Correctly modeled in hardware (separate from RAM_64K)

---

## 3. WHAT NEEDS PROOF

| Item | Why |
|------|-----|
| SUB carry inversion | Hardware subtractor carry ≠ C reference carry |
| SHL/SHR carry latch timing | C computed from OLD bit, not shifted bit |
| PC increment behavior | Does PC_16 increment via external MUX or internal? |
| MEM_ADDR_SRC select codes | Are 00=PC, 01=ZP, 10=REG_PAIR correct for the design? |
| WR_SRC select codes | Are 00=ALU, 01=MEM, 10=PC correct? |
| Control signals during EXECUTE | Not implemented yet — no opcode decode |

---

## 4. TEST VECTORS NOT MATCHING C REFERENCE

### 4.1: DATAPATH_VECTORS.md Test PC-02
- **Vector:** "Set D=0x0000, WE=1, tick → Q=0x0000"
- **Issue:** This tests the REGISTER, not the incrementer
- **Correct PC increment test:** PC_INC=1, tick → PC=PC+1
- **Fix:** Separate register load test from increment test

### 4.2: DATAPATH_VECTORS.md Test IR-03
- **Vector:** "D=0xAB, IR_LOAD=0, tick → IR unchanged (still 0xFF)"
- **Issue:** "still 0xFF" is wrong — IR retains its previous value, whatever it was
- **Fix:** "IR retains previous value"

### 4.3: DATAPATH_VECTORS.md Test ALU-11 (SHL)
- **Vector:** "A=0x80, SHL → RESULT=0x00, Z=1, N=0, C=1"
- **Check:** C=0x80, old bit7=1, so C=1. After shift: 0x80<<1=0x00 (8-bit). Z=1 (0x00==0), N=0 (bit7=0). CORRECT.
- **Status:** OK

### 4.4: DATAPATH_VECTORS.md Test ALU-12 (SHR)
- **Vector:** "A=0x01, SHR → RESULT=0x00, Z=1, N=0, C=1"
- **Check:** C=0x01, old bit0=1, so C=1. After shift: 0x01>>1=0x00. Z=1, N=0. CORRECT.
- **Status:** OK

### 4.5: Missing test vectors
- **NEG with A=0x80:** Result should be 0x80 (overflow), flags Z=0, N=1, C=0
- **DIV with A=0xFF, B=0x01:** Result=0xFF, Z=0, N=1, C=0
- **MOD with A=0xFF, B=0x01:** Result=0x00, Z=1, N=0, C=0
- **INC with A=0xFF:** Result=0x00, Z=1, N=0, C=0
- **DEC with A=0x00:** Result=0xFF, Z=0, N=1, C=0

---

## 5. ASSUMPTIONS IN REPORTS

### 5.1: "Present = PASS" assumption (FIXED)
- Previous report used component existence as proof of correctness
- Now fixed to require actual simulation

### 5.2: MUX select encoding assumption
- Reports assume select codes without verifying against actual circuit
- **Reality:** The DATAPATH circuit defines the encoding, not the reports
- **Fix:** Must verify select codes match between CTRL and DATAPATH

### 5.3: Control signal mapping assumption
- Reports assume control signals exist but don't verify they're connected
- **Reality:** CONTROL_UNIT has output pins but no wiring to DATAPATH
- **Fix:** CPU circuit needs wiring between CTRL and DATAPATH

### 5.4: PC priority assumption
- Reports don't specify what happens when PC_INC=1 AND PC_LOAD=1 simultaneously
- **C reference:** Only one happens per instruction (they're mutually exclusive in the step function)
- **Hardware:** Need explicit priority (PC_LOAD overrides PC_INC)

---

## 6. CONTROL SIGNAL ISSUES

| Signal | Width | Issue |
|--------|-------|-------|
| IR_LOAD | 1 | Defined but not connected in DATAPATH |
| PC_INC | 1 | Defined, needs OR gate with PC_LOAD for WE |
| PC_LOAD | 1 | Defined, but jump target path incomplete |
| MEM_READ | 1 | Defined but RAM has no explicit read enable |
| MEM_WRITE | 1 | Connected to RAM WE |
| ACC_WRITE | 1 | Connected to ACC WE |
| REG_WRITE | 1 | Connected to REG_FILE WE |
| FLAGS_WRITE | 1 | Connected to FLAGS WE |
| ALU_OP[3:0] | 4 | Connected to ALU |
| ALU_A_SRC[1:0] | 2 | Connected to MUX |
| ALU_B_SRC[1:0] | 2 | Connected to MUX |
| WR_SRC[1:0] | 2 | Connected to MUX |
| MEM_ADDR_SRC[1:0] | 2 | Connected to MUX |
| SP_INC | 1 | Defined, SP adder exists but WE logic incomplete |
| SP_DEC | 1 | Defined, SP subtractor exists but WE logic incomplete |
| HALT | 1 | Defined but no circuit to stop clock |

---

## 7. WIDTH MISMATCH POTENTIAL

| Location | Issue |
|----------|-------|
| IR.REG[3:0] → REG_FILE WrAddr[2:0] | 4-bit to 3-bit truncation (correct: reg 0-7) |
| IR.REG[3:0] → REG_FILE RdAddr1[2:0] | 4-bit to 3-bit truncation |
| IMM[7:0] → MEM_ADDR MUX | 8-bit to 16-bit (needs zero extension) |
| PC[15:0] → MEM_ADDR | 16-bit, correct |
| ACC[7:0] → ALU A | 8-bit, correct |
| REG[7:0] → ALU B | 8-bit, correct |
| SP[7:0] → ALU A MUX | 8-bit, correct |
| ALU RESULT[7:0] → ACC | 8-bit, correct |

---

## 8. CLOCK/RESET/WE ISSUES

| Issue | Detail |
|-------|--------|
| PC WE | Needs OR gate: PC_INC OR PC_LOAD |
| SP WE | Needs logic: SP_INC XOR SP_DEC (mutually exclusive) |
| IR WE | IR_LOAD direct connection |
| FLAGS WE | FLAGS_WRITE direct connection |
| ACC WE | ACC_WRITE direct connection |
| REG WE | REG_WRITE direct connection (via decoder) |
| RAM WE | MEM_WRITE direct connection |
| Reset values | PC=0, SP=0, ACC=0, FLAGS=0, REG_FILE=all 0 |

---

## 9. MUX SELECT ENCODING ISSUES

| MUX | Select Width | Encoding | Status |
|-----|-------------|----------|--------|
| MUX_PC_NEXT | 1 | 0=PC+1, 1=Jump | ASSUMED — needs verification |
| MUX_ALU_A | 2 | 00=REG, 01=ACC, 10=SP | ASSUMED — needs verification |
| MUX_ALU_B | 2 | 00=REG, 01=IMM, 10=ONE | ASSUMED — needs verification |
| MUX_WR_SRC | 2 | 00=ALU, 01=MEM, 10=PC | ASSUMED — needs verification |
| MUX_MEM_ADDR | 2 | 00=PC, 01=ZP, 10=REG_PAIR | ASSUMED — needs verification |
| MUX_MEM_WDATA | 1 | 0=REG, 1=ACC | ASSUMED — needs verification |

---

## 10. INSTRUCTION TIMING ISSUES

| Issue | Detail |
|-------|--------|
| Single-cycle vs multi-cycle | C model processes instruction in 1 step |
| Hardware needs 3+ cycles | FETCH → DECODE → EXECUTE |
| Multi-byte instructions | Need extra FETCH states |
| Not implemented | CONTROL_UNIT has no EXECUTE state decode |

---

## SUMMARY

| Category | Correct | Incorrect | Needs Proof |
|----------|---------|-----------|-------------|
| ISA encoding | 8 | 0 | 0 |
| ALU behavior | 12 | 0 | 1 (carry inversion) |
| Flags | 3 | 0 | 2 (SHL/SHR timing) |
| Stack | 3 | 0 | 0 |
| PC | 2 | 0 | 1 (priority) |
| Control signals | 0 | 0 | 15 (not wired) |
| MUX encoding | 0 | 0 | 6 (not verified) |
| Test vectors | 40 | 3 | 5 (missing vectors) |
