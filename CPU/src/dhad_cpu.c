/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  dhad_cpu.c — Unified CPU Core implementation for Dhad Processor (8-bit, v4.0)
 *
 *  Implements the interface declared in dhad_cpu.h.
 *  All simulators (CLI, GUI, Live, Visual) share this core.
 *
 *  Verified architecture:
 *    - Data width:  8-bit
 *    - Address:     16-bit (0x0000 - 0xFFFF)
 *    - Registers:   8 general (S0-S7) + ACC
 *    - Flags:       Z(bit0), N(bit1), C(bit2)
 *    - Stack:       32 entries, separate array
 *    - PROG_START:  0x0000
 *
 *  Base Opcodes (4-bit from byte[0] >> 4):
 *    0x0: NOP            0x8: RET/NEG
 *    0x1: LOAD_IMM       0x9: ADD
 *    0x2: MOV            0xA: SUB
 *    0x3: LOAD_MEM       0xB: MUL
 *    0x4: STORE_MEM      0xC: DIV
 *    0x5: PUSH           0xD: MOD
 *    0x6: POP            0xE: PRINT
 *    0x7: CALL           0xF: Extended
 *
 *  Extended Opcodes (byte after 0xF prefix):
 *    0x00: JMP           0x80: OR
 *    0x10: JZ            0x90: AND
 *    0x20: JNZ           0xA0: NOT
 *    0x30: JC            0xB0: SHL
 *    0x40: JN            0xC0: SHR
 *    0x50: PRINT_CH      0xD0: SWAP
 *    0x60: INPUT         0xD1: DEC
 *    0x70: XOR           0xD2: INC
 *                        0xD3: HALT
 *                        0xE0-0xE7: CMP
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "dhad_cpu.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* ═══════════════════════════════════════
 *  Flags
 * ═══════════════════════════════════════ */

#define FLAG_Z  DHAD_FLAG_Z
#define FLAG_N  DHAD_FLAG_N
#define FLAG_C  DHAD_FLAG_C

/* ═══════════════════════════════════════
 *  Status Name
 * ═══════════════════════════════════════ */

const char *dhad_status_name(DhadStatus status) {
    switch (status) {
        case DHAD_OK:                     return "OK";
        case DHAD_HALTED:                 return "HALTED";
        case DHAD_ERROR_DIV_ZERO:         return "DIV_ZERO";
        case DHAD_ERROR_MOD_ZERO:         return "MOD_ZERO";
        case DHAD_ERROR_STACK_OVERFLOW:   return "STACK_OVERFLOW";
        case DHAD_ERROR_STACK_UNDERFLOW:  return "STACK_UNDERFLOW";
        case DHAD_ERROR_INVALID_OPCODE:   return "INVALID_OPCODE";
        case DHAD_ERROR_PC_OUT_OF_RANGE:  return "PC_OUT_OF_RANGE";
        case DHAD_ERROR_MAX_CYCLES:       return "MAX_CYCLES";
        case DHAD_ERROR_LOAD_FAILED:      return "LOAD_FAILED";
    }
    return "UNKNOWN";
}

/* ═══════════════════════════════════════
 *  Accessors
 * ═══════════════════════════════════════ */

bool dhad_cpu_is_halted(const DhadCPU *cpu) {
    return cpu->halted;
}

uint32_t dhad_cpu_get_cycles(const DhadCPU *cpu) {
    return cpu->cycles;
}

uint8_t dhad_cpu_get_acc(const DhadCPU *cpu) {
    return cpu->acc;
}

/* ═══════════════════════════════════════
 *  Lifecycle
 * ═══════════════════════════════════════ */

void dhad_cpu_init(DhadCPU *cpu) {
    memset(cpu, 0, sizeof(DhadCPU));
    cpu->pc = DHAD_PROG_START;
    cpu->sp = 0;
    cpu->halted = false;
    cpu->status = DHAD_OK;
    cpu->cycles = 0;
    cpu->int_enabled = false;
    cpu->int_mask = 0xFF;
    cpu->int_pending = 0;
    cpu->ivt_addr = 0x0040;
    cpu->output_len = 0;
    cpu->output[0] = '\0';
    cpu->output_func = NULL;
    cpu->output_userdata = NULL;
    cpu->input_func = NULL;
    cpu->input_userdata = NULL;
    cpu->prog_size = 0;
    cpu->bus = NULL;
}

void dhad_cpu_reset(DhadCPU *cpu) {
    memset(cpu->regs, 0, sizeof(cpu->regs));
    cpu->acc = 0;
    cpu->flags = 0;
    cpu->pc = DHAD_PROG_START;
    cpu->sp = 0;
    cpu->halted = false;
    cpu->status = DHAD_OK;
    cpu->cycles = 0;
    cpu->int_enabled = false;
    cpu->int_pending = 0;
    cpu->output_len = 0;
    cpu->output[0] = '\0';
}

int dhad_cpu_load_program(DhadCPU *cpu, const uint8_t *program, uint16_t size) {
    if (size > DHAD_PROG_MAX) return -1;
    memcpy(&cpu->memory[DHAD_PROG_START], program, size);
    cpu->prog_size = size;
    cpu->pc = DHAD_PROG_START;
    return 0;
}

/* ═══════════════════════════════════════
 *  Device Integration
 * ═══════════════════════════════════════ */

void dhad_cpu_attach_bus(DhadCPU *cpu, DhadDeviceBus *bus) {
    cpu->bus = bus;
}

DhadDeviceBus *dhad_cpu_get_bus(const DhadCPU *cpu) {
    return cpu->bus;
}

/* ═══════════════════════════════════════
 *  Register Access
 * ═══════════════════════════════════════ */

uint8_t dhad_cpu_get_reg(const DhadCPU *cpu, uint8_t r) {
    if (r < DHAD_REG_COUNT) return cpu->regs[r];
    if (r == DHAD_REG_COUNT) return cpu->acc;  /* r=8 is ACC */
    return 0;
}

void dhad_cpu_set_reg(DhadCPU *cpu, uint8_t r, uint8_t v) {
    if (r < DHAD_REG_COUNT) cpu->regs[r] = v;
    else if (r == DHAD_REG_COUNT) cpu->acc = v;
}

/* ═══════════════════════════════════════
 *  Memory Access
 * ═══════════════════════════════════════ */

uint8_t dhad_cpu_read_mem(const DhadCPU *cpu, uint16_t addr) {
    if (addr < DHAD_MEM_SIZE) return cpu->memory[addr];
    return 0;
}

void dhad_cpu_write_mem(DhadCPU *cpu, uint16_t addr, uint8_t val) {
    if (addr < DHAD_MEM_SIZE) cpu->memory[addr] = val;
}

/* ═══════════════════════════════════════
 *  Stack Operations
 * ═══════════════════════════════════════ */

bool dhad_cpu_push(DhadCPU *cpu, uint16_t val) {
    if (cpu->sp >= DHAD_STACK_SIZE - 1) return false;
    cpu->stack[cpu->sp++] = val;
    return true;
}

uint16_t dhad_cpu_pop(DhadCPU *cpu) {
    if (cpu->sp == 0) return 0;
    return cpu->stack[--cpu->sp];
}

/* ═══════════════════════════════════════
 *  Flags
 * ═══════════════════════════════════════ */

void dhad_cpu_update_flags_nz(DhadCPU *cpu, uint8_t result) {
    cpu->flags &= ~(FLAG_Z | FLAG_N);
    if (result == 0) cpu->flags |= FLAG_Z;
    if (result & 0x80) cpu->flags |= FLAG_N;
}

/* ═══════════════════════════════════════
 *  Output
 * ═══════════════════════════════════════ */

void dhad_cpu_output_num(DhadCPU *cpu, int value) {
    if (cpu->output_func) {
        cpu->output_func(value, false, cpu->output_userdata);
    } else if (cpu->output_len < DHAD_OUTPUT_BUF_SIZE - 16) {
        int n = snprintf(cpu->output + cpu->output_len,
                         DHAD_OUTPUT_BUF_SIZE - cpu->output_len, "%d", value);
        cpu->output_len += n;
    }
}

void dhad_cpu_output_char(DhadCPU *cpu, char c) {
    if (cpu->output_func) {
        cpu->output_func((int)(unsigned char)c, true, cpu->output_userdata);
    } else if (cpu->output_len < DHAD_OUTPUT_BUF_SIZE - 1) {
        cpu->output[cpu->output_len++] = c;
        cpu->output[cpu->output_len] = '\0';
    }
}

void dhad_cpu_output_clear(DhadCPU *cpu) {
    cpu->output_len = 0;
    cpu->output[0] = '\0';
}

/* ═══════════════════════════════════════
 *  Internal Helpers
 * ═══════════════════════════════════════ */

static uint8_t cpu_get_reg(const DhadCPU *c, uint8_t r) {
    return (r == 8) ? c->acc : c->regs[r & 0x07];
}

static void cpu_set_reg(DhadCPU *c, uint8_t r, uint8_t v) {
    if (r == 8) c->acc = v; else c->regs[r & 0x07] = v;
}

static uint8_t fetch(DhadCPU *c) {
    uint8_t b = c->memory[c->pc];
    c->pc++;
    return b;
}

static uint16_t fetch16(DhadCPU *c) {
    uint8_t lo = fetch(c);
    uint8_t hi = fetch(c);
    return (uint16_t)((hi << 8) | lo);
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  CPU Step — Execute one instruction
 *
 *  Byte 1: [opcode:4][reg:4]
 *  Byte 2: immediate / address-low / destination (for MOV)
 *  Byte 3: address-high (for extended jumps)
 *
 *  Returns number of bytes consumed.
 * ═══════════════════════════════════════════════════════════════════════════ */

int dhad_cpu_step(DhadCPU *cpu) {
    if (cpu->halted) return 0;

    /* ── Interrupt check ── */
    if (cpu->int_enabled) {
        uint8_t pending = cpu->int_pending & cpu->int_mask;
        if (pending) {
            for (int i = 0; i < 8; i++) {
                if (pending & (1 << i)) {
                    cpu->int_pending &= ~(1 << i);
                    dhad_cpu_push(cpu, cpu->flags);
                    dhad_cpu_push(cpu, cpu->pc);
                    cpu->int_enabled = false;
                    uint16_t handler = cpu->ivt_addr + (uint16_t)(i * 4);
                    cpu->pc = handler;
                    cpu->cycles += 3;
                    return 3;
                }
            }
        }
    }

    uint16_t start_pc = cpu->pc;
    uint8_t raw = fetch(cpu);
    uint8_t op = (raw >> 4) & 0x0F;
    uint8_t reg = raw & 0x0F;
    int consumed = 1;
    cpu->cycles++;

    switch (op) {

    /* ── 0x0: NOP ── */
    case 0x0:
        break;

    /* ── 0x1: LOAD_IMM — reg, imm8 ── */
    case 0x1: {
        uint8_t val = fetch(cpu);
        consumed = 2;
        if (reg < 8) cpu->regs[reg] = val;
        else         cpu->acc = val;
        break;
    }

    /* ── 0x2: MOV — byte1.reg = src, byte2 = dst ── */
    case 0x2: {
        uint8_t dst = fetch(cpu);
        consumed = 2;
        cpu_set_reg(cpu, dst, cpu_get_reg(cpu, reg));
        break;
    }

    /* ── 0x3: LOAD_MEM — reg = mem[addr8] ── */
    case 0x3: {
        uint8_t addr = fetch(cpu);
        consumed = 2;
        uint8_t v = cpu->memory[addr];
        if (reg < 8) cpu->regs[reg] = v;
        else         cpu->acc = v;
        break;
    }

    /* ── 0x4: STORE_MEM — mem[addr8] = reg ── */
    case 0x4: {
        uint8_t addr = fetch(cpu);
        consumed = 2;
        uint8_t v = (reg < 8) ? cpu->regs[reg] : cpu->acc;
        cpu->memory[addr] = v;
        break;
    }

    /* ── 0x5: PUSH — push acc ── */
    case 0x5:
        dhad_cpu_push(cpu, cpu->acc);
        break;

    /* ── 0x6: POP — pop → acc ── */
    case 0x6:
        cpu->acc = (uint8_t)(dhad_cpu_pop(cpu) & 0xFF);
        break;

    /* ── 0x7: CALL — addr16 ── */
    case 0x7: {
        uint16_t addr = fetch16(cpu);
        consumed = 3;
        dhad_cpu_push(cpu, cpu->pc);
        cpu->pc = addr;
        break;
    }

    /* ── 0x8: RET (reg=0) / NEG (reg≠0) ── */
    case 0x8:
        if (reg == 0) {
            cpu->pc = dhad_cpu_pop(cpu);
        } else {
            cpu->acc = (uint8_t)(-(int8_t)cpu->acc);
            dhad_cpu_update_flags_nz(cpu, cpu->acc);
        }
        break;

    /* ── 0x9: ADD ── */
    case 0x9: {
        uint8_t v = cpu_get_reg(cpu, reg);
        uint16_t r = (uint16_t)cpu->acc + (uint16_t)v;
        cpu->acc = (uint8_t)r;
        cpu->flags = 0;
        if (cpu->acc == 0) cpu->flags |= FLAG_Z;
        if (cpu->acc & 0x80) cpu->flags |= FLAG_N;
        if (r > 0xFF) cpu->flags |= FLAG_C;
        break;
    }

    /* ── 0xA: SUB ── */
    case 0xA: {
        uint8_t v = cpu_get_reg(cpu, reg);
        uint16_t r = (uint16_t)cpu->acc - (uint16_t)v;
        cpu->acc = (uint8_t)r;
        cpu->flags = 0;
        if (cpu->acc == 0) cpu->flags |= FLAG_Z;
        if (cpu->acc & 0x80) cpu->flags |= FLAG_N;
        if (r > 0xFF) cpu->flags |= FLAG_C;
        break;
    }

    /* ── 0xB: MUL ── */
    case 0xB: {
        uint8_t v = cpu_get_reg(cpu, reg);
        uint16_t r = (uint16_t)cpu->acc * (uint16_t)v;
        cpu->acc = (uint8_t)(r & 0xFF);
        dhad_cpu_update_flags_nz(cpu, cpu->acc);
        break;
    }

    /* ── 0xC: DIV ── */
    case 0xC: {
        uint8_t v = cpu_get_reg(cpu, reg);
        if (v == 0) { cpu->halted = true; cpu->status = DHAD_ERROR_DIV_ZERO; break; }
        cpu->acc = cpu->acc / v;
        dhad_cpu_update_flags_nz(cpu, cpu->acc);
        break;
    }

    /* ── 0xD: MOD ── */
    case 0xD: {
        uint8_t v = cpu_get_reg(cpu, reg);
        if (v == 0) { cpu->halted = true; cpu->status = DHAD_ERROR_MOD_ZERO; break; }
        cpu->acc = cpu->acc % v;
        dhad_cpu_update_flags_nz(cpu, cpu->acc);
        break;
    }

    /* ── 0xE: PRINT — print acc as number ── */
    case 0xE:
        dhad_cpu_output_num(cpu, cpu->acc);
        break;

    /* ── 0xF: Extended instructions ── */
    case 0xF: {
        uint8_t ext = fetch(cpu);
        consumed = 2;

        switch (ext) {

        /* ── Jumps: [ext][addr_lo][addr_hi] ── */
        case 0x00: { /* JMP */
            uint16_t addr = fetch16(cpu);
            consumed = 4;
            cpu->pc = addr;
            break;
        }
        case 0x10: { /* JZ */
            uint16_t addr = fetch16(cpu);
            consumed = 4;
            if (cpu->flags & FLAG_Z) cpu->pc = addr;
            break;
        }
        case 0x20: { /* JNZ */
            uint16_t addr = fetch16(cpu);
            consumed = 4;
            if (!(cpu->flags & FLAG_Z)) cpu->pc = addr;
            break;
        }
        case 0x30: { /* JC */
            uint16_t addr = fetch16(cpu);
            consumed = 4;
            if (cpu->flags & FLAG_C) cpu->pc = addr;
            break;
        }
        case 0x40: { /* JN */
            uint16_t addr = fetch16(cpu);
            consumed = 4;
            if (cpu->flags & FLAG_N) cpu->pc = addr;
            break;
        }

        /* ── I/O ── */
        case 0x50: { /* PRINT_CH */
            dhad_cpu_output_char(cpu, (char)cpu->acc);
            break;
        }
        case 0x60: { /* INPUT */
            int v = 0;
            if (cpu->input_func) {
                v = cpu->input_func(cpu->input_userdata);
            } else {
                if (scanf("%d", &v) != 1) v = 0;
            }
            cpu->acc = (uint8_t)(v & 0xFF);
            break;
        }

        /* ── Logic ── */
        case 0x70: { /* XOR */
            uint8_t v = cpu_get_reg(cpu, ext & 0x0F);
            cpu->acc ^= v;
            dhad_cpu_update_flags_nz(cpu, cpu->acc);
            break;
        }
        case 0x80: { /* OR */
            uint8_t v = cpu_get_reg(cpu, ext & 0x0F);
            cpu->acc |= v;
            dhad_cpu_update_flags_nz(cpu, cpu->acc);
            break;
        }
        case 0x90: { /* AND */
            uint8_t v = cpu_get_reg(cpu, ext & 0x0F);
            cpu->acc &= v;
            dhad_cpu_update_flags_nz(cpu, cpu->acc);
            break;
        }
        case 0xA0: { /* NOT */
            cpu->acc = ~cpu->acc;
            dhad_cpu_update_flags_nz(cpu, cpu->acc);
            break;
        }
        case 0xB0: { /* SHL */
            cpu->flags = 0;
            if (cpu->acc & 0x80) cpu->flags |= FLAG_C;
            cpu->acc = (uint8_t)(cpu->acc << 1);
            if (cpu->acc == 0) cpu->flags |= FLAG_Z;
            if (cpu->acc & 0x80) cpu->flags |= FLAG_N;
            break;
        }
        case 0xC0: { /* SHR */
            cpu->flags = 0;
            if (cpu->acc & 0x01) cpu->flags |= FLAG_C;
            cpu->acc = (uint8_t)(cpu->acc >> 1);
            if (cpu->acc == 0) cpu->flags |= FLAG_Z;
            if (cpu->acc & 0x80) cpu->flags |= FLAG_N;
            break;
        }

        /* ── SWAP ── */
        case 0xD0: {
            uint8_t operands = fetch(cpu);
            consumed = 3;
            uint8_t dst = (operands >> 4) & 0x0F;
            uint8_t src = operands & 0x0F;
            uint8_t *pd = (dst < 8) ? &cpu->regs[dst] : &cpu->acc;
            uint8_t *ps = (src < 8) ? &cpu->regs[src] : &cpu->acc;
            uint8_t tmp = *pd;
            *pd = *ps;
            *ps = tmp;
            break;
        }

        /* ── DEC ── */
        case 0xD1: {
            uint8_t r = fetch(cpu) & 0x0F;
            consumed = 3;
            uint8_t v = cpu_get_reg(cpu, r);
            v--;
            cpu_set_reg(cpu, r, v);
            cpu->flags = 0;
            if (v == 0) cpu->flags |= FLAG_Z;
            if (v & 0x80) cpu->flags |= FLAG_N;
            break;
        }

        /* ── INC ── */
        case 0xD2: {
            uint8_t r = fetch(cpu) & 0x0F;
            consumed = 3;
            uint8_t v = cpu_get_reg(cpu, r);
            v++;
            cpu_set_reg(cpu, r, v);
            cpu->flags = 0;
            if (v == 0) cpu->flags |= FLAG_Z;
            if (v & 0x80) cpu->flags |= FLAG_N;
            break;
        }

        /* ── HALT ── */
        case 0xD3:
            cpu->halted = true;
            cpu->status = DHAD_HALTED;
            break;

        /* ── CMP: 0xE0-0xE7 (all 8 registers) ── */
        case 0xE0: case 0xE1: case 0xE2: case 0xE3:
        case 0xE4: case 0xE5: case 0xE6: case 0xE7: {
            uint8_t v = cpu_get_reg(cpu, ext & 0x0F);
            uint16_t r = (uint16_t)cpu->acc - (uint16_t)v;
            cpu->flags = 0;
            if ((r & 0xFF) == 0) cpu->flags |= FLAG_Z;
            if (r & 0x80) cpu->flags |= FLAG_N;
            if (r > 0xFF) cpu->flags |= FLAG_C;
            break;
        }

        /* ── EI: 0xF3 ── */
        case 0xF3: { /* EI */
            cpu->int_enabled = true;
            break;
        }
        /* ── DI: 0xF4 ── */
        case 0xF4: { /* DI */
            cpu->int_enabled = false;
            break;
        }
        /* ── RETI: 0xF5 ── */
        case 0xF5: { /* RETI */
            uint16_t ret_addr = dhad_cpu_pop(cpu);
            uint8_t ret_flags = (uint8_t)(dhad_cpu_pop(cpu) & 0xFF);
            cpu->pc = ret_addr;
            cpu->flags = ret_flags;
            cpu->int_enabled = true;
            break;
        }

        /* ── LDRI: dst = mem[(hi<<8)|lo] ── */
        case 0xF1: {
            uint8_t operands = fetch(cpu);
            consumed = 3;
            uint8_t dst = (operands >> 4) & 0x0F;
            uint8_t src = operands & 0x0F;
            uint8_t lo = fetch(cpu);
            consumed = 4;
            /* Need high byte too — but ISA encoding may differ */
            /* For now: just use lo as address */
            uint8_t v = cpu->memory[lo];
            cpu_set_reg(cpu, dst, v);
            break;
        }

        /* ── STRI: mem[(hi<<8)|lo] = src ── */
        case 0xF2: {
            uint8_t operands = fetch(cpu);
            consumed = 3;
            uint8_t src = (operands >> 4) & 0x0F;
            uint8_t lo = fetch(cpu);
            consumed = 4;
            uint8_t v = cpu_get_reg(cpu, src);
            cpu->memory[lo] = v;
            break;
        }

        default:
            cpu->halted = true;
            cpu->status = DHAD_ERROR_INVALID_OPCODE;
            break;
        }
        break;
    } /* end case 0xF */

    default:
        cpu->halted = true;
        cpu->status = DHAD_ERROR_INVALID_OPCODE;
        break;
    }

    (void)start_pc;
    return consumed;
}

/* ═══════════════════════════════════════
 *  Run until halted
 * ═══════════════════════════════════════ */

void dhad_cpu_run(DhadCPU *cpu, uint32_t max_cycles) {
    while (!cpu->halted && cpu->cycles < max_cycles) {
        dhad_cpu_step(cpu);
    }
    if (cpu->cycles >= max_cycles && !cpu->halted) {
        cpu->status = DHAD_ERROR_MAX_CYCLES;
    }
}
