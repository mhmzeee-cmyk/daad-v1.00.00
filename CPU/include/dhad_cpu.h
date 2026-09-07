/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  dhad_cpu.h — Unified CPU Core for Dhad Processor (8-bit, v4.0)
 *
 *  This is the SINGLE source of truth for CPU behavior.
 *  All simulators (CLI, GUI, Live, Visual) must use this core.
 *
 *  Verified architecture:
 *    - Data width:  8-bit
 *    - Address:     16-bit (0x0000 - 0xFFFF)
 *    - Registers:   8 general (S0-S7) + ACC
 *    - Flags:       Z(bit0), N(bit1), C(bit2)
 *    - Stack:       32 entries, separate array
 *    - PROG_START:  0x0000
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef DHAD_CPU_H
#define DHAD_CPU_H

#include <stdint.h>
#include <stdbool.h>
#include "dhad_memory_map.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declaration for device bus */
struct DhadDeviceBus;
typedef struct DhadDeviceBus DhadDeviceBus;

/* ═══════════════════════════════════════
 *  CPU-Specific Constants
 * ═══════════════════════════════════════ */

#define DHAD_MAX_CYCLES     100000
#define DHAD_OUTPUT_BUF_SIZE 4096

/* ═══════════════════════════════════════
 *  CPU Status Codes
 * ═══════════════════════════════════════ */

typedef enum {
    DHAD_OK             = 0,    /* Normal execution */
    DHAD_HALTED         = 1,    /* HALT instruction executed */
    DHAD_ERROR_DIV_ZERO = 2,    /* Division by zero */
    DHAD_ERROR_MOD_ZERO = 3,    /* Modulo by zero */
    DHAD_ERROR_STACK_OVERFLOW  = 4,  /* PUSH when stack full */
    DHAD_ERROR_STACK_UNDERFLOW = 5,  /* POP when stack empty */
    DHAD_ERROR_INVALID_OPCODE = 6,  /* Unknown opcode encountered */
    DHAD_ERROR_PC_OUT_OF_RANGE = 7, /* PC outside valid memory */
    DHAD_ERROR_MAX_CYCLES = 8,  /* Cycle limit exceeded */
    DHAD_ERROR_LOAD_FAILED = 9  /* Program load failed (invalid size) */
} DhadStatus;

/** Get human-readable name for a status code */
const char *dhad_status_name(DhadStatus status);

/* ═══════════════════════════════════════
 *  Base Opcodes (4-bit, from byte[0] >> 4)
 * ═══════════════════════════════════════ */

#define DHAD_OP_NOP       0x0
#define DHAD_OP_LOAD_IMM  0x1
#define DHAD_OP_MOV       0x2
#define DHAD_OP_LOAD_MEM  0x3
#define DHAD_OP_STORE_MEM 0x4
#define DHAD_OP_PUSH      0x5
#define DHAD_OP_POP       0x6
#define DHAD_OP_CALL      0x7
#define DHAD_OP_RET_NEG   0x8
#define DHAD_OP_ADD       0x9
#define DHAD_OP_SUB       0xA
#define DHAD_OP_MUL       0xB
#define DHAD_OP_DIV       0xC
#define DHAD_OP_MOD       0xD
#define DHAD_OP_PRINT     0xE
#define DHAD_OP_EXT       0xF

/* Extended opcodes (byte after 0xF0 prefix) */
#define DHAD_EXT_JMP      0x00
#define DHAD_EXT_JZ       0x10
#define DHAD_EXT_JNZ      0x20
#define DHAD_EXT_JC       0x30
#define DHAD_EXT_JN       0x40
#define DHAD_EXT_PRINT_CH 0x50
#define DHAD_EXT_INPUT    0x60
#define DHAD_EXT_XOR      0x70
#define DHAD_EXT_OR       0x80
#define DHAD_EXT_AND      0x90
#define DHAD_EXT_NOT      0xA0
#define DHAD_EXT_SHL      0xB0
#define DHAD_EXT_SHR      0xC0
#define DHAD_EXT_SWAP     0xD0
#define DHAD_EXT_DEC      0xD1
#define DHAD_EXT_INC      0xD2
#define DHAD_EXT_HALT     0xD3
#define DHAD_EXT_CMP      0xE0    /* Phase 12.5: CMP now uses 0xE0-0xE7 (all 8 regs) */
#define DHAD_EXT_EI       0xF3    /* Phase 12.5: relocated from 0xE1 to avoid CMP collision */
#define DHAD_EXT_DI       0xF4    /* Phase 12.5: relocated from 0xE2 to avoid CMP collision */
#define DHAD_EXT_RETI     0xF5    /* Phase 12.5: relocated from 0xE3 to avoid CMP collision */

/* ═══════════════════════════════════════
 *  CPU State Structure
 * ═══════════════════════════════════════ */

typedef struct DhadCPU {
    /* Registers */
    uint8_t  regs[DHAD_REG_COUNT];  /* S0-S7 */
    uint8_t  acc;                    /* Accumulator */
    uint8_t  flags;                  /* Z, N, C */

    /* Program Counter & Stack Pointer */
    uint16_t pc;
    uint8_t  sp;

    /* Memory */
    uint8_t  memory[DHAD_MEM_SIZE];

    /* Stack (separate from memory) */
    uint16_t stack[DHAD_STACK_SIZE];

    /* State */
    bool     halted;
    DhadStatus status;
    uint32_t cycles;

    /* Interrupts */
    bool     int_enabled;
    uint8_t  int_mask;
    uint8_t  int_pending;
    uint16_t ivt_addr;

    /* Output buffer (PRINT/PRINT_CH append here) */
    char     output[DHAD_OUTPUT_BUF_SIZE];
    int      output_len;

    /* Output callback (if set, PRINT/PRINT_CH call this instead of buffering) */
    void   (*output_func)(int value, bool is_char, void *userdata);
    void    *output_userdata;

    /* Input callback (if set, INPUT calls this when keyboard FIFO is empty) */
    int     (*input_func)(void *userdata);
    void    *input_userdata;

    /* Program size (for display purposes) */
    uint16_t prog_size;

    /* Device bus (optional, NULL if not attached) */
    DhadDeviceBus *bus;
} DhadCPU;

/** Check if CPU is halted */
bool dhad_cpu_is_halted(const DhadCPU *cpu);

/** Get cycle count */
uint32_t dhad_cpu_get_cycles(const DhadCPU *cpu);

/** Get ACC register value */
uint8_t dhad_cpu_get_acc(const DhadCPU *cpu);

/* ═══════════════════════════════════════
 *  CPU Lifecycle
 * ═══════════════════════════════════════ */

/** Initialize CPU to clean state */
void dhad_cpu_init(DhadCPU *cpu);

/** Reset CPU state, keep program in memory */
void dhad_cpu_reset(DhadCPU *cpu);

/** Load program binary into memory starting at PROG_START */
int dhad_cpu_load_program(DhadCPU *cpu, const uint8_t *program, uint16_t size);

/* ═══════════════════════════════════════
 *  Device Integration
 * ═══════════════════════════════════════ */

/** Attach a device bus to the CPU. Pass NULL to detach. */
void dhad_cpu_attach_bus(DhadCPU *cpu, DhadDeviceBus *bus);

/** Get the attached device bus (may be NULL) */
DhadDeviceBus *dhad_cpu_get_bus(const DhadCPU *cpu);

/* ═══════════════════════════════════════
 *  Execution
 * ═══════════════════════════════════════ */

/** Execute one instruction. Returns number of bytes consumed (1-4). */
int dhad_cpu_step(DhadCPU *cpu);

/** Run until halted or max_cycles reached */
void dhad_cpu_run(DhadCPU *cpu, uint32_t max_cycles);

/* ═══════════════════════════════════════
 *  Register Access
 * ═══════════════════════════════════════ */

/** Get register value. r=0-7 for S0-S7, r=8 for ACC */
uint8_t dhad_cpu_get_reg(const DhadCPU *cpu, uint8_t r);

/** Set register value. r=0-7 for S0-S7, r=8 for ACC */
void dhad_cpu_set_reg(DhadCPU *cpu, uint8_t r, uint8_t v);

/* ═══════════════════════════════════════
 *  Memory Access (with bounds checking)
 * ═══════════════════════════════════════ */

/** Read byte from memory */
uint8_t dhad_cpu_read_mem(const DhadCPU *cpu, uint16_t addr);

/** Write byte to memory */
void dhad_cpu_write_mem(DhadCPU *cpu, uint16_t addr, uint8_t val);

/* ═══════════════════════════════════════
 *  Stack Operations (with bounds checking)
 * ═══════════════════════════════════════ */

/** Push value to stack. Returns false on overflow. */
bool dhad_cpu_push(DhadCPU *cpu, uint16_t val);

/** Pop value from stack. Returns 0 on underflow. */
uint16_t dhad_cpu_pop(DhadCPU *cpu);

/* ═══════════════════════════════════════
 *  Flags
 * ═══════════════════════════════════════ */

/** Update Z and N flags based on 8-bit result. Does NOT change C. */
void dhad_cpu_update_flags_nz(DhadCPU *cpu, uint8_t result);

/* ═══════════════════════════════════════
 *  Output
 * ═══════════════════════════════════════ */

/** Append formatted number to output buffer */
void dhad_cpu_output_num(DhadCPU *cpu, int value);

/** Append character to output buffer */
void dhad_cpu_output_char(DhadCPU *cpu, char c);

/** Clear output buffer */
void dhad_cpu_output_clear(DhadCPU *cpu);

#ifdef __cplusplus
}
#endif

#endif /* DHAD_CPU_H */
