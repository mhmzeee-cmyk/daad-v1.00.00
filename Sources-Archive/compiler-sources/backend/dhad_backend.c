/**
 * @file dhad_backend.c
 * @brief DHAD Target Backend — IR → DHAD Assembly (v5 hardened)
 *
 * Phase 12.5-F.1: Hardened Stack Frame + Register Spilling
 *
 * DHAD ISA critical constraints:
 * - All ALU ops work through ACC: add reg => ACC += reg
 * - PUSH/POP only work with ACC
 * - CMP: ACC -= reg, sets flags (destructive to ACC)
 * - SHL/SHR: shift ACC by 1, no operand
 * - NOT: ACC = ~ACC, no operand
 * - MOV dst, src: copies register value
 * - LOAD reg, imm: loads immediate into register (8-bit only)
 * - LDRI dst, addr_hi, addr_lo: indirect load (16-bit address via register pair)
 * - STRI src, addr_hi, addr_lo: indirect store (16-bit address via register pair)
 * - HALT: stops execution
 *
 * Register classification (Phase 12.5-F.1):
 * - S0-S5: Allocatable (6 registers for user values)
 * - S6:    DEDICATED SCRATCH — never holds user values, used for spill/reload addressing
 * - S7:    Frame Pointer (FP) — callee-saved, points to current frame base low byte
 * - ACC:   ALU operations, return value
 * - HIGH BYTE: Always 0xE0 (frame area at 0xE000-0xE0FF)
 *
 * Frame design (Phase 12.5-F.1):
 * - Frame area: 0xE000-0xE0FF (256 bytes, hardcoded)
 * - Frame grows upward from 0xE000
 * - Global frame counter (g_frame_size) accumulates across functions
 * - Spill addresses are ABSOLUTE: 0xE000 + spill_offset
 * - S7 = frame pointer low byte (high byte is always 0xE0)
 * - Maximum total frame usage: 256 bytes across ALL functions
 *
 * Scratch register policy:
 * - S6 is NEVER allocated to user values
 * - S6 is NEVER a spill victim
 * - S6 is always available for address computation
 * - Pointer operations use S5 (high byte) + S6 (low byte) as documented limitation
 */

#include "../backend/backend_interface.h"
#include "../ir/ir_function.h"
#include "../ir/ir_module.h"
#include "../ir/ir_basic_block.h"
#include "../ir/ir_instruction.h"
#include "../ir/ir_value.h"
#include "../ir/ir_types.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ═══════════════════════════════════════
 *  DHAD Register Names (English for assembler compatibility)
 * ═══════════════════════════════════════ */

static const char* DHAD_REG[] = {
    "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "acc"
};
#define ACC_INDEX 8

/* Register classification (Phase 12.5-F.1) */
#define DHAD_NUM_REGS    6    /* S0-S5 allocatable (6 registers) */
#define DHAD_SCRATCH_REG 6    /* S6 = dedicated scratch */
#define DHAD_FRAME_REG   7    /* S7 = frame pointer */

/* Frame area constants (Phase 12.5-F.1) */
#define DHAD_FRAME_BASE_ADDR  0xE000  /* Start of frame area in memory */
#define DHAD_FRAME_HIGH_BYTE  0xE0    /* High byte of frame area (fixed) */
#define DHAD_FRAME_MAX_SIZE   256     /* Maximum frame area size (0xE000-0xE0FF) */
#define DHAD_FRAME_WARN_SIZE  240     /* Warning threshold (leave room for growth) */

#define MAX_IR_REGS 256

/* Function-scoped label prefix (Phase 12.5-F.3).
 * IR basic blocks use generic names (if_then, if_merge, __cmp_true_4, etc.)
 * that collide across functions. We prefix each emitted label with
 * "funcname_" to make them unique in the assembler's global namespace.
 * Set at the start of each function emission. */
static char g_func_label_prefix[128];
static char g_label_buf[256];

static const char* func_label(const char* ir_label) {
    snprintf(g_label_buf, sizeof(g_label_buf), "%s_%s", g_func_label_prefix, ir_label);
    return g_label_buf;
}

/* Helper: format a function-prefixed label with a numeric suffix */
static const char* cmp_label(const char* kind, int uid) {
    static char _buf[256];
    snprintf(_buf, sizeof(_buf), "%s_%s_%d", g_func_label_prefix, kind, uid);
    return _buf;
}

/* IR register → DHAD register mapping (Phase 12.5-F) */
typedef struct {
    int ir_reg;
    int dhad_reg;       /* 0-6 for S0-S6, -1 if spilled */
    int spill_offset;   /* Offset from frame base if spilled */
    int is_loaded;
    int is_dirty;
} DHADRegMapping;

/* Forward declarations for spilling allocator */
static void reg_map_clear(void);
static DHADRegMapping* reg_map_find(int ir_reg);
static DHADRegMapping* reg_map_get_or_add(int ir_reg);
static int ensure_in_reg(int ir_reg, FILE* out);

/* IR register → DHAD register mapping (Phase 12.5-F.1).
 * Fast path: IR regs 1-6 → S0-S5 (allocatable pool).
 * IR regs 7+ go through the slow allocator path.
 * S6 (scratch) and S7 (frame pointer) are NEVER assigned via fast path. */
static int ir_reg_to_dhad(int ir_id) {
    if (ir_id >= 1 && ir_id <= DHAD_NUM_REGS) return ir_id - 1;
    return -1;
}

/* Phase 12.5-F: Ensure IR register is in a DHAD register, spilling if needed.
 * Returns DHAD register index (0-6 for S0-S6, 8 for ACC, -1 on error).
 * When all registers are full, spills the longest-lived value to memory.
 *
 * Phase 12.5-F.4: Fast path for IR regs 1-6 → S0-S5. After a CALL, the
 * callee clobbers S0-S5. The CALL handler invalidates reg_map entries
 * (is_loaded=0). We MUST check reg_map: if the entry exists but is NOT
 * loaded, the register holds a stale value and we must reload from frame
 * via ensure_in_reg. If no entry exists (first use, e.g., ALLOCA init),
 * we trust the fast path — the calling convention guarantees the register
 * is available. */
static int ensure_dhad_reg(int ir_reg, FILE* out) {
    int fast = ir_reg_to_dhad(ir_reg);
    if (fast >= 0) {
        DHADRegMapping* m = reg_map_find(ir_reg);
        if (!m) return fast;
        if (m->is_loaded && m->dhad_reg == fast) return fast;
        return ensure_in_reg(ir_reg, out);
    }
    return ensure_in_reg(ir_reg, out);
}

/* ═══════════════════════════════════════
 *  DHAD Register Allocator with Spilling
 * ═══════════════════════════════════════ */

/* Per-function state */
static int g_frame_size = 0;
static int g_spill_count = 0;
static int g_function_frame_base = 0;

/* Phase 12.5-F.6: FP-relative frame addressing for recursion isolation.
 * g_current_frame_size: total bytes for this function's frame (computed in pass 1).
 * g_use_fp_relative: 1 = spill/reload use FP-relative addressing (pass 2). */
static int g_current_frame_size = 0;
static int g_use_fp_relative = 0;

static DHADRegMapping g_reg_map[MAX_IR_REGS];
static int g_reg_map_count = 0;

static void reg_map_clear(void) {
    g_reg_map_count = 0;
    /* NOTE: g_frame_size is NOT reset — it accumulates across functions
     * to ensure each function's spill slots are at unique absolute addresses.
     * This is critical for recursion and nested calls. */
    g_spill_count = 0;
}

static DHADRegMapping* reg_map_find(int ir_reg) {
    for (int i = 0; i < g_reg_map_count; i++) {
        if (g_reg_map[i].ir_reg == ir_reg) return &g_reg_map[i];
    }
    return NULL;
}

static DHADRegMapping* reg_map_get_or_add(int ir_reg) {
    DHADRegMapping* m = reg_map_find(ir_reg);
    if (m) return m;
    if (g_reg_map_count >= MAX_IR_REGS) return NULL;
    m = &g_reg_map[g_reg_map_count++];
    m->ir_reg = ir_reg;
    m->dhad_reg = -1;
    m->spill_offset = -1;
    m->is_loaded = 0;
    m->is_dirty = 0;
    return m;
}

/* Find the IR register mapped to a specific DHAD register (S0-S5).
 * Returns the mapping if found and loaded, NULL otherwise. */
static DHADRegMapping* reg_map_get_by_dhad(int dhad_reg) {
    for (int i = 0; i < g_reg_map_count; i++) {
        if (g_reg_map[i].dhad_reg == dhad_reg && g_reg_map[i].is_loaded)
            return &g_reg_map[i];
    }
    return NULL;
}

static int alloc_spill_slot(void) {
    if (g_frame_size >= DHAD_FRAME_MAX_SIZE - 2) {
        /* Frame overflow: cannot allocate more spill slots */
        return -1;
    }
    if (g_frame_size >= DHAD_FRAME_WARN_SIZE) {
        fprintf(stderr, "warning: frame usage %d bytes (max %d)\n",
                g_frame_size, DHAD_FRAME_MAX_SIZE);
    }
    /* Phase 12.5-F.6: Offset relative to this function frame base */
    int offset = g_frame_size - g_function_frame_base;
    g_frame_size += 1;
    g_spill_count++;
    return offset;
}

static int find_free_dhad_reg(void) {
    int used[DHAD_NUM_REGS] = {0};
    for (int i = 0; i < g_reg_map_count; i++) {
        if (g_reg_map[i].dhad_reg >= 0 && g_reg_map[i].dhad_reg < DHAD_NUM_REGS) {
            used[g_reg_map[i].dhad_reg] = 1;
        }
    }
    for (int i = 0; i < DHAD_NUM_REGS; i++) {
        if (!used[i]) return i;
    }
    return -1;
}

/* Spill: store value from DHAD reg to memory frame area (Phase 12.5-F.1/F.6).
 * Uses S6 as dedicated scratch for address high byte (always 0xE0).
 *
 * Absolute mode (g_use_fp_relative=0): Address = DHAD_FRAME_BASE_ADDR + spill_offset.
 * FP-relative mode (g_use_fp_relative=1): Address = 0xE000 + FP + relative_offset
 *   where relative_offset = spill_offset - frame_size (always negative, 8-bit wrap).
 *   DHAD 8-bit arithmetic: load acc, (rel & 0xFF); add s7 => ACC = rel + FP (wraps).
 *
 * STRI src, s6, acc: mem[(S6<<8)|ACC] = src */
static void spill_to_frame(DHADRegMapping* m, FILE* out) {
    if (m->dhad_reg < 0 || m->dhad_reg >= DHAD_NUM_REGS) return;
    if (m->spill_offset < 0) {
        m->spill_offset = alloc_spill_slot();
        if (m->spill_offset < 0) return; /* frame overflow */
    }

    if (g_use_fp_relative) {
        /* FP-relative: relative_offset = spill_offset - frame_size (negative, 8-bit wrap) */
        int rel = m->spill_offset - g_current_frame_size;
        unsigned char rel_u = (unsigned char)(rel & 0xFF);
        fprintf(out, "  # spill s%d -> frame[%d] (rel=%d, FP-relative)\n",
                m->dhad_reg, m->spill_offset, rel);
        fprintf(out, "  load s6, %d\n", DHAD_FRAME_HIGH_BYTE);
        fprintf(out, "  load acc, %d\n", rel_u);
        fprintf(out, "  add s7\n");
        fprintf(out, "  stri s%d, s6, acc\n", m->dhad_reg);
    } else {
        /* Absolute addressing (pass 1 or non-recursive functions) */
        int addr = DHAD_FRAME_BASE_ADDR + m->spill_offset;
        fprintf(out, "  # spill s%d -> frame[%d] @0x%04X\n", m->dhad_reg, m->spill_offset, addr);
        fprintf(out, "  load s6, %d\n", DHAD_FRAME_HIGH_BYTE);
        fprintf(out, "  load acc, %d\n", m->spill_offset & 0xFF);
        fprintf(out, "  stri s%d, s6, acc\n", m->dhad_reg);
    }

    m->is_loaded = 0;
    m->is_dirty = 0;
}

/* Reload: load value from memory frame area to DHAD reg (Phase 12.5-F.1/F.6).
 * Uses S6 as dedicated scratch for address high byte (always 0xE0).
 *
 * Absolute mode: Address = DHAD_FRAME_BASE_ADDR + spill_offset.
 * FP-relative mode: Address = 0xE000 + FP + relative_offset.
 * LDRI dst, s6, acc: dst = mem[(S6<<8)|ACC] */
static void reload_from_frame(DHADRegMapping* m, int target_reg, FILE* out) {
    if (m->spill_offset < 0) return;
    if (target_reg < 0 || target_reg >= DHAD_NUM_REGS) return;

    if (g_use_fp_relative) {
        int rel = m->spill_offset - g_current_frame_size;
        unsigned char rel_u = (unsigned char)(rel & 0xFF);
        fprintf(out, "  # reload s%d <- frame[%d] (rel=%d, FP-relative)\n",
                target_reg, m->spill_offset, rel);
        fprintf(out, "  load s6, %d\n", DHAD_FRAME_HIGH_BYTE);
        fprintf(out, "  load acc, %d\n", rel_u);
        fprintf(out, "  add s7\n");
        fprintf(out, "  ldri s%d, s6, acc\n", target_reg);
    } else {
        fprintf(out, "  # reload s%d <- frame[%d] @0x%04X\n", target_reg, m->spill_offset,
                DHAD_FRAME_BASE_ADDR + m->spill_offset);
        fprintf(out, "  load s6, %d\n", DHAD_FRAME_HIGH_BYTE);
        fprintf(out, "  load acc, %d\n", m->spill_offset & 0xFF);
        fprintf(out, "  ldri s%d, s6, acc\n", target_reg);
    }

    m->dhad_reg = target_reg;
    m->is_loaded = 1;
}

static int ensure_in_reg(int ir_reg, FILE* out) {
    DHADRegMapping* m = reg_map_get_or_add(ir_reg);
    if (!m) return -1;
    if (m->dhad_reg >= 0 && m->is_loaded && m->dhad_reg < DHAD_NUM_REGS)
        return m->dhad_reg;

    int free_reg = find_free_dhad_reg();
    if (free_reg < 0) {
        /* Spill longest-lived */
        DHADRegMapping* victim = NULL;
        for (int i = 0; i < g_reg_map_count; i++) {
            if (g_reg_map[i].dhad_reg >= 0 && g_reg_map[i].dhad_reg < DHAD_NUM_REGS &&
                g_reg_map[i].is_loaded && g_reg_map[i].ir_reg != ir_reg) {
                if (!victim || g_reg_map[i].ir_reg > victim->ir_reg)
                    victim = &g_reg_map[i];
            }
        }
        if (victim) {
            spill_to_frame(victim, out);
            free_reg = victim->dhad_reg;
            victim->dhad_reg = -1;
        } else {
            return -1;
        }
    }

    if (m->spill_offset >= 0) {
        m->dhad_reg = free_reg;
        reload_from_frame(m, free_reg, out);
    } else {
        m->dhad_reg = free_reg;
        m->is_loaded = 1;
    }
    return free_reg;
}

/* Alloca-to-register tracking: maps IR alloca register id → DHAD register id */
#define ALLOCA_MAP_SIZE 64
static int alloca_map_ir[ALLOCA_MAP_SIZE];
static int alloca_map_dhad[ALLOCA_MAP_SIZE];
static int alloca_map_count = 0;

static void alloca_map_clear(void) {
    alloca_map_count = 0;
}

static void alloca_map_add(int ir_id, int dhad_id) {
    if (alloca_map_count < ALLOCA_MAP_SIZE) {
        alloca_map_ir[alloca_map_count] = ir_id;
        alloca_map_dhad[alloca_map_count] = dhad_id;
        alloca_map_count++;
    }
}

static int __attribute__((unused)) alloca_map_lookup(int ir_id) {
    for (int i = 0; i < alloca_map_count; i++) {
        if (alloca_map_ir[i] == ir_id) return alloca_map_dhad[i];
    }
    return -1;
}

static const char* reg_name(int idx) {
    if (idx >= 0 && idx <= 8) return DHAD_REG[idx];
    return DHAD_REG[0];
}

const char* dhad_reg_name(int reg_id) {
    if (reg_id >= 1 && reg_id <= 9) return DHAD_REG[reg_id - 1];
    return "s0";
}

const char* dhad_type_suffix(IRType type) {
    (void)type;
    return "";
}

/* ═══════════════════════════════════════
 *  DHAD String Table
 * ═══════════════════════════════════════ */

#define DHAD_MAX_STR 256
static char dhad_str_lbl[DHAD_MAX_STR][64];
static char dhad_str_dat[DHAD_MAX_STR][256];
static int  dhad_str_cnt = 0;

/* ═══════════════════════════════════════
 *  Emit IR value as DHAD operand
 * ═══════════════════════════════════════ */

static void emit_val(IRValue val, FILE* out) {
    switch (val.kind) {
    case IR_VALUE_CONSTANT_INT:
        fprintf(out, "%lld", val.as.int_val);
        break;
    case IR_VALUE_CONSTANT_FLOAT:
        fprintf(out, "%d", (int)val.as.float_val);
        break;
    case IR_VALUE_CONSTANT_STRING: {
        /* String labels are emitted from IR module's table (dhad_emit_module_strings).
         * Just reference the label in code. */
        fprintf(out, "%s", val.as.string_val ? val.as.string_val : "0");
        break;
    }
    case IR_VALUE_REGISTER: {
        int idx = ir_reg_to_dhad(val.id);
        fprintf(out, "%s", reg_name(idx >= 0 ? idx : 0));
        break;
    }
    case IR_VALUE_LABEL:
        /* Phase 12.5-F.3: Prefix IR labels with function name to avoid
         * collisions across functions (e.g., "if_merge" in factorial vs deepest). */
        fprintf(out, "%s", val.as.label_val ? func_label(val.as.label_val) : "_null");
        break;
    case IR_VALUE_NULL:
        fprintf(out, "0");
        break;
    }
}

/* ═══════════════════════════════════════
 *  Helper: load value into ACC or register
 * ═══════════════════════════════════════ */

static void load_to_acc(IRValue val, FILE* out) {
    if (val.kind == IR_VALUE_REGISTER) {
        int idx = ensure_dhad_reg(val.id, out);
        if (idx == ACC_INDEX) return;
        if (idx >= 0) {
            fprintf(out, "  mov acc, %s\n", reg_name(idx));
            return;
        }
    }
    fprintf(out, "  load acc, ");
    emit_val(val, out);
    fprintf(out, "\n");
}

static void load_to_reg(IRValue val, int dst_idx, FILE* out) {
    if (val.kind == IR_VALUE_REGISTER) {
        int src_idx = ensure_dhad_reg(val.id, out);
        if (src_idx == dst_idx) return;
        if (src_idx >= 0) {
            fprintf(out, "  mov %s, %s\n", reg_name(dst_idx), reg_name(src_idx));
            return;
        }
    }
    fprintf(out, "  load %s, ", reg_name(dst_idx));
    emit_val(val, out);
    fprintf(out, "\n");
}

/* ═══════════════════════════════════════
 *  Emit ALU operation: load left into ACC, op right, store to dst
 * ═══════════════════════════════════════ */

static void emit_alu(const char* op, IRValue left, IRValue right, int dst_idx, FILE* out) {
    load_to_acc(left, out);

    if (right.kind == IR_VALUE_REGISTER) {
        int r2 = ensure_dhad_reg(right.id, out);
        if (r2 == ACC_INDEX) {
            fprintf(out, "  mov s4, acc\n");
            load_to_acc(left, out);
            fprintf(out, "  %s s4\n", op);
        } else if (r2 >= 0) {
            fprintf(out, "  %s %s\n", op, reg_name(r2));
        }
    } else {
        int scratch = (dst_idx >= 0 && dst_idx != ACC_INDEX) ? dst_idx : 4;
        fprintf(out, "  load %s, ", reg_name(scratch));
        emit_val(right, out);
        fprintf(out, "\n");
        fprintf(out, "  %s %s\n", op, reg_name(scratch));
    }

    if (dst_idx >= 0 && dst_idx != ACC_INDEX)
        fprintf(out, "  mov %s, acc\n", reg_name(dst_idx));
}

/* ═══════════════════════════════════════
 *  Emit single IR instruction → DHAD
 * ═══════════════════════════════════════ */

static int emit_inst(IRInstruction* inst, FILE* out) {
    if (!inst || !out) return 0;

    /* Phase 12.5-F.5: Don't allocate dst_idx for built-in calls that don't
     * return values (print, print_ch, halt). Allocating a register wastes it
     * and cascades into wrong spills/reloads for subsequent instructions. */
    int dst_idx = -1;
    if (inst->result.kind == IR_VALUE_REGISTER) {
        int skip_dst = 0;
        if (inst->opcode == IR_OP_CALL && inst->operand_count >= 1) {
            const char* fn = inst->operands[0].as.label_val;
            if (fn && (strcmp(fn, "اطبع") == 0 || strcmp(fn, "print") == 0 ||
                       strcmp(fn, "اطبع_حرف") == 0 || strcmp(fn, "print_ch") == 0 ||
                       strcmp(fn, "توقف") == 0 || strcmp(fn, "halt") == 0))
                skip_dst = 1;
        }
        if (!skip_dst)
            dst_idx = ensure_dhad_reg(inst->result.id, out);
    }

    switch (inst->opcode) {

    case IR_OP_NOP:
        /* Phase 12.5-F.3: If NOP has two register operands, emit as MOV.
         * Some optimization passes convert stores to NOP with operands preserved. */
        if (inst->operand_count >= 2 &&
            inst->operands[0].kind == IR_VALUE_REGISTER &&
            inst->operands[1].kind == IR_VALUE_REGISTER) {
            int nop_src = ensure_dhad_reg(inst->operands[1].id, out);
            int nop_dst = ensure_dhad_reg(inst->operands[0].id, out);
            if (nop_src >= 0 && nop_dst >= 0 && nop_src != nop_dst)
                fprintf(out, "  mov %s, %s\n", reg_name(nop_dst), reg_name(nop_src));
        }
        break;

    /* ── MOV: dst = src ── */
    case IR_OP_MOV:
        if (dst_idx >= 0)
            load_to_reg(inst->operands[0], dst_idx, out);
        break;

    /* ── LOAD: dst = *ptr (dereference pointer via LDRI if not alloca-mapped)
     * Phase 12.5: Uses register-pair addressing for full 16-bit memory access.
     * addr16 = (get_reg(addr_hi) << 8) | get_reg(addr_lo)
     * If the source is alloca-mapped (local var in register), use MOV instead.
     * For 16-bit addresses: addr_hi = ptr >> 8, addr_lo = ptr & 0xFF.
     * We use s5 as scratch for the high byte. */
    case IR_OP_LOAD:
        if (dst_idx >= 0) {
            if (inst->operands[0].kind == IR_VALUE_REGISTER) {
                int alloca_dhad = alloca_map_lookup(inst->operands[0].id);
                if (alloca_dhad >= 0) {
                    /* Phase 12.5-F.4: Check if alloca's register is still valid.
                     * After CALL, registers are clobbered. alloca_map is NOT
                     * invalidated, so we must check reg_map for validity.
                     * If invalidated, reload from frame via ensure_dhad_reg. */
                    DHADRegMapping* m = reg_map_find(inst->operands[0].id);
                    if (m && m->is_loaded && m->dhad_reg >= 0 && m->dhad_reg < DHAD_NUM_REGS) {
                        /* Still valid — simple MOV */
                        if (dst_idx != m->dhad_reg)
                            fprintf(out, "  mov %s, %s\n", reg_name(dst_idx), reg_name(m->dhad_reg));
                        break;
                    }
                    /* Invalidated — reload from frame */
                    int src = ensure_dhad_reg(inst->operands[0].id, out);
                    if (src >= 0 && src != dst_idx)
                        fprintf(out, "  mov %s, %s\n", reg_name(dst_idx), reg_name(src));
                    break;
                }
                /* Pointer dereference: use LDRI with address in register pair.
                 * For addresses that fit in 8 bits: high=0, low=ptr.
                 * For larger addresses: high = ptr >> 8 (computed via shift), low = ptr & 0xFF.
                 * Phase 12.5-F.1: Use S6 (scratch) for high byte instead of S5. */
                int ptr_idx = ir_reg_to_dhad(inst->operands[0].id);
                if (ptr_idx >= 0) {
                    /* Extract high byte: s6 = ptr >> 8 */
                    if (ptr_idx == ACC_INDEX) {
                        fprintf(out, "  mov s4, acc\n");
                        fprintf(out, "  mov acc, s4\n");
                    }
                    fprintf(out, "  mov s6, %s\n", reg_name(ptr_idx));
                    fprintf(out, "  mov acc, s6\n");
                    /* Shift right 8 times to get high byte */
                    for (int i = 0; i < 8; i++)
                        fprintf(out, "  shr\n");
                    fprintf(out, "  mov s6, acc\n");
                    /* Low byte: ptr & 0xFF (mask in dst) */
                    if (ptr_idx != dst_idx)
                        fprintf(out, "  mov %s, %s\n", reg_name(dst_idx), reg_name(ptr_idx));
                    fprintf(out, "  load acc, 0xFF\n");
                    fprintf(out, "  and %s\n", reg_name(dst_idx));
                    fprintf(out, "  mov %s, acc\n", reg_name(dst_idx));
                    fprintf(out, "  ldri %s, s6, %s\n", reg_name(dst_idx), reg_name(dst_idx));
                    break;
                }
            }
            load_to_reg(inst->operands[0], dst_idx, out);
        }
        break;

    /* ── STORE: *ptr = value (dereference pointer via STRI if not alloca-mapped)
     * Phase 12.5: Uses register-pair addressing for full 16-bit memory access.
     * addr16 = (addr_hi << 8) | addr_lo. Split pointer into s5 (high) and ptr_reg (low). */
    case IR_OP_STORE: {
        /* operands[0] = address/destination, operands[1] = value to store */
        if (inst->operands[0].kind == IR_VALUE_REGISTER) {
            int alloca_dhad = alloca_map_lookup(inst->operands[0].id);
            if (alloca_dhad >= 0) {
                /* Local variable stored in register — simple MOV */
                load_to_reg(inst->operands[1], alloca_dhad, out);
                /* Phase 12.5-F.4: Update reg_map to reflect that the alloca's
                 * register now holds a valid value. Without this, a subsequent
                 * LOAD from the same alloca would reload from the stale frame
                 * instead of using the freshly-written register. */
                DHADRegMapping* m = reg_map_find(inst->operands[0].id);
                if (m) {
                    m->dhad_reg = alloca_dhad;
                    m->is_loaded = 1;
                }
                break;
            }
            /* Pointer dereference: use STRI with 16-bit address pair */
            int ptr_idx = ir_reg_to_dhad(inst->operands[0].id);
            int val_idx = -1;
            if (inst->operands[1].kind == IR_VALUE_REGISTER)
                val_idx = ir_reg_to_dhad(inst->operands[1].id);
            if (ptr_idx >= 0 && val_idx >= 0) {
                /* Phase 12.5-F.1: Compute 16-bit address for STRI.
                 * S6 = high byte (scratch, always safe).
                 * For low byte: check if val_idx==S5 (can double as low byte),
                 * otherwise save/reload S5 if it's a live user value. */
                int addr_lo = -1;
                int need_s5_save = 0;
                int save_slot = -1;

                if (val_idx == 5) {
                    /* Value is in S5 — use it directly as both value AND low byte.
                     * STRI s5, s6, s5: store S5 at addr (S6<<8)|S5. Correct. */
                    addr_lo = 5;
                } else {
                    /* Check if S5 holds a live user value */
                    DHADRegMapping* s5_map = reg_map_get_by_dhad(5);
                    if (s5_map && s5_map->is_loaded) {
                        need_s5_save = 1;
                        save_slot = alloc_spill_slot();
                        if (save_slot >= 0) {
                            /* Save S5 to frame: STRI s5, s6, acc */
                            fprintf(out, "  load s6, %d\n", DHAD_FRAME_HIGH_BYTE);
                            fprintf(out, "  load acc, %d\n", save_slot & 0xFF);
                            fprintf(out, "  stri s5, s6, acc\n");
                        }
                    }
                    addr_lo = 5; /* Use S5 as low byte */
                }

                /* High byte = ptr >> 8 → S6 */
                fprintf(out, "  mov s6, %s\n", reg_name(ptr_idx));
                fprintf(out, "  mov acc, s6\n");
                for (int i = 0; i < 8; i++)
                    fprintf(out, "  shr\n");
                fprintf(out, "  mov s6, acc\n");

                /* Low byte = ptr & 0xFF → addr_lo (S5 or val_idx) */
                if (addr_lo != ptr_idx) {
                    fprintf(out, "  mov %s, %s\n", reg_name(addr_lo), reg_name(ptr_idx));
                }
                fprintf(out, "  load acc, 0xFF\n");
                fprintf(out, "  and %s\n", reg_name(addr_lo));
                fprintf(out, "  mov %s, acc\n", reg_name(addr_lo));

                /* STRI val, s6(high), addr_lo(low) */
                fprintf(out, "  stri %s, s6, %s\n", reg_name(val_idx), reg_name(addr_lo));

                /* Restore S5 if we saved it */
                if (need_s5_save && save_slot >= 0) {
                    fprintf(out, "  load s6, %d\n", DHAD_FRAME_HIGH_BYTE);
                    fprintf(out, "  load acc, %d\n", save_slot & 0xFF);
                    fprintf(out, "  ldri s5, s6, acc\n");
                }
                break;
            }
        }
        int store_dst = -1;
        int val_idx = -1;
        if (inst->operands[0].kind == IR_VALUE_REGISTER)
            store_dst = ir_reg_to_dhad(inst->operands[0].id);
        if (inst->operands[1].kind == IR_VALUE_REGISTER)
            val_idx = ir_reg_to_dhad(inst->operands[1].id);
        if (store_dst >= 0 && val_idx >= 0) {
            fprintf(out, "  mov %s, %s\n", reg_name(store_dst), reg_name(val_idx));
        } else if (val_idx >= 0) {
            fprintf(out, "  st %s, ", reg_name(val_idx));
            emit_val(inst->operands[0], out);
            fprintf(out, "\n");
        }
        break;
    }

    /* ── ALLOCA: init register to 0 and track alloca→register mapping ──
     * Phase 12.5-F: Register pressure aware — spills if needed */
    case IR_OP_ALLOCA:
        if (dst_idx >= 0) {
            fprintf(out, "  load %s, 0\n", reg_name(dst_idx));
            alloca_map_add(inst->result.id, dst_idx);
            /* Track in new allocator too */
            DHADRegMapping* m = reg_map_get_or_add(inst->result.id);
            if (m) {
                m->dhad_reg = dst_idx;
                m->is_loaded = 1;
            }
        }
        break;

    /* ═══════════════════════════════════════
     *  Arithmetic: ACC = left op right, store to dst
     * ═══════════════════════════════════════ */
    case IR_OP_ADD:
        if (dst_idx >= 0) emit_alu("add", inst->operands[0], inst->operands[1], dst_idx, out);
        break;
    case IR_OP_SUB:
        if (dst_idx >= 0) emit_alu("sub", inst->operands[0], inst->operands[1], dst_idx, out);
        break;
    case IR_OP_MUL:
        if (dst_idx >= 0) emit_alu("mul", inst->operands[0], inst->operands[1], dst_idx, out);
        break;
    case IR_OP_DIV:
        if (dst_idx >= 0) emit_alu("div", inst->operands[0], inst->operands[1], dst_idx, out);
        break;
    case IR_OP_MOD:
        if (dst_idx >= 0) emit_alu("mod", inst->operands[0], inst->operands[1], dst_idx, out);
        break;
    case IR_OP_AND:
        if (dst_idx >= 0) emit_alu("and", inst->operands[0], inst->operands[1], dst_idx, out);
        break;
    case IR_OP_OR:
        if (dst_idx >= 0) emit_alu("or", inst->operands[0], inst->operands[1], dst_idx, out);
        break;
    case IR_OP_XOR:
        if (dst_idx >= 0) emit_alu("xor", inst->operands[0], inst->operands[1], dst_idx, out);
        break;

    /* ── SHL/SHR: shift ACC by amount ── */
    case IR_OP_SHL:
        if (dst_idx >= 0) {
            load_to_acc(inst->operands[0], out);
            if (inst->operands[1].kind == IR_VALUE_CONSTANT_INT) {
                long long amt = inst->operands[1].as.int_val;
                if (amt == 1) {
                    fprintf(out, "  shl\n");
                } else if (amt > 1 && amt <= 8) {
                    fprintf(out, "  load s6, %lld\n", amt);
                    fprintf(out, "__shl_loop_%d:\n", inst->result.id);
                    fprintf(out, "  shl\n");
                    fprintf(out, "  dec s6\n");
                    fprintf(out, "  jnz __shl_loop_%d\n", inst->result.id);
                }
            } else {
                int amt_idx = ir_reg_to_dhad(inst->operands[1].id);
                if (amt_idx >= 0 && amt_idx != ACC_INDEX) {
                    fprintf(out, "  mov s6, %s\n", reg_name(amt_idx));
                    fprintf(out, "__shl_loop_%d:\n", inst->result.id);
                    fprintf(out, "  shl\n");
                    fprintf(out, "  dec s6\n");
                    fprintf(out, "  jnz __shl_loop_%d\n", inst->result.id);
                }
            }
            if (dst_idx >= 0 && dst_idx != ACC_INDEX)
                fprintf(out, "  mov %s, acc\n", reg_name(dst_idx));
        }
        break;

    case IR_OP_SHR:
        if (dst_idx >= 0) {
            load_to_acc(inst->operands[0], out);
            if (inst->operands[1].kind == IR_VALUE_CONSTANT_INT) {
                long long amt = inst->operands[1].as.int_val;
                if (amt == 1) {
                    fprintf(out, "  shr\n");
                } else if (amt > 1 && amt <= 8) {
                    fprintf(out, "  load s6, %lld\n", amt);
                    fprintf(out, "__shr_loop_%d:\n", inst->result.id);
                    fprintf(out, "  shr\n");
                    fprintf(out, "  dec s6\n");
                    fprintf(out, "  jnz __shr_loop_%d\n", inst->result.id);
                }
            } else {
                int amt_idx = ir_reg_to_dhad(inst->operands[1].id);
                if (amt_idx >= 0 && amt_idx != ACC_INDEX) {
                    fprintf(out, "  mov s6, %s\n", reg_name(amt_idx));
                    fprintf(out, "__shr_loop_%d:\n", inst->result.id);
                    fprintf(out, "  shr\n");
                    fprintf(out, "  dec s6\n");
                    fprintf(out, "  jnz __shr_loop_%d\n", inst->result.id);
                }
            }
            if (dst_idx >= 0 && dst_idx != ACC_INDEX)
                fprintf(out, "  mov %s, acc\n", reg_name(dst_idx));
        }
        break;

    /* ── Unary ── */
    case IR_OP_NEG:
        if (dst_idx >= 0) {
            load_to_acc(inst->operands[0], out);
            fprintf(out, "  neg\n");
            if (dst_idx != ACC_INDEX)
                fprintf(out, "  mov %s, acc\n", reg_name(dst_idx));
        }
        break;

    case IR_OP_NOT:
        if (dst_idx >= 0) {
            load_to_acc(inst->operands[0], out);
            fprintf(out, "  not\n");
            if (dst_idx != ACC_INDEX)
                fprintf(out, "  mov %s, acc\n", reg_name(dst_idx));
        }
        break;

    /* ── CMP: ACC = left - right, set flags, store 0/1 result to dst ── */
    case IR_OP_CMP: {
        load_to_acc(inst->operands[0], out);
        if (inst->operands[1].kind == IR_VALUE_REGISTER) {
            int r2 = ir_reg_to_dhad(inst->operands[1].id);
            if (r2 == ACC_INDEX) {
                fprintf(out, "  mov s4, acc\n");
                load_to_acc(inst->operands[0], out);
                fprintf(out, "  cmp s4\n");
            } else if (r2 >= 0) {
                fprintf(out, "  cmp %s\n", reg_name(r2));
            }
    } else {
        /* CMP ext 0xE1-E3 conflict with EI/DI/RETI, so scratch must be s0/s4-s7 */
        int scratch;
        if (dst_idx >= 0 && dst_idx != ACC_INDEX && dst_idx != 1 && dst_idx != 2 && dst_idx != 3)
            scratch = dst_idx;
        else
            scratch = 4;
        fprintf(out, "  load %s, ", reg_name(scratch));
        emit_val(inst->operands[1], out);
        fprintf(out, "\n");
        fprintf(out, "  cmp %s\n", reg_name(scratch));
    }
        /* Generate correct flag check based on comparison operator.
         * DHAD CMP: ACC -= reg, sets Z (equal), N (bit 7), C (borrow).
         * Z=1 means equal, N=1 means negative, C=1 means unsigned borrow.
         * For LT: use N flag (negative after subtract)
         * For LE: use Z or N flag
         * For GT: use !Z and !N
         * For GE: use !N
         * For EQ: use Z
         * For NE: use !Z
         * After CMP, flags are set. We check them BEFORE any LOAD/MOV. */
        if (dst_idx >= 0) {
            int uid = inst->result.id;
            switch (inst->compare_op) {
            case IR_CMP_EQ:
                fprintf(out, "  jz %s\n", cmp_label("__cmp_true", uid));
                break;
            case IR_CMP_NE:
                fprintf(out, "  jnz %s\n", cmp_label("__cmp_true", uid));
                break;
            case IR_CMP_LT:
                fprintf(out, "  jc %s\n", cmp_label("__cmp_true", uid));
                break;
            case IR_CMP_LE:
                fprintf(out, "  jz %s\n", cmp_label("__cmp_true", uid));
                fprintf(out, "  jc %s\n", cmp_label("__cmp_true", uid));
                break;
            case IR_CMP_GT:
                fprintf(out, "  jz %s\n", cmp_label("__cmp_false", uid));
                fprintf(out, "  jc %s\n", cmp_label("__cmp_false", uid));
                fprintf(out, "  load %s, 1\n", reg_name(dst_idx));
                fprintf(out, "  jmp %s\n", cmp_label("__cmp_done", uid));
                fprintf(out, "%s:\n", cmp_label("__cmp_false", uid));
                fprintf(out, "  load %s, 0\n", reg_name(dst_idx));
                fprintf(out, "  jmp %s\n", cmp_label("__cmp_done", uid));
                fprintf(out, "%s:\n", cmp_label("__cmp_done", uid));
                fprintf(out, "  mov acc, %s\n", reg_name(dst_idx));
                fprintf(out, "  or acc\n");
                break;
            case IR_CMP_GE:
                fprintf(out, "  jc %s\n", cmp_label("__cmp_false", uid));
                fprintf(out, "  load %s, 1\n", reg_name(dst_idx));
                fprintf(out, "  jmp %s\n", cmp_label("__cmp_done", uid));
                fprintf(out, "%s:\n", cmp_label("__cmp_false", uid));
                fprintf(out, "  load %s, 0\n", reg_name(dst_idx));
                fprintf(out, "  jmp %s\n", cmp_label("__cmp_done", uid));
                fprintf(out, "%s:\n", cmp_label("__cmp_done", uid));
                fprintf(out, "  mov acc, %s\n", reg_name(dst_idx));
                fprintf(out, "  or acc\n");
                break;
            default:
                break;
            }
            /* For EQ/NE/LT/LE: jump to true, fallthrough to false */
            if (inst->compare_op != IR_CMP_GT && inst->compare_op != IR_CMP_GE) {
                fprintf(out, "  load %s, 0\n", reg_name(dst_idx));
                fprintf(out, "  jmp %s\n", cmp_label("__cmp_done", uid));
                fprintf(out, "%s:\n", cmp_label("__cmp_true", uid));
                fprintf(out, "  load %s, 1\n", reg_name(dst_idx));
                fprintf(out, "%s:\n", cmp_label("__cmp_done", uid));
                fprintf(out, "  mov acc, %s\n", reg_name(dst_idx));
                fprintf(out, "  or acc\n");
            }
        }
        break;
    }

    /* ── Jumps ── */
    case IR_OP_JMP:
        fprintf(out, "  jmp ");
        emit_val(inst->operands[0], out);
        fprintf(out, "\n");
        break;

    /* IR conditional jump format: operands[0]=cond, operands[1]=cmp_val, operands[2]=true_label, operands[3]=false_label
     * CMP produces 0/1 in the condition register. We check cond != 0 (for JNE) or cond == 0 (for JE). */
    case IR_OP_JE:
        /* JE: jump if equal → load cond, if zero jump to true label, else jump to false */
        load_to_acc(inst->operands[0], out);
        fprintf(out, "  jz ");
        emit_val(inst->operands[2], out);
        fprintf(out, "\n");
        if (inst->operand_count > 3 && inst->operands[3].kind == IR_VALUE_LABEL) {
            fprintf(out, "  jmp ");
            emit_val(inst->operands[3], out);
            fprintf(out, "\n");
        }
        break;

    case IR_OP_JNE:
        /* JNE: jump if not equal → load cond, if not zero jump to true label, else jump to false */
        load_to_acc(inst->operands[0], out);
        fprintf(out, "  jnz ");
        emit_val(inst->operands[2], out);
        fprintf(out, "\n");
        if (inst->operand_count > 3 && inst->operands[3].kind == IR_VALUE_LABEL) {
            fprintf(out, "  jmp ");
            emit_val(inst->operands[3], out);
            fprintf(out, "\n");
        }
        break;

    case IR_OP_JG: {
        /* JG: jump if greater than → load cond, check if positive and non-zero */
        load_to_acc(inst->operands[0], out);
        fprintf(out, "  jz ");
        emit_val(inst->operands[3], out);
        fprintf(out, "\n");
        fprintf(out, "  jmp ");
        emit_val(inst->operands[2], out);
        fprintf(out, "\n");
        break;
    }

    case IR_OP_JL: {
        /* JL: jump if less than → load cond, check if negative */
        load_to_acc(inst->operands[0], out);
        fprintf(out, "  jnz ");
        emit_val(inst->operands[2], out);
        fprintf(out, "\n");
        break;
    }

    case IR_OP_JGE: {
        /* JGE: jump if greater or equal → load cond, check if non-negative */
        load_to_acc(inst->operands[0], out);
        fprintf(out, "  jz ");
        emit_val(inst->operands[3], out);
        fprintf(out, "\n");
        fprintf(out, "  jmp ");
        emit_val(inst->operands[2], out);
        fprintf(out, "\n");
        break;
    }

    case IR_OP_JLE: {
        /* JLE: jump if less or equal → load cond, check if zero or negative */
        load_to_acc(inst->operands[0], out);
        fprintf(out, "  jz ");
        emit_val(inst->operands[2], out);
        fprintf(out, "\n");
        fprintf(out, "  jnz ");
        emit_val(inst->operands[2], out);
        fprintf(out, "\n");
        break;
    }

    /* ── CALL ── */
    case IR_OP_CALL: {
        const char* func = inst->operands[0].as.label_val;
        if (!func) break;

        if (strcmp(func, "اطبع") == 0 || strcmp(func, "print") == 0) {
            if (inst->operand_count >= 2) {
                IRValue arg = inst->operands[1];
                /* String literal: generate print_ch loop using immediate addresses.
                 * DHAD ld/st only support 8-bit immediate addresses (not register-indirect).
                 * So we load the address into a register and read bytes using
                 * computed immediate addresses within the loop. */
                if (arg.kind == IR_VALUE_CONSTANT_STRING && arg.as.string_val) {
                    int uid = inst->result.id;
                    /* The string label address will be resolved by the assembler.
                     * We can't use register-indirect ld, so we emit a tight loop
                     * that loads each byte at a known offset from the string base.
                     * Since DHAD doesn't support indirect addressing, we emit
                     * a sequence that loads the pointer, then uses the fact that
                     * ld reg, label resolves to a known address at assembly time.
                     *
                     * Strategy: load base address, then use a workaround:
                     * Since we can't do register-indirect, we must use the string
                     * length known at compile time and emit individual loads. */
                    const char* str_data = arg.ir_str ? arg.ir_str : "";
                    int len = (int)strlen(str_data);
                    if (len == 0) {
                        /* Empty string: do nothing */
                    } else if (len <= 32) {
                        /* Short string: emit individual print_ch for each byte */
                        for (int i = 0; i < len; i++) {
                            fprintf(out, "  load acc, %d\n", (unsigned char)str_data[i]);
                            fprintf(out, "  print_ch\n");
                        }
                    } else {
                        /* Long string: use ld with computed immediate address.
                         * This requires knowing the string's absolute address,
                         * which we compute relative to the string label.
                         * Emit: load base addr into s5, loop using ld with
                         * a modified address each iteration.
                         *
                         * Since DHAD ld only takes immediate addr, we use a
                         * different approach: store the loop count in a register,
                         * and emit ld with a computed immediate for each position.
                         * This is a hybrid approach. */
                        fprintf(out, "  # String print loop (len=%d)\n", len);
                        fprintf(out, "  load s6, 0\n");  /* loop counter (S6=scratch) */
                        fprintf(out, "%s:\n", cmp_label("__print_str", uid));
                        fprintf(out, "  cmp s6\n");      /* ACC = 0 - counter */
                        fprintf(out, "  load acc, %s\n", arg.as.string_val);
                        fprintf(out, "  # Cannot do indirect ld on DHAD\n");
                        fprintf(out, "  jz %s\n", cmp_label("__print_str_done", uid));
                        fprintf(out, "  print_ch\n");
                        fprintf(out, "  inc s6\n");
                        fprintf(out, "  jmp %s\n", cmp_label("__print_str", uid));
                        fprintf(out, "%s:\n", cmp_label("__print_str_done", uid));
                    }
                } else {
                    /* Integer/variable: print as number */
                    load_to_acc(arg, out);
                    fprintf(out, "  print\n");
                }
            }
        } else if (strcmp(func, "اطبع_حرف") == 0 || strcmp(func, "print_ch") == 0) {
            if (inst->operand_count >= 2) {
                load_to_acc(inst->operands[1], out);
                fprintf(out, "  print_ch\n");
            }
        } else if (strcmp(func, "ادخل") == 0 || strcmp(func, "input") == 0) {
            fprintf(out, "  input\n");
            if (dst_idx >= 0 && dst_idx != ACC_INDEX)
                fprintf(out, "  mov %s, acc\n", reg_name(dst_idx));
        } else if (strcmp(func, "توقف") == 0 || strcmp(func, "halt") == 0) {
            fprintf(out, "  halt\n");
        } else {
            /* Phase 12.5-F.6: CALL convention with caller-save frame slots.
             * - Save ALL S0-S5 to dedicated caller-save frame slots before call.
             * - Restore ALL S0-S5 from caller-save slots after call.
             * - Caller-save slots are at the END of the function's frame:
             *   S0 at frame[frame_size-6], S1 at frame[frame_size-5], ...
             *   S5 at frame[frame_size-1].
             *   Relative offsets: -6, -5, -4, -3, -2, -1 from FP.
             * - This ensures ALL registers survive across calls, including
             *   fast-path registers (IR regs 1-6) that don't have reg_map entries.
             * - Save return value in S6 (scratch, not tracked). */

            /* Step 1: Save ALL S0-S5 to caller-save frame slots (except dst_idx).
             * Each slot is at FP + (slot_index - frame_size) where slot_index
             * = frame_size-6 for S0, frame_size-5 for S1, ..., frame_size-1 for S5.
             * So relative offset = -6 + r for register r. */
            for (int r = 0; r <= 5; r++) {
                if (r == dst_idx) continue;
                int rel = r - 6;  /* -6 for S0, -5 for S1, ..., -1 for S5 */
                unsigned char rel_u = (unsigned char)(rel & 0xFF);
                fprintf(out, "  # save s%d to caller-save slot (rel=%d)\n", r, rel);
                fprintf(out, "  load s6, %d\n", DHAD_FRAME_HIGH_BYTE);
                fprintf(out, "  load acc, %d\n", rel_u);
                fprintf(out, "  add s7\n");
                fprintf(out, "  stri s%d, s6, acc\n", r);
            }

            /* Step 2: Proactively spill reg_map entries to spill slots
             * (for values that need to survive beyond this call) */
            for (int r = 0; r <= 5; r++) {
                if (r == dst_idx) continue;
                for (int i = 0; i < g_reg_map_count; i++) {
                    DHADRegMapping* m = &g_reg_map[i];
                    if (m->dhad_reg == r && m->is_loaded) {
                        spill_to_frame(m, out);
                        m->dhad_reg = -1;
                    }
                }
            }

            /* Step 3: Save caller's FP */
            fprintf(out, "  mov acc, s7\n");
            fprintf(out, "  push\n");

            /* Step 4: Load arguments into s0-s3 */
            for (int i = 1; i < inst->operand_count && i <= 4; i++) {
                load_to_reg(inst->operands[i], i - 1, out);
            }
            fprintf(out, "  call %s\n", func);

            /* Step 5: Save return value to S6 */
            fprintf(out, "  mov s6, acc\n");

            /* Step 6: Restore caller's frame pointer */
            fprintf(out, "  pop\n");
            fprintf(out, "  mov s7, acc\n");

            /* Step 7: Push return value to HW stack for safekeeping.
             * The restore loop below clobbers S6 and ACC, so we need
             * to protect the return value on the HW stack. */
            fprintf(out, "  mov acc, s6\n");
            fprintf(out, "  push\n");

            /* Step 8: Restore ALL S0-S5 from caller-save frame slots (except dst_idx) */
            for (int r = 0; r <= 5; r++) {
                if (r == dst_idx) continue;
                int rel = r - 6;
                unsigned char rel_u = (unsigned char)(rel & 0xFF);
                fprintf(out, "  # restore s%d from caller-save slot (rel=%d)\n", r, rel);
                fprintf(out, "  load s6, %d\n", DHAD_FRAME_HIGH_BYTE);
                fprintf(out, "  load acc, %d\n", rel_u);
                fprintf(out, "  add s7\n");
                fprintf(out, "  ldri s%d, s6, acc\n", r);
            }

            /* Step 9: Invalidate ALL reg_map entries for S0-S5 (callee clobbered them).
             * Multiple IR registers may map to the same DHAD register. */
            for (int r = 0; r <= 5; r++) {
                if (r == dst_idx) continue;
                for (int i = 0; i < g_reg_map_count; i++) {
                    DHADRegMapping* m = &g_reg_map[i];
                    if (m->dhad_reg == r) {
                        m->is_loaded = 0;
                        m->dhad_reg = -1;
                    }
                }
            }

            /* Step 10: Pop return value from HW stack and move to destination */
            fprintf(out, "  pop\n");
            if (dst_idx >= 0 && dst_idx != ACC_INDEX) {
                fprintf(out, "  mov %s, acc\n", reg_name(dst_idx));
            }
        }
        break;
    }

    /* ── RET ── */
    case IR_OP_RET:
        if (inst->operand_count >= 1)
            load_to_acc(inst->operands[0], out);
        /* Phase 12.5-F.1/F.6: Restore frame pointer before return.
         * Save return value to hardware stack, restore S7, pop return value.
         * FP-relative mode: restore from [0xE000 + FP - frame_size].
         * Absolute mode: restore from [0xE000 + g_function_frame_base]. */
        if (g_use_fp_relative) {
            int neg_frame = (256 - g_current_frame_size) & 0xFF;
            fprintf(out, "  push\n");
            fprintf(out, "  load s6, %d\n", DHAD_FRAME_HIGH_BYTE);
            fprintf(out, "  load acc, %d\n", neg_frame);
            fprintf(out, "  add s7\n");
            fprintf(out, "  ldri s7, s6, acc\n");
            fprintf(out, "  pop\n");
        } else if (g_function_frame_base < DHAD_FRAME_MAX_SIZE) {
            fprintf(out, "  push\n");           /* save return value */
            fprintf(out, "  load s6, %d\n", DHAD_FRAME_HIGH_BYTE);
            fprintf(out, "  load acc, %d\n", g_function_frame_base & 0xFF);
            fprintf(out, "  ldri s7, s6, acc\n");
            fprintf(out, "  pop\n");            /* restore return value to ACC */
        }
        fprintf(out, "  ret\n");
        break;

    /* ── PUSH/POP ── */
    case IR_OP_PUSH:
        if (inst->operand_count >= 1) {
            load_to_acc(inst->operands[0], out);
            fprintf(out, "  push\n");
        }
        break;

    case IR_OP_POP:
        fprintf(out, "  pop\n");
        if (dst_idx >= 0 && dst_idx != ACC_INDEX)
            fprintf(out, "  mov %s, acc\n", reg_name(dst_idx));
        break;

    /* ── PHI: no-op ── */
    case IR_OP_PHI:
        break;

    /* ── CAST: implicit 8-bit ── */
    case IR_OP_CAST:
        if (dst_idx >= 0)
            load_to_reg(inst->operands[0], dst_idx, out);
        break;

    /* ── LEA: dst = base + offset (address calculation) ──
     * Phase 12.4: Computes address into dst register.
     * The address can later be dereferenced via LDRI/STRI. */
    case IR_OP_LEA:
        if (dst_idx >= 0) {
            emit_alu("add", inst->operands[0], inst->operands[1], dst_idx, out);
        }
        break;

    /* ── MEMBER_ACCESS: same as LEA — compute struct field address ── */
    case IR_OP_MEMBER_ACCESS:
        if (dst_idx >= 0) {
            emit_alu("add", inst->operands[0], inst->operands[1], dst_idx, out);
        }
        break;

    /* ── LOAD_ELEMENT: compute element address then dereference ──
     * Phase 12.5-F.1: Uses S6 (scratch) for high byte instead of S5.
     * Address = base + (index * elem_size). High byte in s6, low byte in dst. */
    case IR_OP_LOAD_ELEMENT:
        if (dst_idx >= 0) {
            emit_alu("add", inst->operands[0], inst->operands[1], dst_idx, out);
            /* Dereference: LDRI dst, s6(high), dst(low) */
            fprintf(out, "  load s6, 0\n");
            fprintf(out, "  ldri %s, s6, %s\n", reg_name(dst_idx), reg_name(dst_idx));
        }
        break;

    /* ── STORE_ELEMENT: store value at computed address ──
     * Phase 12.5-F.1: Uses S6 (scratch) for high byte instead of S5. */
    case IR_OP_STORE_ELEMENT:
        if (inst->operand_count >= 2) {
            if (inst->operands[1].kind == IR_VALUE_REGISTER) {
                int val_idx = ir_reg_to_dhad(inst->operands[1].id);
                if (val_idx >= 0 && dst_idx >= 0) {
                    /* Computed address is in dst. Use STRI to store value.
                     * Convention: dst holds low byte, high byte is 0 for small arrays. */
                    fprintf(out, "  load s6, 0\n");
                    fprintf(out, "  stri %s, s6, %s\n", reg_name(val_idx), reg_name(dst_idx));
                }
            }
        }
        break;

    default:
        fprintf(out, "  # unsupported IR opcode %d\n", inst->opcode);
        break;
    }

    return 1;
}

/* ═══════════════════════════════════════
 *  Block Reordering for Correct Fall-Through
 *
 *  The IR builder creates blocks in insertion order:
 *    entry, if_then, if_merge   (for if without else)
 *    entry, if_then, if_else, if_merge  (for if/else)
 *    entry, while_cond, while_body, while_exit  (for while)
 *
 *  The conditional branch is:
 *    jne cond, true_label, false_label
 *
 *  If we emit blocks in IR order, the false block may not
 *  be the next block in memory, causing wrong fall-through.
 *
 *  Solution: DFS traversal of CFG, always following the
 *  false branch first (as the fall-through path). This
 *  ensures the false block is always the next block in
 *  the emission order after a conditional branch.
 * ═══════════════════════════════════════ */

#define MAX_BLOCKS 256

static void get_block_successors(IRBasicBlock* bb, const char** true_lbl, const char** false_lbl) {
    *true_lbl = NULL;
    *false_lbl = NULL;
    if (bb->instruction_count == 0) return;
    IRInstruction* term = &bb->instructions[bb->instruction_count - 1];
    switch (term->opcode) {
        case IR_OP_JE:
        case IR_OP_JNE:
        case IR_OP_JG:
        case IR_OP_JL:
        case IR_OP_JGE:
        case IR_OP_JLE:
            if (term->operand_count > 2 && term->operands[2].kind == IR_VALUE_LABEL)
                *true_lbl = term->operands[2].as.label_val;
            if (term->operand_count > 3 && term->operands[3].kind == IR_VALUE_LABEL)
                *false_lbl = term->operands[3].as.label_val;
            break;
        case IR_OP_JMP:
            if (term->operand_count > 0 && term->operands[0].kind == IR_VALUE_LABEL)
                *true_lbl = term->operands[0].as.label_val;
            break;
        default:
            break;
    }
}

static int reorder_dhad_blocks(IRFunction* func, IRBasicBlock** out) {
    if (!func || func->block_count == 0) return 0;

    int visited[MAX_BLOCKS] = {0};
    int count = 0;

    /* Worklist DFS: start from entry (block 0) */
    int worklist[MAX_BLOCKS];
    int wl_size = 0;
    worklist[wl_size++] = 0;

    while (wl_size > 0 && count < func->block_count) {
        int idx = worklist[--wl_size];
        if (idx < 0 || idx >= func->block_count || visited[idx]) continue;

        visited[idx] = 1;
        out[count++] = func->blocks[idx];

        const char* true_lbl = NULL;
        const char* false_lbl = NULL;
        get_block_successors(func->blocks[idx], &true_lbl, &false_lbl);

        /* DFS: push true first so false is processed next (fall-through) */
        if (true_lbl) {
            for (int j = 0; j < func->block_count; j++)
                if (func->blocks[j]->label && strcmp(func->blocks[j]->label, true_lbl) == 0) {
                    worklist[wl_size++] = j;
                    break;
                }
        }
        if (false_lbl) {
            for (int j = 0; j < func->block_count; j++)
                if (func->blocks[j]->label && strcmp(func->blocks[j]->label, false_lbl) == 0) {
                    worklist[wl_size++] = j;
                    break;
                }
        }
    }

    /* Add any remaining unreachable blocks in original order */
    for (int i = 0; i < func->block_count && count < func->block_count; i++)
        if (!visited[i])
            out[count++] = func->blocks[i];

    return count;
}

/* ═══════════════════════════════════════
 *  Emit DHAD function (with block reordering)
 *  Phase 12.5-F.6: Two-pass compilation for FP-relative frame addressing.
 *  Pass 1: emit to memory buffer (absolute addressing) to compute frame_size.
 *  Pass 2: emit to real output with FP-relative addressing.
 * ═══════════════════════════════════════ */

int emit_dhad_function(IRFunction* func, FILE* out) {
    if (!func || !out) return 0;
    int count = 0;
    alloca_map_clear();
    reg_map_clear();
    g_function_frame_base = g_frame_size;  /* Capture current frame base */
    g_spill_count = 0;

    /* Reserve frame slot 0 for saved FP (old S7).
     * Spill slots start at offset 1. */
    g_frame_size += 1;

    /* Phase 12.5-F.3: Set function-scoped label prefix to avoid collisions */
    snprintf(g_func_label_prefix, sizeof(g_func_label_prefix), "%s", func->name);

    /* Reorder blocks (shared between both passes) */
    IRBasicBlock* ordered[MAX_BLOCKS];
    int n = reorder_dhad_blocks(func, ordered);

    /* ══ Pass 1: emit to memory buffer (absolute addressing) to compute frame_size ══ */
    g_use_fp_relative = 0;
    char* buf = NULL;
    size_t buf_size = 0;
    FILE* mem = open_memstream(&buf, &buf_size);

    /* Pass 1 prologue (absolute) */
    fprintf(mem, "\n%s:\n", func->name);
    if (g_function_frame_base < DHAD_FRAME_MAX_SIZE) {
        fprintf(mem, "  load s6, %d\n", DHAD_FRAME_HIGH_BYTE);
        fprintf(mem, "  load acc, %d\n", g_function_frame_base & 0xFF);
        fprintf(mem, "  stri s7, s6, acc\n");
        fprintf(mem, "  load s7, %d\n", g_function_frame_base & 0xFF);
    }

    /* Pass 1 body */
    for (int i = 0; i < n; i++) {
        IRBasicBlock* bb = ordered[i];
        if (bb->label && bb->label[0] && strcmp(bb->label, "entry") != 0) {
            fprintf(mem, "%s:\n", func_label(bb->label));
        }
        for (int j = 0; j < bb->instruction_count; j++) {
            count += emit_inst(&bb->instructions[j], mem);
        }
    }

    /* Pass 1 epilogue (absolute) */
    if (g_function_frame_base < DHAD_FRAME_MAX_SIZE) {
        fprintf(mem, "  push\n");
        fprintf(mem, "  load s6, %d\n", DHAD_FRAME_HIGH_BYTE);
        fprintf(mem, "  load acc, %d\n", g_function_frame_base & 0xFF);
        fprintf(mem, "  ldri s7, s6, acc\n");
        fprintf(mem, "  pop\n");
    }
    fprintf(mem, "  ret\n");

    fclose(mem);
    free(buf);

    /* Record frame_size computed by pass 1, then add caller-save area.
     * 6 slots at the end of the frame for saving S0-S5 across calls.
     * This avoids the need for reg_map entries for fast-path registers. */
    int frame_size = (g_frame_size - g_function_frame_base) + 6;
    g_current_frame_size = frame_size;

    /* ══ Pass 2: emit to real output with FP-relative addressing ══ */
    /* Reset allocator state to same starting point as pass 1.
     * Must clear reg_map so pass 2 recreates the same entries with
     * the same spill offsets (matching pass 1's frame_size computation). */
    g_frame_size = g_function_frame_base + 1;  /* Same starting point */
    g_spill_count = 0;
    g_use_fp_relative = 1;
    alloca_map_clear();
    reg_map_clear();

    /* ── PROLOGUE (FP-relative): ──
     * 1. Save old FP at [0xE000 + old_FP]  (old_FP is current S7)
     * 2. Set FP = old_FP + frame_size      (FP now points to end of frame) */
    fprintf(out, "\n%s:\n", func->name);
    fprintf(out, "  # ── prologue (frame_size=%d) ──\n", frame_size);
    fprintf(out, "  load s6, %d\n", DHAD_FRAME_HIGH_BYTE);
    fprintf(out, "  mov acc, s7\n");           /* ACC = old_FP */
    fprintf(out, "  stri s7, s6, acc\n");      /* [0xE000 + old_FP] = old_FP */
    fprintf(out, "  load acc, %d\n", frame_size & 0xFF);
    fprintf(out, "  add s7\n");                /* ACC = frame_size + old_FP */
    fprintf(out, "  mov s7, acc\n");           /* FP = old_FP + frame_size */

    /* ── BODY (FP-relative spill/reload) ── */
    for (int i = 0; i < n; i++) {
        IRBasicBlock* bb = ordered[i];
        if (bb->label && bb->label[0] && strcmp(bb->label, "entry") != 0) {
            fprintf(out, "%s:\n", func_label(bb->label));
        }
        for (int j = 0; j < bb->instruction_count; j++) {
            count += emit_inst(&bb->instructions[j], out);
        }
    }

    /* ── EPILOGUE (FP-relative): restore FP from [0xE000 + FP - frame_size] ── */
    fprintf(out, "  # ── epilogue ──\n");
    fprintf(out, "  push\n");                    /* save return value (ACC) */
    {
        int neg_frame = (256 - frame_size) & 0xFF;
        fprintf(out, "  load s6, %d\n", DHAD_FRAME_HIGH_BYTE);
        fprintf(out, "  load acc, %d\n", neg_frame);
        fprintf(out, "  add s7\n");             /* ACC = FP - frame_size = old_FP */
        fprintf(out, "  ldri s7, s6, acc\n");   /* FP = [0xE000 + old_FP] = saved FP */
    }
    fprintf(out, "  pop\n");                     /* restore return value to ACC */
    fprintf(out, "  ret\n");

    g_use_fp_relative = 0;

    if (g_spill_count > 0) {
        fprintf(out, "  # frame: %d bytes total, %d spill slots in this func\n",
                frame_size, g_spill_count);
    }

    return count;
}

/* ═══════════════════════════════════════
 *  Emit string data section from IR module
 *  (called from main.c BEFORE functions for 8-bit addr compat)
 * ═══════════════════════════════════════ */

void dhad_emit_string_data(FILE* out) {
    /* Emit from backend string table (populated during emit_dhad_function) */
    if (dhad_str_cnt == 0) return;
    fprintf(out, "\n# String data (%d strings)\n", dhad_str_cnt);
    for (int i = 0; i < dhad_str_cnt; i++) {
        fprintf(out, "%s: .ds \"%s\"\n", dhad_str_lbl[i], dhad_str_dat[i]);
    }
}

/* Emit string data directly from IR module's string table.
 * Called BEFORE functions to ensure string labels are in 8-bit range.
 * For short strings (<=32 bytes), skip .ds since we use unrolled byte loads. */
void dhad_emit_module_strings(IRModule* module, FILE* out) {
    if (!module || module->string_count == 0) return;
    int emitted = 0;
    for (int i = 0; i < module->string_count; i++) {
        int len = module->strings[i].str ? (int)strlen(module->strings[i].str) : 0;
        if (len > 32) {
            /* Long string: emit .ds data section (may need ld with address) */
            if (!emitted) {
                fprintf(out, "\n# String data\n");
                emitted = 1;
            }
            fprintf(out, "%s: .ds \"%s\"\n", module->strings[i].label, module->strings[i].str);
        }
        /* Short strings: no .ds needed — bytes are emitted inline */
    }
}
