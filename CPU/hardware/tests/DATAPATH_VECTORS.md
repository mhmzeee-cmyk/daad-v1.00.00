# DATAPATH_VECTORS.md — Corrected Manual Test Vectors for Logisim Simulation

**Purpose:** Step-by-step instructions for testing each subcircuit in Logisim Evolution
**Reference:** ALU_REFERENCE_TABLE.md, C source code (dhad_cpu.c)

---

## IMPORTANT: Simulation Notes

- All clocks are manual (right-click → Tick Once)
- All resets are active-high (set to 1, then back to 0)
- "Set" means: right-click pin → Set State → enter value
- "Read" means: hover over output pin to see value
- Binary values shown as: 0b00000000 or 0x00

---

## TEST 1: PC_16

**Goal:** Verify PC loads, increments, and resets correctly

### T1-1: Reset
- Set RST=1, CLK=0
- Set D=0x0000, WE=0
- Tick → Q=0x0000 
- Set RST=0

### T1-2: Load
- Set D=0x0042, WE=1
- Tick → Q=0x0042 
- Set WE=0

### T1-3: Increment (via external adder + MUX)
- Set PC_INC=1, PC_LOAD=0
- MUX selects PC+1 path
- Tick → Q=0x0043 

### T1-4: Wrap
- Set D=0xFFFF, WE=1
- Tick → Q=0xFFFF 
- Set PC_INC=1, tick → Q=0x0000 

---

## TEST 2: INSTRUCTION_REGISTER

**Goal:** Verify IR latches data and extracts OPCODE/REG

### T2-1: Reset
- Set RST=1, CLK=0
- Tick → IR=0x00, OPCODE=0x0, REG=0x0
- Set RST=0

### T2-2: Load
- Set D=0xA3 (OPCODE=0xA, REG=0x3)
- Set IR_LOAD=1
- Tick → OPCODE=0xA, REG=0x3 
- Set IR_LOAD=0

### T2-3: Hold
- Set D=0x57, IR_LOAD=0
- Tick → IR unchanged (still 0xA3) 

---

## TEST 3: REGISTER_FILE_8x8

**Goal:** Verify read/write operations on registers

### T3-1: Write S0
- Set WrAddr=0 (S0), WrData=0x42, REG_WRITE=1
- Tick → S0=0x42 

### T3-2: Write S7
- Set WrAddr=7 (S7), WrData=0xBE, REG_WRITE=1
- Tick → S7=0xBE 

### T3-3: Read S0
- Set RdAddr1=0
- Read RdData1=0x42 

### T3-4: Read S7
- Set RdAddr1=7
- Read RdData1=0xBE 

### T3-5: Write-Read
- Set WrAddr=3, WrData=0x11, REG_WRITE=1, tick
- Set REG_WRITE=0
- Set RdAddr1=3
- Read RdData1=0x11 

---

## TEST 4: ACC_8

**Goal:** Verify accumulator load, reset

### T4-1: Reset
- Set RST=1, CLK=0
- Tick → Q=0x00
- Set RST=0

### T4-2: Load
- Set D=0x55, WE=1
- Tick → Q=0x55 

### T4-3: Hold
- Set D=0x00, WE=0
- Tick → Q=0x55 (unchanged) 

---

## TEST 5: ALU_8 — ALL 13 OPERATIONS

**Reference:** ALU_REFERENCE_TABLE.md

### T5-1: ADD (0000)
- Set ALU_OP=0000
- Set A=0x34, B=0x12
- Read: RESULT=0x46, Z=0, N=0, C=0 

### T5-2: ADD overflow
- Set ALU_OP=0000
- Set A=0xFF, B=0x01
- Read: RESULT=0x00, Z=1, N=0, C=1 

### T5-3: SUB (0001)
- Set ALU_OP=0001
- Set A=0x34, B=0x12
- Read: RESULT=0x22, Z=0, N=0, C=0 

### T5-4: SUB borrow
- Set ALU_OP=0001
- Set A=0x05, B=0x0A
- Read: RESULT=0xFB, Z=0, N=1, C=1 

### T5-5: SUB equal
- Set ALU_OP=0001
- Set A=0x42, B=0x42
- Read: RESULT=0x00, Z=1, N=0, C=0 

### T5-6: AND (0010)
- Set ALU_OP=0010
- Set A=0xFF, B=0x0F
- Read: RESULT=0x0F, Z=0, N=0, C=0 

### T5-7: OR (0011)
- Set ALU_OP=0011
- Set A=0xF0, B=0x0F
- Read: RESULT=0xFF, Z=0, N=1, C=0 

### T5-8: XOR (0100)
- Set ALU_OP=0100
- Set A=0xFF, B=0xFF
- Read: RESULT=0x00, Z=1, N=0, C=0 

### T5-9: NOT (0101)
- Set ALU_OP=0101
- Set A=0x0F
- Read: RESULT=0xF0, Z=0, N=1, C=0 

### T5-10: SHL (0110)
- Set ALU_OP=0110
- Set A=0x05 (0b00000101)
- Read: RESULT=0x0A, Z=0, N=0, C=0 

### T5-11: SHL carry
- Set ALU_OP=0110
- Set A=0x80 (0b10000000)
- Read: RESULT=0x00, Z=1, N=0, C=1 

### T5-12: SHR (0111)
- Set ALU_OP=0111
- Set A=0x05 (0b00000101)
- Read: RESULT=0x02, Z=0, N=0, C=1 

### T5-13: SHR carry
- Set ALU_OP=0111
- Set A=0x01 (0b00000001)
- Read: RESULT=0x00, Z=1, N=0, C=1 

### T5-14: MUL (1000)
- Set ALU_OP=1000
- Set A=0x03, B=0x07
- Read: RESULT=0x15, Z=0, N=0, C=0 

### T5-15: DIV (1001)
- Set ALU_OP=1001
- Set A=0xFF, B=0x05
- Read: RESULT=0x33, Z=0, N=0, C=0 

### T5-16: DIV by zero
- Set ALU_OP=1001
- Set A=0x10, B=0x00
- Read: RESULT=0x00, Z=1, N=0, C=0, HALT=1 

### T5-17: MOD (1010)
- Set ALU_OP=1010
- Set A=0xFF, B=0x05
- Read: RESULT=0x04, Z=0, N=0, C=0 

### T5-18: MOD by zero
- Set ALU_OP=1010
- Set A=0x10, B=0x00
- Read: RESULT=0x00, Z=1, N=0, C=0, HALT=1 

### T5-19: NEG (1011)
- Set ALU_OP=1011
- Set A=0x05
- Read: RESULT=0xFB, Z=0, N=1, C=0 

### T5-20: NEG zero
- Set ALU_OP=1011
- Set A=0x00
- Read: RESULT=0x00, Z=1, N=0, C=0 

### T5-21: NEG overflow
- Set ALU_OP=1011
- Set A=0x80
- Read: RESULT=0x80, Z=0, N=1, C=0 

### T5-22: CMP (1100)
- Set ALU_OP=1100
- Set A=0x10, B=0x20
- Read: RESULT=0xF0, Z=0, N=1, C=1 
- Note: CMP result is computed but NOT written to ACC

### T5-23: CMP equal
- Set ALU_OP=1100
- Set A=0x42, B=0x42
- Read: RESULT=0x00, Z=1, N=0, C=0 

---

## TEST 6: FLAGS_REG

**Goal:** Verify flags register latches correctly

### T6-1: Set flags
- Set D=0b00000101 (Z=1, N=0, C=1), WE=1
- Tick → Z=1, N=0, C=1 

### T6-2: Clear flags
- Set D=0b00000000, WE=1
- Tick → Z=0, N=0, C=0 

### T6-3: Hold
- Set D=0b00000111, WE=0
- Tick → Z=0, N=0, C=0 (unchanged) 

---

## TEST 7: SP_8

**Goal:** Verify stack pointer increment/decrement

### T7-1: Reset
- Set RST=1
- Tick → Q=0x00
- Set RST=0

### T7-2: Load and increment
- Set D=0x05, WE=1, tick
- Set WE=0
- Set SP_INC=1, tick → Q=0x06 

### T7-3: Decrement
- Set SP_INC=0, SP_DEC=1, tick → Q=0x05 

### T7-4: Wrap
- Set D=0x00, WE=1, tick
- Set WE=0, SP_DEC=1, tick → Q=0xFF 

---

## TEST 8: RAM_64K

**Goal:** Verify RAM read/write

### T8-1: Write
- Set ADDR=0x0100, WDATA=0xAB, WE=1, CLK=0
- Tick → RAM[0x0100]=0xAB 

### T8-2: Read
- Set ADDR=0x0100, WE=0
- Read RDATA=0xAB 

### T8-3: Overwrite
- Set ADDR=0x0100, WDATA=0xCD, WE=1
- Tick → RAM[0x0100]=0xCD 

---

## TEST 9: DATAPATH (Integration)

**Goal:** Verify end-to-end data flow with control signals

### T9-1: FETCH cycle
- Set PC=0x0000, MEM_ADDR_SRC=00(PC), MEM_READ=1, IR_LOAD=1
- Pre-load RAM[0x0000]=0xA3 (ADD S3)
- Tick → IR=0xA3, OPCODE=0xA, REG=0x3

### T9-2: ALU operation
- Set ALU_OP=0000(ADD), ALU_A_SRC=01(ACC), ALU_B_SRC=00(REG)
- Set ACC=0x10, REG.RdData1=0x20
- Read ALU.RESULT=0x30

### T9-3: Write-back
- Set WR_SRC=00(ALU), ACC_WRITE=1
- Tick → ACC=0x30 

### T9-4: Memory address
- Set MEM_ADDR_SRC=01(ZP), IMM=0x42
- Read MEM_ADDR=0x0042 

### T9-5: Store
- Set MEM_WRITE=1, MUX_MEM_WDATA select=0(REG)
- Set REG.RdData1=0xBE
- Tick → RAM[0x0042]=0xBE 

### T9-6: Push
- Set MEM_ADDR_SRC=10(REG_PAIR), ACC=0x42
- Set MUX_MEM_WDATA select=1(ACC)
- Set MEM_WRITE=1
- Tick → RAM[SP]=0x42, SP increments

---

## TEST 10: CONTROL_UNIT (Basic)

**Goal:** Verify FSM transitions

### T10-1: Reset
- Set RST=1
- Tick → State=FETCH(00)
- Set RST=0

### T10-2: FETCH→DECODE
- Tick → State=DECODE(01)

### T10-3: DECODE→EXECUTE
- Tick → State=EXECUTE(10)

### T10-4: EXECUTE→FETCH
- Tick → State=FETCH(00)

---

## MISSING/TODO Test Vectors

| Test | Why Needed | Priority |
|------|-----------|----------|
| NEG A=0x80 | Overflow behavior | HIGH |
| INC A=0xFF | Wrap behavior | HIGH |
| DEC A=0x00 | Wrap behavior | HIGH |
| DIV A=0xFF B=0x01 | Edge case | MEDIUM |
| MOD A=0xFF B=0x01 | Edge case | MEDIUM |
| CALL push high byte | SP+1 for high byte | HIGH |
| RET pop | SP decrement and pop | HIGH |
| Branch taken/not-taken | Flag-based jumps | MEDIUM |
| Multi-byte instruction fetch | Extra FETCH cycles | HIGH |
| Stack overflow | SP > 31 | LOW |
