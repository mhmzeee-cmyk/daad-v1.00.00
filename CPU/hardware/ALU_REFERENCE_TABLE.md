# ALU_REFERENCE_TABLE.md — Exact ALU Behavior from C Reference

**Source:** `src/dhad_cpu.c` lines 312-576
**Status:** EXTRACTED FROM SOURCE — NOT ASSUMED

---

## Flag Definitions

```c
#define DHAD_FLAG_ZERO      0x01  // bit 0
#define DHAD_FLAG_NEG       0x02  // bit 1
#define DHAD_FLAG_CARRY     0x04  // bit 2
#define DHAD_FLAG_OVERFLOW  0x08  // bit 3 (reserved, not used in C model)
```

---

## Critical Rule: Flag Update Behavior

**Different instructions have DIFFERENT flag update patterns:**

| Pattern | Instructions | Behavior |
|---------|-------------|----------|
| CLEAR-then-SET | ADD, SUB, CMP | `flags = 0; then set Z, N, C` |
| SET-only | SHL, SHR, NOT, NEG, INC, DEC, XOR, OR, AND | Only modify specific bits, preserve others |

**This means:**
- ADD/SUB/CMP: flags.Z and flags.N are computed from the 8-bit RESULT
- SHL/SHR: flags.C is computed from the OLD bit BEFORE shift
- NOT/NEG/INC/DEC: no C flag update

---

## Complete ALU Operation Table

### ADD (opcode 0x9)
```c
uint16_t r = (uint16_t)acc + (uint16_t)v;
acc = (uint8_t)r;
flags = 0;
if (acc == 0) flags |= Z;
if (acc & 0x80) flags |= N;
if (r > 0xFF) flags |= C;
```

| A | B | 16-bit r | Result | Z | N | C |
|---|---|----------|--------|---|---|---|
| 0x00 | 0x00 | 0x0000 | 0x00 | 1 | 0 | 0 |
| 0x01 | 0x01 | 0x0002 | 0x02 | 0 | 0 | 0 |
| 0x12 | 0x34 | 0x0046 | 0x46 | 0 | 0 | 0 |
| 0x7F | 0x01 | 0x0080 | 0x80 | 0 | 1 | 0 |
| 0x80 | 0x01 | 0x0081 | 0x81 | 0 | 1 | 0 |
| 0xFF | 0x01 | 0x0100 | 0x00 | 1 | 0 | 1 |
| 0xFF | 0xFF | 0x01FE | 0xFE | 0 | 1 | 1 |

---

### SUB (opcode 0xA)
```c
uint16_t r = (uint16_t)acc - (uint16_t)v;
acc = (uint8_t)r;
flags = 0;
if (acc == 0) flags |= Z;
if (acc & 0x80) flags |= N;
if (r > 0xFF) flags |= C;  // C = BORROW (A < B unsigned)
```

| A | B | 16-bit r | Result | Z | N | C | Note |
|---|---|----------|--------|---|---|---|------|
| 0x00 | 0x00 | 0x0000 | 0x00 | 1 | 0 | 0 | Equal |
| 0x10 | 0x05 | 0x000B | 0x0B | 0 | 0 | 0 | A > B |
| 0x05 | 0x10 | 0x00F5 | 0xF5 | 0 | 1 | 1 | A < B → C=1 (borrow) |
| 0x42 | 0x42 | 0x0000 | 0x00 | 1 | 0 | 0 | Equal |
| 0x00 | 0x01 | 0x00FF | 0xFF | 0 | 1 | 1 | Underflow |
| 0xFF | 0x01 | 0x00FE | 0xFE | 0 | 1 | 0 | |
| 0x80 | 0x01 | 0x007F | 0x7F | 0 | 0 | 0 | |

**CRITICAL: C flag = BORROW, not traditional carry.**
In hardware: ALU subtractor carry-out is INVERTED to get this behavior.
- Two's complement: A - B = A + (~B) + 1
- If result > 0xFF → no borrow → C=0 in two's complement
- If result ≤ 0xFF → borrow → C=1 in two's complement
- C code: `r > 0xFF` → C=1 (this is the BORROW)

**Wait — re-analyzing:**
- `r > 0xFF` after subtraction means the 16-bit result didn't fit in 8 bits
- For A-B: if A ≥ B, r = A-B ≤ 0xFF, so `r > 0xFF` is FALSE → C=0
- If A < B, r = A-B+0x100 > 0xFF, so `r > 0xFF` is TRUE → C=1
- **C=1 means BORROW (A < B unsigned)**

**Hardware mapping:**
- Standard two's complement subtractor: result = A + (~B) + 1
- Cout = 1 when A ≥ B (no borrow), Cout = 0 when A < B (borrow)
- To match C reference: **invert the carry output** from the subtractor

---

### AND (extended opcode 0x9r)
```c
acc &= v;
flags = 0;
if (acc == 0) flags |= Z;
if (acc & 0x80) flags |= N;
```

| A | B | Result | Z | N | C |
|---|---|--------|---|---|---|
| 0x0F | 0xF0 | 0x00 | 1 | 0 | 0 |
| 0xFF | 0x0F | 0x0F | 0 | 0 | 0 |
| 0xFF |  clears | | ||||---||1||---

||--- ||<br|### OR### |### OR OR OR###||---|══||||---|||---|─|---


---

|### OR══
 0|════
|════──│.

|══════|---|,|══|---|||---

|=|--- SET0 OR,||
,0══op的 OR2(---══|──， |,X| 0| be


 OR**0.
 (0�| |
| A | B | Result | Z | N | C |
|---|---|--------|---|---|---|
| 0x0F | 0xF0 | xFF | 0 | 1 | 0 |
| 0x55 | 0xAA | xFF | 0 | 1 | 0 |

---

### OR (extended opcode 0x8r)
```c
acc |= v;
flags = 0;
if (acc == 0) flags |= Z;
if (acc & 0x80) flags |= N;
```

| A | B | Result | Z | N | C |
|---|---|--------|---|---|---|
| 0x0F | 0xF0 | 0xFF | 0 | 1 | 0 |
| 0x00 | 0x00 | 0x00 | 1 | 0 | 0 |

---

### XOR (extended opcode 0x7r)
```c
acc ^= v;
flags = 0;
if (acc == 0) flags |= Z;
if (acc & 0x80) flags |= N;
```

| A | B | Result | Z | N | C |
|---|---|--------|---|---|---|
| 0xFF | 0xFF | 0x00 | 1 | 0 | 0 |
| 0x55 | 0xAA | 0xFF | 0 | 1 | 0 |
| 0x00 | 0x00 | 0x00 | 1 | 0 | 0 |

---

### NOT (extended opcode 0xA0)
```c
acc = ~acc;
flags = 0;
if (acc == 0) flags |= Z;
if (acc & 0x80) flags |= N;
```

| A | Result | Z | N | C |
|---|--------|---|---|---|
| 0x00 | 0xFF | 0 | 1 | 0 |
| 0xFF | 0x00 | 1 | 0 | 0 |
| 0x55 | 0xAA | 0 | 1 | 0 |

---

### SHL (extended opcode 0xB0)
```c
flags = 0;
if (acc & 0x80) flags |= C;  // OLD bit7 → C
acc = (uint8_t)(acc << 1);
if (acc == 0) flags |= Z;
if (acc & 0x80) flags |= N;
```

| A | OLD bit7 | Result | Z | N | C | Note |
|---|----------|--------|---|---|---|------|
| 0x01 | 0 | 0x02 | 0 | 0 | 0 | |
| 0x80 | 1 | 0x00 | 1 | 0 | 1 | C = old bit7 |
| 0x40 | 0 | 0x80 | 0 | 1 | 0 | |
| 0x00 | 0 | 0x00 | 1 | 0 | 0 | |
| 0xFF | 1 | 0xFE | 0 | 1 | 1 | |

**CRITICAL: C = OLD bit7, computed BEFORE the shift.**

---

### SHR (extended opcode 0xC0)
```c
flags = 0;
if (acc & 0x01) flags |= C;  // OLD bit0 → C
acc = (uint8_t)(acc >> 1);
if (acc == 0) flags |= Z;
if (acc & 0x80) flags |= N;
```

| A | OLD bit0 | Result | Z | N | C | Note |
|---|----------|--------|---|---|---|------|
| 0x80 | 0 | 0x40 | 0 | 0 | 0 | |
| 0x01 | 1 | 0x00 | 1 | 0 | 1 | C = old bit0 |
| 0x03 | 1 | 0x01 | 0 | 0 | 1 | |
| 0x00 | 0 | 0x00 | 1 | 0 | 0 | |
| 0xFF | 1 | 0x7F | 0 | 0 | 1 | |

**CRITICAL: C = OLD bit0, computed BEFORE the shift.**
**N after SHR is always 0 (unsigned shift fills with 0).**

---

### MUL (opcode 0xB)
```c
uint16_t r = (uint16_t)acc * (uint16_t)v;
acc = (uint8_t)(r & 0xFF);
flags = 0;
if (acc == 0) flags |= Z;
if (acc & 0x80) flags |= N;
```

| A | B | 16-bit r | Result | Z | N | C |
|---|---|----------|--------|---|---|---|
| 0x03 | 0x04 | 0x000C | 0x0C | 0 | 0 | 0 |
| 0x10 | 0x10 | 0x0100 | 0x00 | 1 | 0 | 0 |
| 0xFF | 0x02 | 0x01FE | 0xFE | 0 | 1 | 0 |
| 0x00 | 0xFF | 0x0000 | 0x00 | 1 | 0 | 0 |

**Note: Low 8 bits of product. C flag is ALWAYS 0 (not updated).**

---

### DIV (opcode 0xC)
```c
uint8_t v = dhad_cpu_get_reg(cpu, reg);
if (v == 0) {
    cpu->halted = true;
    cpu->status = DHAD_ERROR_DIV_ZERO;
    break;
}
cpu->acc = cpu->acc / v;
flags = 0;
if (acc == 0) flags |= Z;
if (acc & 0x80) flags |= N;
```

| A | B | Result | Z | N | C | Note |
|---|---|--------|---|---|---|------|
| 0x0A | 0x03 | 0x03 | 0 | 0 | 0 | 10/3=3 |
| 0x0A | 0x01 | 0x0A | 0 | 0 | 0 | |
| 0x00 | 0x01 | 0x00 | 1 | 0 | 0 | |
| 0xFF | 0x01 | 0xFF | 0 | 1 | 0 | |
| 0x0A | 0x00 | HALT | - | - | - | CPU halts |

**Note: C flag is ALWAYS 0 (not updated). Division by zero HALTS the CPU.**

---

### MOD (opcode 0xD)
```c
if (v == 0) { cpu->halted = true; break; }
cpu->acc = cpu->acc % v;
flags = 0;
if (acc == 0) flags |= Z;
if (acc & 0x80) flags |= N;
```

| A | B | Result | Z | N | C | Note |
|---|---|--------|---|---|---|------|
| 0x0A | 0x03 | 0x01 | 0 | 0 | 0 | 10%3=1 |
| 0x0A | 0x05 | 0x00 | 1 | 0 | 0 | 10%5=0 |
| 0x0A | 0x00 | HALT | - | - | - | CPU halts |

**Note: C flag is ALWAYS 0. Division by zero HALTS.**

---

### CMP (extended opcode 0xE0-0xE7)
```c
uint8_t cmp_reg = ext & 0x0F;
uint8_t v = dhad_cpu_get_reg(cpu, cmp_reg);
uint16_t r = (uint16_t)cpu->acc - (uint16_t)v;
cpu->flags = 0;
if ((r & 0xFF) == 0) cpu->flags |= Z;
if (r & 0x80) cpu->flags |= N;
if (r > 0xFF) cpu->flags |= C;
```

| A | B | 16-bit r | Z | N | C | Note |
|---|---|----------|---|---|---|------|
| 0x10 | 0x10 | 0x0000 | 1 | 0 | 0 | Equal |
| 0x10 | 0x20 | 0x00F0 | 0 | 1 | 1 | A < B |
| 0x20 | 0x10 | 0x0010 | 0 | 0 | 0 | A > B |
| 0x00 | 0x01 | 0x00FF | 0 | 1 | 1 | |

**CRITICAL: CMP does NOT write to ACC. Flags only. Same C behavior as SUB.**

---

### NEG (opcode 0x8, reg != 0)
```c
cpu->acc = (uint8_t)(-(int8_t)cpu->acc);
cpu->flags = 0;
if (cpu->acc == 0) cpu->flags |= Z;
if (cpu->acc & 0x80) cpu->flags |= N;
```

| A | Result | Z | N | C | Note |
|---|--------|---|---|---|------|
| 0x00 | 0x00 | 1 | 0 | 0 | -0 = 0 |
| 0x01 | 0xFF | 0 | 1 | 0 | -1 = 0xFF |
| 0x80 | 0x80 | 0 | 1 | 0 | -128 = -128 (overflow!) |
| 0xFF | 0x01 | 0 | 0 | 0 | -(-1) = 1 |
| 0x7F | 0x81 | 0 | 1 | 0 | |

**Note: C flag is ALWAYS 0 (not updated). NEG is 2's complement: 0 - A.**

---

### INC (extended opcode 0xD2)
```c
uint8_t r = fetch(cpu) & 0x0F;
uint8_t v = dhad_cpu_get_reg(cpu, r);
v++;
dhad_cpu_set_reg(cpu, r, v);
cpu->flags = 0;
if (v == 0) cpu->flags |= Z;
if (v & 0x80) cpu->flags |= N;
```

| A | Result | Z | N | C | Note |
|---|--------|---|---|---|------|
| 0x00 | 0x01 | 0 | 0 | 0 | |
| 0xFF | 0x00 | 1 | 0 | 0 | Wraparound |
| 0x7F | 0x80 | 0 | 1 | 0 | |

**Note: C flag is ALWAYS 0. Operates on ANY register, writes back to SAME register.**

---

### DEC (extended opcode 0xD1)
```c
uint8_t r = fetch(cpu) & 0x0F;
uint8_t v = dhad_cpu_get_reg(cpu, r);
v--;
dhad_cpu_set_reg(cpu, r, v);
cpu->flags = 0;
if (v == 0) cpu->flags |= Z;
if (v & 0x80) cpu->flags |= N;
```

| A | Result | Z | N | C | Note |
|---|--------|---|---|---|------|
| 0x01 | 0x00 | 1 | 0 | 0 | |
| 0x00 | 0xFF | 0 | 1 | 0 | Wraparound |
| 0x80 | 0x7F | 0 | 0 | 0 | |

**Note: C flag is ALWAYS 0. Operates on ANY register.**

---

## Hardware ALU Implications

The ALU_8 must handle these差异:

| Operation | Flags Computation | C Source | Hardware Behavior |
|-----------|-------------------|----------|-------------------|
| ADD | Clear all, set Z/N/C | `flags=0; then set` | Output Z, N, Cout from adder |
| SUB | Clear all, set Z/N/C(borrow) | `flags=0; then set` | Output Z, N, **INVERT** Cout |
| AND/OR/XOR | Clear all, set Z/N | `flags=0; then set` | Output Z, N; C=0 |
| NOT | Clear all, set Z/N | `flags=0; then set` | Output Z, N; C=0 |
| SHL | Set C from old bit7, then Z/N | `if bit7: C=1; shift; then Z/N` | **Latch C before shift** |
| SHR | Set C from old bit0, then Z/N | `if bit0: C=1; shift; then Z/N` | **Latch C before shift** |
| MUL | Clear all, set Z/N | `flags=0; then set` | Output Z, N; C=0 |
| DIV | Clear all, set Z/N | `flags=0; then set` | Output Z, N; C=0 |
| MOD | Clear all, set Z/N | `flags=0; then set` | Output Z, N; C=0 |
| NEG | Clear all, set Z/N | `flags=0; then set` | Output Z, N; C=0 |
| CMP | Clear all, set Z/N/C(borrow) | `flags=0; then set` | Output Z, N, **INVERT** Cout |
| INC | Clear all, set Z/N | `flags=0; then set` | Output Z, N; C=0 |
| DEC | Clear all, set Z/N | `flags=0; then set` | Output Z, N; C=0 |

**Key insight:** The control unit must select WHICH flags to write based on the instruction:
- ADD/SUB/CMP: write Z, N, C
- SHL/SHR: write Z, N, C (C computed differently)
- All others: write Z, N only (C preserved from previous)
