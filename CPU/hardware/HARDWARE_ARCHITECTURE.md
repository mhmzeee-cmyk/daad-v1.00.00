# HARDWARE_ARCHITECTURE.md — Dhad Processor Hardware Design

**Version:** 1.0.0
**Date:** 2026-09-05
**Status:** Foundation created — 24 Logisim subcircuits

---

## Overview

This document describes the hardware implementation of the Dhad 8-bit Processor in Logisim Evolution. The hardware model is designed to be behaviorally compatible with the C reference model (`src/dhad_cpu.c`).

---

## Architecture Diagram

```
┌─────────────────────────────────────────────────────────────────────────┐
│                           DHAD CPU (Top Level)                         │
│                                                                         │
│  ┌──────────────────────────────────────────────────────────────────┐  │
│  │                        DATAPATH                                   │  │
│  │                                                                   │  │
│  │  ┌──────────┐    ┌──────────┐    ┌──────────┐    ┌──────────┐  │  │
│  │  │   PC_16  │    │   IR     │    │ REG_FILE │    │  ACC_8   │  │  │
│  │  │ 16-bit   │    │  8-bit   │    │  8x8     │    │  8-bit   │  │  │
│  │  │ Program  │    │ Instr.   │    │ S0-S7    │    │ Accum.   │  │  │
│  │  │ Counter  │    │ Register │    │ 2-read   │    │ Register │  │  │
│  │  └────┬─────┘    └────┬─────┘    │ 1-write  │    └────┬─────┘  │  │
│  │       │               │          └────┬─────┘         │         │  │
│  │       │               │               │               │         │  │
│  │       │          ┌────┴─────┐         │          ┌────┴─────┐  │  │
│  │       │          │ OPCODE   │         │          │ FLAGS_REG│  │  │
│  │       │          │ DECODER  │         │          │  Z, N, C │  │  │
│  │       │          └────┬─────┘         │          └────┬─────┘  │  │
│  │       │               │               │               │         │  │
│  │       │          ┌────┴─────┐         │               │         │  │
│  │       │          │ CONTROL  │◄────────┼───────────────┤         │  │
│  │       │          │  UNIT    │         │               │         │  │
│  │       │          │  (FSM)   │         │               │         │  │
│  │       │          └────┬─────┘         │               │         │  │
│  │       │               │               │               │         │  │
│  │  ┌────┴─────┐    ┌────┴─────┐    ┌────┴─────┐    ┌────┴─────┐  │  │
│  │  │  MUX     │    │  MUX     │    │  ALU_8   │    │   SP_8   │  │  │
│  │  │ Addr     │    │ Reg/Imm  │    │ 8-bit    │    │ 8-bit    │  │  │
│  │  │ Select   │    │ Select   │    │ 13 ops   │    │ Stack    │  │  │
│  │  └────┬─────┘    └────┬─────┘    └────┬─────┘    │ Pointer  │  │  │
│  │       │               │               │          └──────────┘  │  │
│  │       └───────────────┴───────┬───────┘                         │  │
│  │                               │                                  │  │
│  └───────────────────────────────┼──────────────────────────────────┘  │
│                                  │                                      │
│  ┌───────────────────────────────┴──────────────────────────────────┐  │
│  │                        RAM_64K                                    │  │
│  │                    64KB Memory                                    │  │
│  │                16-bit addr, 8-bit data                           │  │
│  └──────────────────────────────────────────────────────────────────┘  │
│                                                                         │
│  External: CLK, RST                                                     │
│  Output: ACC[7:0], PC[15:0], FLAGS[7:0], HALTED                       │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## Circuit Hierarchy

```
DHAD_CPU (Top Level)
├── DATAPATH
│   ├── PC_16              (16-bit Program Counter)
│   ├── INSTRUCTION_REGISTER (8-bit IR with OPCODE/REG extraction)
│   ├── REGISTER_FILE_8x8  (8x8-bit Register File: S0-S7)
│   ├── ACC_8              (8-bit Accumulator)
│   ├── FLAGS_REG          (Flags: Z, N, C)
│   ├── ALU_8              (8-bit ALU, 13 operations)
│   ├── SP_8               (8-bit Stack Pointer)
│   └── MUX8_1             (Operand Source MUX)
├── CONTROL_UNIT           (FSM: FETCH → DECODE → EXECUTE)
└── RAM_64K                (64KB RAM)
```

---

## Component Specifications

### 1. NAND2 — NAND Gate
- **Inputs:** A, B (1-bit each)
- **Output:** Y = NOT(A AND B)
- **Purpose:** Universal gate, foundation for all logic

### 2. NOT1 — NOT Gate
- **Inputs:** A (1-bit)
- **Output:** Y = NOT A

### 3. AND2 — AND Gate
- **Inputs:** A, B (1-bit each)
- **Output:** Y = A AND B

### 4. OR2 — OR Gate
- **Inputs:** A, B (1-bit each)
- **Output:** Y = A OR B

### 5. XOR2 — XOR Gate
- **Inputs:** A, B (1-bit each)
- **Output:** Y = A XOR B

### 6. MUX2_1 — 2:1 Multiplexer
- **Inputs:** A, B (1-bit), Sel (1-bit)
- **Output:** Y = Sel ? B : A

### 7. MUX8_1 — 8:1 Multiplexer
- **Inputs:** I0-I7 (8-bit each), Sel (3-bit)
- **Output:** Y = I[Sel]

### 8. DECODER_3to8 — 3-to-8 Decoder
- **Input:** Sel (3-bit)
- **Outputs:** Y0-Y7 (1-bit each)

### 9. HALF_ADDER — Half Adder
- **Inputs:** A, B
- **Outputs:** Sum = A XOR B, Cout = A AND B

### 10. FULL_ADDER — Full Adder
- **Inputs:** A, B, Cin
- **Outputs:** Sum, Cout

### 11. ADDER_8 — 8-bit Adder
- **Inputs:** A[7:0], B[7:0], Cin
- **Outputs:** Sum[7:0], Cout

### 12. SUBTRACTOR_8 — 8-bit Subtractor
- **Inputs:** A[7:0], B[7:0]
- **Outputs:** Diff[7:0], Bout

### 13. ALU_8 — Dhad 8-bit ALU

| ALU_OP | Operation | Result |
|--------|-----------|--------|
| 0000 | ADD | A + B |
| 0001 | SUB | A - B |
| 0010 | AND | A & B |
| 0011 | OR | A \| B |
| 0100 | XOR | A ^ B |
| 0101 | NOT | ~A |
| 0110 | SHL | A << 1 |
| 0111 | SHR | A >> 1 |
| 1000 | MUL | A * B (low byte) |
| 1001 | DIV | A / B |
| 1010 | MOD | A % B |
| 1011 | NEG | -A |
| 1100 | CMP | A - B (flags only) |

**Flags Output:** ZERO, NEGATIVE, CARRY

### 14. REGISTER_8 — 8-bit Register
- **Inputs:** D[7:0], WE, CLK, RST
- **Output:** Q[7:0]
- **Behavior:** On CLK rising edge, if WE=1, Q <= D

### 15. REGISTER_FILE_8x8 — 8x8 Register File
- **Write Port:** WrAddr[2:0], WrData[7:0], WE, CLK
- **Read Port 1:** RdAddr1[2:0] → RdData1[7:0] (async)
- **Read Port 2:** RdAddr2[2:0] → RdData2[7:0] (async)
- **Registers:** S0-S7 (indices 0-7)

### 16. ACC_8 — 8-bit Accumulator
- **Inputs:** D[7:0], WE, CLK, RST
- **Output:** Q[7:0]
- **Note:** Not part of register file; addressed separately (reg=8)

### 17. FLAGS_REG — Flags Register
- **Inputs:** D[7:0], WE, CLK, RST
- **Outputs:** Z (bit 0), N (bit 1), C (bit 2)

### 18. PC_16 — 16-bit Program Counter
- **Inputs:** D[15:0], WE, CLK, RST
- **Output:** Q[15:0]
- **Init:** 0x0000

### 19. SP_8 — 8-bit Stack Pointer
- **Inputs:** D[7:0], WE, CLK, RST
- **Output:** Q[7:0]
- **Init:** 0x00

### 20. INSTRUCTION_REGISTER — IR
- **Inputs:** D[7:0], IR_LOAD, CLK
- **Outputs:** OPCODE[3:0], REG[3:0], IR_FULL[7:0]
- **Behavior:** On CLK rising edge (IR_LOAD=1), latch D[7:0]
- **Extract:** OPCODE = IR[7:4], REG = IR[3:0]

### 21. RAM_64K — 64KB RAM
- **Inputs:** ADDR[15:0], WDATA[7:0], WE, CLK
- **Output:** RDATA[7:0]
- **Size:** 65,536 bytes

### 22. CONTROL_UNIT — FSM Control
- **Inputs:** OPCODE[3:0], EXT_OP[3:0], CLK, RST, FLAGS
- **Outputs:** IR_LOAD, PC_INC, PC_LOAD, MEM_READ, MEM_WRITE, ACC_WRITE, REG_WRITE, ALU_OP[3:0], FLAGS_WRITE, SP_INC, SP_DEC, HALT, STACK_PUSH, STACK_POP, INT_EN, INT_DIS
- **States:** FETCH(00) → DECODE(01) → EXECUTE(10) → FETCH

### 23. DATAPATH — Datapath
- Connects all components
- Provides bus connections between registers, ALU, memory

### 24. CPU — Top Level
- Instantiates DATAPATH + CONTROL_UNIT + RAM_64K
- External interface: CLK, RST, ACC_OUT, PC_OUT, FLAGS_OUT, HALTED

---

## Instruction Cycle (Hardware)

```
FETCH (State S0):
  IR_LOAD = 1
  MEM_READ = 1  (read Memory[PC])
  PC_INC = 1    (PC = PC + 1)

DECODE (State S1):
  (Combinational: decode OPCODE, determine instruction type)

EXECUTE (State S2):
  Based on OPCODE:
    - Set ALU_OP
    - Set REG_WRITE / ACC_WRITE
    - Set MEM_READ / MEM_WRITE
    - Set PC_LOAD (for jumps)
    - Set SP_INC / SP_DEC (for stack ops)
    - Set HALT (for halt)
    - Set FLAGS_WRITE (for arithmetic/logic)
  All signals active for one CLK cycle

Then transition back to FETCH.
```

---

## File Locations

| File | Description |
|------|-------------|
| `hardware/dhad_cpu.circ` | Logisim Evolution project (24 circuits) |
| `hardware/HARDWARE_SPEC.md` | Detailed hardware specification |
| `hardware/HARDWARE_ARCHITECTURE.md` | This file |
| `src/dhad_cpu.c` | C reference model (DO NOT MODIFY) |
| `include/dhad_cpu.h` | C reference header |

---

## Known Limitations (Current Version)

1. **DIV/MOD by zero** — Hardware doesn't detect yet (C model halts)
2. **Stack overflow/underflow** — Not detected in hardware yet
3. **MMIO** — Not implemented (future phase)
4. **Interrupts** — EI/DI/RETI signals exist but full ISR not wired
5. **LDRI/STRI** — Register-indirect addressing not fully connected

---

## Next Steps

1. Wire all components in DATAPATH circuit
2. Complete FSM logic in CONTROL_UNIT
3. Add DIV/MOD zero detection
4. Add stack overflow/underflow detection
5. Implement MMIO (Display, Keyboard, Timer)
6. Test each instruction individually
7. Run full program tests

---

*This document is the single source of truth for the Dhad hardware architecture.*
