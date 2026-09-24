# DATAPATH_DISCREPANCIES.md — Differences Between Docs and Source Code

**Audit Date:** 2026-09-05
**Source:** `src/dhad_cpu.c` (663 lines — the GOLDEN reference)

---

## Critical Behaviors Extracted From Source Code

### 1. fetch() — Lines 211-215
```c
static uint8_t fetch(DhadCPU *c) {
    uint8_t b = c->memory[c->pc];
    c->pc++;
    return b;
}
```
**Hardware implication:** Memory read happens AT current PC, THEN PC increments. These are separate operations in hardware.

### 2. fetch16() — Lines 217-221
```c
static uint16_t fetch16(DhadCPU *c) {
    uint8_t lo = fetch(c);
    uint8_t hi = fetch(c);
    return (uint16_t)((hi << 8) | lo);
}
```
**Hardware implication:** Little-endian. Low byte fetched first (PC), then high byte (PC+1).

### 3. REG Field Usage — Line 231
```c
uint8_t reg = raw & 0x0F;
```
- reg=0-7 → S0-S7
- reg=8 → ACC (via `get_reg(cpu, 8)` returning `cpu->acc`)
- reg>8 → masked to `r & 0x07` (wraps to S0-S7)

### 4. MOV Encoding — Lines 249-253
```c
case 0x2: {
    uint8_t dst = fetch(cpu);
    dhad_cpu_set_reg(cpu, dst, dhad_cpu_get_reg(cpu, reg));
    break;
}
```
**CRITICAL:** `reg` from byte0 is **SOURCE**, `dst` from byte1 is **DESTINATION**. This is the opposite of typical MOV encoding!

### 5. SUB Carry Flag — Lines 325-333
```c
uint16_t r = (uint16_t)cpu->acc - (uint16_t)v;
cpu->acc = (uint8_t)r;
if (r > 0xFF) cpu->flags |= DHAD_FLAG_C;
```
**CRITICAL:** `r > 0xFF` after subtraction means ACC < v (unsigned). This is **BORROW**, not traditional carry. The C flag is SET when there is a BORROW.

### 6. CMP — Lines 566-576
```c
uint16_t r = (uint16_t)cpu->acc - (uint16_t)v;
cpu->flags = 0;
if ((r & 0xFF) == 0) cpu->flags |= DHAD_FLAG_Z;
if (r & 0x80) cpu->flags |= DHAD_FLAG_N;
if (r > 0xFF) cpu->flags |= DHAD_FLAG_C;
```
**CMP does NOT write to ACC.** Only updates flags. ACC remains unchanged.

### 7. SHL — Lines 452-458
```c
cpu->flags = 0;
if (cpu->acc & 0x80) cpu->flags |= DHAD_FLAG_C;  // OLD bit7 → C
cpu->acc = (uint8_t)(cpu->acc << 1);
if (cpu->acc == 0) cpu->flags |= DHAD_FLAG_Z;
if (cpu->acc & 0x80) cpu->flags |= DHAD_FLAG_N;
```
**C = OLD bit7 (before shift).** Then result is checked for Z and N.

### 8. SHR — Lines 461-467
```c
cpu->flags = 0;
if (cpu->acc & 0x01) cpu->flags |= DHAD_FLAG_C;  // OLD bit0 → C
cpu->acc = (uint8_t)(cpu->acc >> 1);
if (cpu->acc == 0) cpu->flags |= DHAD_FLAG_Z;
if (cpu->acc & 0x80) cpu->flags |= DHAD_FLAG_N;
```
**C = OLD bit0 (before shift).** N after SHR is always 0 (unsigned shift).

### 9. INC/DEC — Lines 480-502
```c
case 0xD1: {  // DEC
    uint8_t r = fetch(cpu) & 0x0F;
    uint8_t v = dhad_cpu_get_reg(cpu, r);
    v--;
    dhad_cpu_set_reg(cpu, r, v);
    cpu->flags = 0;
    if (v == 0) cpu->flags |= DHAD_FLAG_Z;
    if (v & 0x80) cpu->flags |= DHAD_FLAG_N;
    break;
}
```
**INC/DEC operate on ANY register** (not just ACC). They write back to the SAME register. No C flag update.

### 10. NEG — Lines 304-309
```c
} else {
    cpu->acc = (uint8_t)(-(int8_t)cpu->acc);
    cpu->flags = 0;
    if (cpu->acc == 0) cpu->flags |= DHAD_FLAG_Z;
    if (cpu->acc & 0x80) cpu->flags |= DHAD_FLAG_N;
}
```
**NEG operates on ACC only.** 2's complement: `ACC = -ACC`. No C flag update.

### 11. Stack Semantics — Lines 139-150
```c
bool dhad_cpu_push(DhadCPU *cpu, uint16_t val) {
    if (cpu->sp < DHAD_STACK_SIZE - 1) {
        cpu->stack[cpu->sp++] = val;
        return true;
    }
    return false;
}

uint16_t dhad_cpu_pop(DhadCPU *cpu) {
    if (cpu->sp == 0) return 0;
    return cpu->stack[--cpu->sp];
}
```
**Stack grows UP:**
- PUSH: `stack[SP] = val; SP++`
- POP: `SP--; return stack[SP]`
- SP starts at 0

### 12. PUSH — Lines 273-277
```c
case 0x5:
    if (!dhad_cpu_push(cpu, cpu->acc)) {
        cpu->status = DHAD_ERROR_STACK_OVERFLOW;
    }
    break;
```
**PUSH pushes ACC (8-bit), NOT PC.** Stack stores `uint16_t` but ACC is 8-bit.

### 13. CALL — Lines 288-295
```c
case 0x7: {
    uint16_t addr = fetch16(cpu);  // Fetches 2-byte address
    if (!dhad_cpu_push(cpu, cpu->pc)) {  // Pushes PC AFTER fetch16
        cpu->status = DHAD_ERROR_STACK_OVERFLOW;
    }
    cpu->pc = addr;
    break;
}
```
**CALL pushes the return address** (PC after fetching the 3-byte instruction = start+3).

### 14. RET — Lines 299-303
```c
case 0x8:
    if (reg == 0) {
        if (cpu->sp == 0) {
            cpu->status = DHAD_ERROR_STACK_UNDERFLOW;
        }
        cpu->pc = dhad_cpu_pop(cpu);
    }
```
**RET pops 16-bit value into PC.** But stack entries are `uint16_t` and ACC push only stores 8-bit. This means CALL pushes 16-bit PC, RET pops 16-bit PC. PUSH instruction pushes 8-bit ACC (zero-extended).

### 15. RETI — Lines 589-599
```c
case 0xF5: {
    if (cpu->sp < 2) {
        cpu->status = DHAD_ERROR_STACK_UNDERFLOW;
    }
    uint16_t ret_addr = dhad_cpu_pop(cpu);   // Pop PC
    uint8_t ret_flags = (uint8_t)(dhad_cpu_pop(cpu) & 0xFF);  // Pop flags
    cpu->pc = ret_addr;
    cpu->flags = ret_flags;
    cpu->int_enabled = true;
    break;
}
```
**RETI pops twice:** first PC, then flags. Order matters!

---

## Discrepancies Found

| # | Issue | Documentation | Source Code | Impact |
|---|-------|---------------|-------------|--------|
| 1 | MOV operand order | Not clearly documented | reg=SRC, byte1=DST | Critical |
| 2 | SUB C flag meaning | "carry/borrow" ambiguous | C=1 when ACC<v (borrow) | Critical |
| 3 | PUSH width | Pushes ACC (8-bit) | Pushes ACC zero-extended to 16-bit | Medium |
| 4 | Stack direction | "grows UP" | stack[SP++], --SP | Verified correct |
| 5 | CALL pushes PC | After fetch16 | PC = start+3 (after all 3 bytes fetched) | Critical |
| 6 | INC/DEC targets | Not specified | Any register, not just ACC | Medium |
| 7 | NEG target | Not specified | ACC only | Low |

---

*All discrepancies resolved by deferring to source code.*
