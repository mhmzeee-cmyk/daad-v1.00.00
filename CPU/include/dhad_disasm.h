/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  dhad_disasm.h — Disassembler for Dhad Processor v4.1
 *
 *  Converts machine code → assembly representation.
 *  Uses ISA definitions from dhad_cpu.h (single source of truth).
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef DHAD_DISASM_H
#define DHAD_DISASM_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ═══════════════════════════════════════
 *  Disassembler Output
 * ═══════════════════════════════════════ */

#define DHAD_DISASM_BUF_SIZE 128

typedef struct {
    char     text[DHAD_DISASM_BUF_SIZE];  /* Assembly text */
    uint8_t  length;                        /* Instruction length in bytes */
    bool     valid;                         /* false if invalid opcode */
    uint8_t  raw[4];                        /* Raw bytes */
    int      raw_len;                       /* Number of raw bytes */
} DhadDisasmResult;

/* ═══════════════════════════════════════
 *  Disassembler API
 * ═══════════════════════════════════════ */

/**
 * Disassemble one instruction from buffer.
 * @param code    Pointer to instruction bytes
 * @param size    Available bytes
 * @param addr    Address of instruction (for display)
 * @param result  Output: disassembled instruction
 * @return Number of bytes consumed, or 0 on error
 */
int dhad_disasm(const uint8_t *code, uint16_t size, uint16_t addr,
                DhadDisasmResult *result);

/**
 * Disassemble a range of memory.
 * @param memory  Full 64KB memory array
 * @param start   Start address
 * @param count   Number of instructions to disassemble
 * @param callback Called for each instruction with address and result
 */
typedef void (*DhadDisasmCallback)(uint16_t addr, const DhadDisasmResult *result, void *userdata);

void dhad_disasm_range(const uint8_t *memory, uint16_t start, int count,
                       DhadDisasmCallback callback, void *userdata);

/**
 * Get instruction length for given opcode.
 * @return 1-4 bytes, or 0 if invalid
 */
int dhad_disasm_instruction_length(uint8_t first_byte, uint8_t second_byte);

/**
 * Get register name by index.
 * @return Arabic name ("س0"-"س7", "مح")
 */
const char* dhad_disasm_reg_name(uint8_t r);

/**
 * Get extended instruction name.
 * @return Arabic mnemonic or NULL
 */
const char* dhad_disasm_ext_name(uint8_t ext);

#ifdef __cplusplus
}
#endif

#endif /* DHAD_DISASM_H */
