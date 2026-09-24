# HARDWARE_SPEC.md — Dhad Processor Hardware Specification

**Source:** `src/dhad_cpu.c` + `include/dhad_cpu.h` + `include/dhad_memory_map.h`
**Extracted:** 2026-09-05
**Status:** Extracted from working C reference model

---

## 1. CPU Core

| Property | Value | Source |
|----------|-------|--------|
| Data Width | 8-bit | `uint8_t` everywhere |
| Address Width | 16-bit | `uint16_t pc`, `uint16_t addr` |
| Memory Size | 65,536 bytes (64 KB) | `DHAD_MEM_SIZE = 65536` |
| Clock Model | 1 cycle per instruction (base) | `cpu->cycles++` in step() |
| Max Cycles | 100,000 (safety limit) | `DHAD_MAX_CYCLES` |

---

## 2. Registers

| Register | Width | Encoding | Init Value | Source |
|----------|-------|----------|------------|--------|
| S0-S7 | 8-bit | reg=0-7 | 0x00 | `cpu->regs[r & 0x07]` |
| ACC (مح) | 8-bit | reg=8 (or opcode-specific) | 0x00 | `cpu->acc` |
| FLAGS | 8-bit | dedicated | 0x00 | `cpu->flags` |
| PC | 16-bit | dedicated | 0x0000 | `cpu->pc = DHAD_PROG_START` |
| SP | 8-bit | dedicated | 0x00 | `cpu->sp` |

### Register Access Rules (from `dhad_cpu_get_reg` / `dhad_cpu_set_reg`)

```
r == 8       → ACC
r & 0x07     → regs[r & 0x07]  (wraps 0xF to S7)
```

### Flags (from `dhad_memory_map.h`)

| Flag | Bit | Set When |
|------|-----|----------|
| Z | bit 0 | result == 0 |
| N | bit 1 | result bit 7 = 1 (negative in signed) |
| C | bit 2 | carry (ADD) or borrow (SUB/CMP) or MSB shift out |

**Critical:** Flags are set per-instruction, not accumulated. Each instruction clears flags then sets only relevant ones.

---

## 3. Stack

| Property | Value |
|----------|-------|
| Width | 16-bit per entry |
| Size | 64 entries |
| Direction | Grows UP (SP increments on PUSH) |
| Separate | YES — `uint16_t stack[64]`, NOT in RAM |
| Underflow | Returns 0, sets status |
| Overflow | Sets status (SP < 63 check) |

### PUSH (opcode 0x5):
```
push ACC only (16-bit, upper byte = 0)
SP = SP + 1
stack[SP] = ACC
```

### POP (opcode 0x6):
```
SP = SP - 1
ACC = stack[SP] & 0xFF
```

---

## 4. Memory Map

```
0x0000-0xEFFF  Program/RAM (61,440 bytes)
0xF000-0xF0FF  MMIO I/O (256 bytes)
0xF100-0xF1FF  IVT (256 bytes)
0xF200-0xFFFF  Reserved
```

### MMIO Address Format: 0xF0DD
```
bits[11:8] = device index (0-15)
bits[7:4]  = register within device (0-15)
bits[3:0]  = always 0 (in the 0xF0 range)
```

Actual encoding: `0xF000 | (device << 4) | reg`

### MMIO Devices

| Device | Index | Address Range |
|--------|-------|---------------|
| Display | 0 | 0xF000-0xF00F |
| Keyboard | 1 | 0xF010-0xF01F |
| Timer | 2 | 0xF020-0xF02F |
| GPIO | 3 | 0xF030-0xF03F (future) |

---

## 5. Instruction Encoding

### Byte Format

```
Basic (1 byte):
  [7:4] = opcode (4-bit)
  [3:0] = reg/operand (4-bit)

+Immediate (2 bytes):
  byte0: [7:4] opcode, [3:0] reg
  byte1: immediate value (8-bit)

Extended (2 bytes):
  byte0: 0xF (EXT opcode)
  byte1: extended opcode (8-bit)

Extended+Address (4 bytes):
  byte0: 0xF
  byte1: extended opcode
  byte2: address low byte
  byte3: address high byte
```

**Important:** All multi-byte instructions are little-endian for addresses (lo first, hi second).

---

## 6. Complete ISA — Instruction Behavior

### Base Instructions (opcode = byte0[7:4])

#### 0x0: NOP
- **Bytes:** 1
- **Behavior:** Nothing. PC += 1.
- **Flags:** Not affected
- **Cycles:** 1

#### 0x1: LOAD_IMM
- **Bytes:** 2 (opcode, immediate8)
- **Behavior:** reg = immediate8
- **If reg < 8:** `regs[reg] = val`
- **If reg == 8:** `acc = val`
- **Flags:** Not affected
- **Cycles:** 1

#### 0x2: MOV
- **Bytes:** 2 (opcode, dst_operand)
- **Behavior:** dst = src (where src = reg from byte0)
- **dst = byte1 & 0x0F (with r==8 → ACC)**
- **src = byte0[3:0] (with r==8 → ACC)**
- **Flags:** Not affected
- **Cycles:** 1

#### 0x3: LOAD_MEM
- **Bytes:** 2 (opcode, addr8)
- **Behavior:** reg = memory[zero_extended_addr8]
- **Address is zero-extended from 8-bit to 16-bit**
- **Flags:** Not affected
- **Cycles:** 1

#### 0x4: STORE_MEM
- **Bytes:** 2 (opcode, addr8)
- **Behavior:** memory[zero_extended_addr8] = reg
- **Address is zero-extended from 8-bit to 16-bit**
- **Flags:** Not affected
- **Cycles:** 1

#### 0x5: PUSH
- **Bytes:** 1
- **Behavior:**
  ```
  if (SP >= 63) → status = STACK_OVERFLOW
  SP = SP + 1
  stack[SP] = ACC (16-bit, upper byte = 0)
  ```
- **Flags:** Not affected
- **Cycles:** 1

#### 0x6: POP
- **Bytes:** 1
- **Behavior:**
  ```
  if (SP == 0) → status = STACK_UNDERFLOW
  ACC = stack[SP] (low 8 bits)
  SP = SP - 1
  ```
- **Flags:** Not affected
- **Cycles:** 1

#### 0x7: CALL
- **Bytes:** 3 (opcode, addr_lo, addr_hi)
- **Behavior:**
  ```
  addr = fetch16() // lo then hi
  push(PC)         // push return address (PC after instruction)
  PC = addr
  ```
- **Flags:** Not affected
- **Cycles:** 1

#### 0x8: RET/NEG (reg field determines which)
- **Bytes:** 1
- **If reg == 0 (RET):**
  ```
  if (SP == 0) → status = STACK_UNDERFLOW
  PC = pop()  // 16-bit from stack
  ```
- **If reg != 0 (NEG):**
  ```
  ACC = (uint8_t)(-(int8_t)ACC)
  // 2's complement negate
  flags = 0
  if (ACC == 0) flags |= Z
  if (ACC & 0x80) flags |= N
  ```
- **Cycles:** 1

#### 0x9: ADD
- **Bytes:** 1
- **Behavior:**
  ```
  v = get_reg(reg)
  r = (uint16_t)ACC + (uint16_t)v
  ACC = (uint8_t)r
  flags = 0
  if (ACC == 0) flags |= Z
  if (ACC & 0x80) flags |= N
  if (r > 0xFF) flags |= C
  ```
- **Cycles:** 1

#### 0xA: SUB
- **Bytes:** 1
- **Behavior:**
  ```
  v = get_reg(reg)
  r = (uint16_t)ACC - (uint16_t)v
  ACC = (uint8_t)r
  flags = 0
  if (ACC == 0) flags |= Z
  if (ACC & 0x80) flags |= N
  if (r > 0xFF) flags |= C  // borrow (ACC < v unsigned)
  ```
- **Note:** C flag = borrow = (ACC < v unsigned). `r > 0xFF` when ACC < v.
- **Cycles:** 1

#### 0xB: MUL
- **Bytes:** 1
- **Behavior:**
  ```
  v = get_reg(reg)
  r = (uint16_t)ACC * (uint16_t)v
  ACC = (uint8_t)(r & 0xFF)  // low byte only
  flags = 0
  if (ACC == 0) flags |= Z
  if (ACC & 0x80) flags |= N
  ```
- **Cycles:** 1

#### 0xC: DIV
- **Bytes:** 1
- **Behavior:**
  ```
  v = get_reg(reg)
  if (v == 0) → halted = true, status = DIV_ZERO, break
  ACC = ACC / v
  flags = 0
  if (ACC == 0) flags |= Z
  if (ACC & 0x80) flags |= N
  ```
- **Cycles:** 1

#### 0xD: MOD
- **Bytes:** 1
- **Behavior:**
  ```
  v = get_reg(reg)
  if (v == 0) → halted = true, status = MOD_ZERO, break
  ACC = ACC % v
  flags = 0
  if (ACC == 0) flags |= Z
  if (ACC & 0x80) flags |= N
  ```
- **Cycles:** 1

#### 0xE: PRINT
- **Bytes:** 1
- **Behavior:** Output ACC as decimal number (via MMIO or callback)
- **Flags:** Not affected
- **Cycles:** 1

#### 0xF: EXT (Extended instructions)
- **Bytes:** 2-4 (fetches ext byte after 0xF)
- See Extended Instructions below

---

### Extended Instructions (byte0 = 0xF, byte1 = extended opcode)

#### 0xF0 0x00: JMP
- **Bytes:** 4 (0xF0, 0x00, addr_lo, addr_hi)
- **Behavior:** PC = addr16
- **Flags:** Not affected

#### 0xF0 0x10: JZ
- **Bytes:** 4
- **Behavior:** if (flags & Z) PC = addr16
- **Flags:** Not affected

#### 0xF0 0x20: JNZ
- **Bytes:** 4
- **Behavior:** if (!(flags & Z)) PC = addr16
- **Flags:** Not affected

#### 0xF0 0x30: JC
- **Bytes:** 4
- **Behavior:** if (flags & C) PC = addr16
- **Flags:** Not affected

#### 0xF0 0x40: JN
- **Bytes:** 4
- **Behavior:** if (flags & N) PC = addr16
- **Flags:** Not affected

#### 0xF0 0x50: PRINT_CH
- **Bytes:** 2
- **Behavior:** Output ACC as ASCII character
- **Flags:** Not affected

#### 0xF0 0x60: INPUT
- **Bytes:** 2
- **Behavior:** Read from keyboard → ACC
- **Flags:** Not affected

#### 0xF0 0x70+r: XOR
- **Bytes:** 2
- **Behavior:** ACC ^= get_reg(r)
- **Flags:** Z, N only

#### 0xF0 0x80+r: OR
- **Bytes:** 2
- **Behavior:** ACC |= get_reg(r)
- **Flags:** Z, N only

#### 0xF0 0x90+r: AND
- **Bytes:** 2
- **Behavior:** ACC &= get_reg(r)
- **Flags:** Z, N only

#### 0xF0 0xA0: NOT
- **Bytes:** 2
- **Behavior:** ACC = ~ACC
- **Flags:** Z, N only

#### 0xF0 0xB0: SHL
- **Bytes:** 2
- **Behavior:**
  ```
  flags = 0
  if (ACC & 0x80) flags |= C   // save old MSB
  ACC = ACC << 1
  if (ACC == 0) flags |= Z
  if (ACC & 0x80) flags |= N
  ```

#### 0xF0 0xC0: SHR
- **Bytes:** 2
- **Behavior:**
  ```
  flags = 0
  if (ACC & 0x01) flags |= C   // save old LSB
  ACC = ACC >> 1
  if (ACC == 0) flags |= Z
  if (ACC & 0x80) flags |= N   // N is always 0 after SHR (unsigned)
  ```

#### 0xF0 0xD0: SWAP
- **Bytes:** 3 (0xF0, 0xD0, operands)
- **operands[7:4] = dst, operands[3:0] = src**
- **Behavior:** tmp = get_reg(dst); set_reg(dst, get_reg(src)); set_reg(src, tmp)
- **Flags:** Not affected

#### 0xF0 0xD1: DEC
- **Bytes:** 3 (0xF0, 0xD1, reg_operand)
- **Behavior:** r = reg_operand & 0x0F; v = get_reg(r) - 1; set_reg(r, v)
- **Flags:** Z, N only

#### 0xF0 0xD2: INC
- **Bytes:** 3 (0xF0, 0xD2, reg_operand)
- **Behavior:** r = reg_operand & 0x0F; v = get_reg(r) + 1; set_reg(r, v)
- **Flags:** Z, N only

#### 0xF0 0xD3: HALT
- **Bytes:** 2
- **Behavior:** halted = true; status = HALTED

#### 0xF0 0xE0+r: CMP (r = 0-7)
- **Bytes:** 2
- **Behavior:**
  ```
  v = get_reg(r)
  r = (uint16_t)ACC - (uint16_t)v
  flags = 0
  if ((r & 0xFF) == 0) flags |= Z
  if (r & 0x80) flags |= N
  if (r > 0xFF) flags |= C
  ```
- **Note:** ACC is NOT modified. Only flags change.

#### 0xF0 0xF1: LDRI
- **Bytes:** 3 (0xF0, 0xF1, operands, addr_lo_reg)
- **operands[7:4] = dst, operands[3:0] = addr_hi_reg**
- **addr_lo_reg = byte3 & 0x0F**
- **Behavior:**
  ```
  addr_hi = get_reg(addr_hi_reg)
  addr_lo = get_reg(addr_lo_reg)
  addr = (addr_hi << 8) | addr_lo
  dst = mem_read(addr)
  set_reg(dst_reg, val)
  ```

#### 0xF0 0xF2: STRI
- **Bytes:** 3 (0xF0, 0xF2, operands, addr_lo_reg)
- **operands[7:4] = src, operands[3:0] = addr_hi_reg**
- **addr_lo_reg = byte3 & 0x0F**
- **Behavior:**
  ```
  addr_hi = get_reg(addr_hi_reg)
  addr_lo = get_reg(addr_lo_reg)
  addr = (addr_hi << 8) | addr_lo
  val = get_reg(src_reg)
  mem_write(addr, val)
  ```

#### 0xF0 0xF3: EI
- **Bytes:** 2
- **Behavior:** int_enabled = true

#### 0xF0 0xF4: DI
- **Bytes:** 2
- **Behavior:** int_enabled = false

#### 0xF0 0xF5: RETI
- **Bytes:** 2
- **Behavior:**
  ```
  if (SP < 2) → status = STACK_UNDERFLOW
  ret_addr = pop()  // 16-bit
  ret_flags = pop() & 0xFF  // 8-bit
  PC = ret_addr
  flags = ret_flags
  int_enabled = true
  ```

---

## 7. Instruction Cycle Model

```
FETCH:
  IR_byte0 = Memory[PC]; PC += 1
  opcode = IR_byte0[7:4]
  reg    = IR_byte0[3:0]

DECODE:
  if opcode == 0xF:
    IR_byte1 = Memory[PC]; PC += 1
    ext_opcode = IR_byte1
    if JMP/JZ/JNZ/JC/JN:
      addr_lo = Memory[PC]; PC += 1
      addr_hi = Memory[PC]; PC += 1

EXECUTE:
  Based on decoded opcode
  (see instruction table above)

POST-EXECUTE:
  cycles += 1
  device_tick (if bus attached)
  check_interrupts (if enabled && pending)
```

### Interrupt Dispatch
```
if (int_enabled && int_pending):
  pending = int_pending & int_mask
  for i in 0..7:
    if pending[i]:
      int_pending[i] = 0
      push(flags)
      push(PC)
      int_enabled = false
      PC = ivt_addr + (i * 4)
      break
```

---

## 8. ALU Operations Summary

| Operation | Input A | Input B | Output | Z | N | C |
|-----------|---------|---------|--------|---|---|---|
| ADD | ACC | reg | ACC + reg | result==0 | bit7 | carry |
| SUB | ACC | reg | ACC - reg | result==0 | bit7 | borrow |
| AND | ACC | reg | ACC & reg | result==0 | bit7 | — |
| OR | ACC | reg | ACC \| reg | result==0 | bit7 | — |
| XOR | ACC | reg | ACC ^ reg | result==0 | bit7 | — |
| NOT | ACC | — | ~ACC | result==0 | bit7 | — |
| SHL | ACC | — | ACC << 1 | result==0 | bit7 | old_bit7 |
| SHR | ACC | — | ACC >> 1 | result==0 | 0 | old_bit0 |
| CMP | ACC | reg | ACC - reg (flags only) | result==0 | bit7 | borrow |
| NEG | ACC | — | -ACC (2's comp) | result==0 | bit7 | — |
| MUL | ACC | reg | ACC * reg (low byte) | result==0 | bit7 | — |
| DIV | ACC | reg | ACC / reg | result==0 | bit7 | — |
| MOD | ACC | reg | ACC % reg | result==0 | bit7 | — |
| INC | reg | — | reg + 1 | result==0 | bit7 | — |
| DEC | reg | — | reg - 1 | result==0 | bit7 | — |

---

## 9. Control Signals (derived from instruction behavior)

| Signal | Description | When Active |
|--------|-------------|-------------|
| REG_WRITE | Write to register file | LOAD_IMM, MOV, LDRI, INC, DEC |
| ACC_WRITE | Write to ACC | LOAD_IMM, MOV, LOAD_MEM, POP, LDRI, all ALU ops |
| FLAGS_WRITE | Update flags | ADD, SUB, MUL, DIV, MOD, NEG, INC, DEC, AND, OR, XOR, NOT, SHL, SHR, CMP |
| ALU_EN | ALU output enable | ADD, SUB, MUL, DIV, MOD, AND, OR, XOR, NOT, SHL, SHR, CMP, NEG |
| ALU_OP[3:0] | ALU operation select | See ALU section |
| MEM_READ | Read from memory | LOAD_MEM, LDRI, fetch cycles |
| MEM_WRITE | Write to memory | STORE_MEM, STRI, PUSH (stack write) |
| IR_LOAD | Load instruction register | Every FETCH |
| PC_LOAD | Load PC from bus | JMP, JZ, JNZ, JC, JN, CALL, RET |
| PC_INC | Increment PC | Every instruction |
| SP_LOAD | Load SP | — |
| SP_INC | Increment SP | PUSH |
| SP_DEC | Decrement SP | POP |
| HALT | Stop execution | HALT instruction |
| INT_EN | Interrupt enable | EI instruction |
| INT_DIS | Interrupt disable | DI instruction |
| STACK_PUSH | Push to stack | PUSH, CALL, interrupt |
| STACK_POP | Pop from stack | POP, RET, RETI |

---

## 10. ALU Op Codes (for hardware implementation)

| ALU_OP | Operation | Result |
|--------|-----------|--------|
| 0000 | A + B | ADD |
| 0001 | A - B | SUB |
| 0010 | A & B | AND |
| 0011 | A \| B | OR |
| 0100 | A ^ B | XOR |
| 0101 | ~A | NOT |
| 0110 | A << 1 | SHL |
| 0111 | A >> 1 | SHR |
| 1000 | A * B | MUL (low byte) |
| 1001 | A / B | DIV |
| 1010 | A % B | MOD |
| 1011 | -A | NEG |
| 1100 | A - B (flags only) | CMP |

---

*This document is the single source of truth for the Dhad hardware design.*
*All hardware components must match this specification exactly.*
