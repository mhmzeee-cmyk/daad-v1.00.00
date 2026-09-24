/**
 * @file backend_interface.c
 * @brief تنفيذ Backend الموحد - Unified Backend Implementation
 *
 * يجمع:
 * - backend_interface (instruction emission)
 * - function_generator (prologue/epilogue)
 * - expression_generator (expressions)
 * - statement_generator (statements)
 * - stack_manager (stack layout)
 * - label_manager (labels)
 * - register_allocator (register allocation)
 */

#include "backend_interface.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ── جدول أطوال نصوص الوحدة: يُملأ عند إصدار المجمّع ويُقرأ في CALL اطبع ── */
#define DAAD_MAX_STRINGS 256
static char   g_str_labels[DAAD_MAX_STRINGS][64];
static size_t g_str_lengths[DAAD_MAX_STRINGS];
static int    g_str_count = 0;

static int string_length_of(const char* label) {
    for (int i = 0; i < g_str_count; i++)
        if (strcmp(g_str_labels[i], label) == 0) return (int)g_str_lengths[i];
    return -1;
}

static int emit_x86_function(IRFunction* func, FILE* out);
static int emit_x86_instruction(IRInstruction* inst, FILE* out, BackendContext* ctx);
static const char* x86_reg_name_internal(int reg_id);
static const char* x86_type_suffix_internal(IRType type);

/* DHAD backend (implemented in dhad_backend.c) */
extern int emit_dhad_function(IRFunction* func, FILE* out);
extern const char* dhad_reg_name(int reg_id);
extern const char* dhad_type_suffix(IRType type);
extern void dhad_emit_string_data(FILE* out);

static void emit_label_ref(const IRFunction* func, IRValue label, FILE* out) {
    if (label.kind == IR_VALUE_LABEL && label.as.label_val) {
        fprintf(out, ".L_%s_%s", func->name, label.as.label_val);
    } else {
        fprintf(out, "%s", ir_value_name(label));
    }
}

BackendContext* backend_context_create(void) {
    BackendContext* ctx = (BackendContext*)calloc(1, sizeof(BackendContext));
    if (!ctx) return NULL;
    ctx->reg_alloc = reg_alloc_create();
    if (!ctx->reg_alloc) { free(ctx); return NULL; }
    ctx->stack = stack_manager_create();
    if (!ctx->stack) { reg_alloc_destroy(ctx->reg_alloc); free(ctx); return NULL; }
    ctx->labels = label_manager_create();
    if (!ctx->labels) { stack_manager_destroy(ctx->stack); reg_alloc_destroy(ctx->reg_alloc); free(ctx); return NULL; }
    ctx->frame_size = 0;
    ctx->param_count = 0;
    ctx->local_offset = 0;
    ctx->alloca_counter = 0;
    ctx->current_func = NULL;
    ctx->error_occurred = 0;
    ctx->reg_map_size = 256;
    ctx->reg_to_offset = (int*)calloc(ctx->reg_map_size, sizeof(int));
    ctx->max_reg_id = 0;
    ctx->callee_saved_count = 0;
    if (!ctx->reg_to_offset) {
        label_manager_destroy(ctx->labels);
        stack_manager_destroy(ctx->stack);
        reg_alloc_destroy(ctx->reg_alloc);
        free(ctx);
        return NULL;
    }
    for (int i = 0; i < ctx->reg_map_size; i++) ctx->reg_to_offset[i] = -1;
    return ctx;
}

void backend_context_destroy(BackendContext* ctx) {
    if (!ctx) return;
    reg_alloc_destroy(ctx->reg_alloc);
    stack_manager_destroy(ctx->stack);
    label_manager_destroy(ctx->labels);
    free(ctx->reg_to_offset);
    free(ctx);
}

int backend_build_intervals(BackendContext* ctx, IRFunction* func) {
    if (!ctx || !func) return 0;

    int* last_use = (int*)calloc(256, sizeof(int));
    if (!last_use) return 0;

    int time = 0;
    for (int i = 0; i < func->block_count; i++) {
        IRBasicBlock* bb = func->blocks[i];
        for (int j = 0; j < bb->instruction_count; j++) {
            IRInstruction* inst = &bb->instructions[j];
            time++;

            for (int o = 0; o < inst->operand_count; o++) {
                if (inst->operands[o].kind == IR_VALUE_REGISTER) {
                    int id = inst->operands[o].id;
                    if (id > 0 && id < 256) last_use[id] = time;
                }
            }

            if (inst->result.kind == IR_VALUE_REGISTER) {
                int id = inst->result.id;
                if (id > 0 && id < 256) last_use[id] = time;
            }
        }
    }

    int start_time = 0;
    for (int i = 0; i < func->block_count; i++) {
        IRBasicBlock* bb = func->blocks[i];
        for (int j = 0; j < bb->instruction_count; j++) {
            IRInstruction* inst = &bb->instructions[j];
            start_time++;

            if (inst->result.kind == IR_VALUE_REGISTER) {
                int id = inst->result.id;
                if (id > 0 && id < 256) {
                    reg_alloc_add_interval(ctx->reg_alloc, id, start_time, last_use[id]);
                }
            }
        }
    }

    free(last_use);
    return time;
}

int backend_allocate_registers(BackendContext* ctx) {
    if (!ctx) return 0;
    reg_alloc_linear_scan(ctx->reg_alloc);
    
    ctx->callee_saved_count = 0;
    static const PhysicalRegister callee_saved[] = {REG_RBX, REG_R12, REG_R13, REG_R14, REG_R15};
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < ctx->reg_alloc->interval_count; j++) {
            if (ctx->reg_alloc->intervals[j].phys_reg == callee_saved[i] &&
                !ctx->reg_alloc->intervals[j].is_spilled) {
                ctx->callee_saved_regs[ctx->callee_saved_count] = callee_saved[i];
                ctx->callee_saved_count++;
                break;
            }
        }
    }
    
    return reg_alloc_get_frame_size(ctx->reg_alloc);
}

Backend* backend_create_x86(void) {
    Backend* b = (Backend*)calloc(1, sizeof(Backend));
    if (!b) return NULL;
    b->target = BACKEND_X86;
    b->name = "x86-64";
    b->emit_function = emit_x86_function;
    b->emit_instruction = NULL;
    b->reg_name = x86_reg_name_internal;
    b->type_suffix = x86_type_suffix_internal;
    return b;
}

Backend* backend_create_arm(void) {
    Backend* b = (Backend*)calloc(1, sizeof(Backend));
    if (!b) return NULL;
    b->target = BACKEND_ARM;
    b->name = "ARM64";
    b->emit_function = NULL;
    b->emit_instruction = NULL;
    b->reg_name = NULL;
    b->type_suffix = NULL;
    return b;
}

Backend* backend_create_riscv(void) {
    Backend* b = (Backend*)calloc(1, sizeof(Backend));
    if (!b) return NULL;
    b->target = BACKEND_RISCV;
    b->name = "RISC-V";
    b->emit_function = NULL;
    b->emit_instruction = NULL;
    b->reg_name = NULL;
    b->type_suffix = NULL;
    return b;
}

Backend* backend_create_dhad(void) {
    Backend* b = (Backend*)calloc(1, sizeof(Backend));
    if (!b) return NULL;
    b->target = BACKEND_DHAD;
    b->name = "DHAD-8";
    b->emit_function = emit_dhad_function;
    b->emit_instruction = NULL;
    b->reg_name = dhad_reg_name;
    b->type_suffix = dhad_type_suffix;
    return b;
}

Backend* backend_get(BackendTarget target) {
    switch (target) {
        case BACKEND_X86: return backend_create_x86();
        case BACKEND_ARM: return backend_create_arm();
        case BACKEND_RISCV: return backend_create_riscv();
        case BACKEND_DHAD: return backend_create_dhad();
    }
    return NULL;
}

void backend_destroy(Backend* backend) {
    free(backend);
}

int x86_calculate_frame_size(IRFunction* func) {
    if (!func) return 16;
    int size = 0;
    for (int i = 0; i < func->param_count; i++) {
        size += 8;
    }
    size += func->alloca_count * 8;
    if (size % 16 != 0) {
        size = ((size + 15) / 16) * 16;
    }
    if (size < 16) size = 16;
    return size;
}

int x86_get_param_offset(int param_index) {
    return 16 + param_index * 8;
}

const char* x86_reg_name(int reg_id) {
    return x86_reg_name_internal(reg_id);
}

static const char* x86_reg_name_internal(int reg_id) {
    switch (reg_id) {
        case 1:  return "%rax";
        case 2:  return "%rbx";
        case 3:  return "%rcx";
        case 4:  return "%rdx";
        case 5:  return "%r8";
        case 6:  return "%r9";
        case 7:  return "%r10";
        case 8:  return "%r11";
        case 9:  return "%r12";
        case 10: return "%r13";
        case 11: return "%r14";
        case 12: return "%r15";
        default: return "%rax";
    }
}

static int x86_reg_is_float(int reg_id) {
    return (reg_id >= REG_XMM0 && reg_id <= REG_XMM7);
}

__attribute__((unused)) static const char* resolve_reg_name(RegisterAllocator* alloc, int ir_reg) {
    if (!alloc) return x86_reg_name_internal(ir_reg);
    PhysicalRegister phys = reg_alloc_get_physical(alloc, ir_reg);
    if (phys != REG_NONE) return reg_alloc_phys_name(phys);
    return x86_reg_name_internal(ir_reg);
}

static int reg_get_offset(BackendContext* ctx, int reg_id) {
    if (!ctx || reg_id <= 0) return -(reg_id * 8);
    
    if (ctx->reg_alloc) {
        PhysicalRegister phys = reg_alloc_get_physical(ctx->reg_alloc, reg_id);
        if (phys != REG_NONE) {
            return -(reg_id * 8);
        }
        
        if (reg_alloc_is_spilled(ctx->reg_alloc, reg_id)) {
            int spill_off = reg_alloc_get_spill_offset(ctx->reg_alloc, reg_id);
            if (spill_off >= 0) {
                return -(ctx->max_reg_id + 1 + spill_off / 8) * 8;
            }
        }
    }
    
    return -(reg_id * 8);
}

static const char* reg_get_name(BackendContext* ctx, int reg_id) {
    if (!ctx || reg_id <= 0) return x86_reg_name_internal(reg_id);
    
    if (ctx->reg_alloc) {
        PhysicalRegister phys = reg_alloc_get_physical(ctx->reg_alloc, reg_id);
        if (phys != REG_NONE) {
            return reg_alloc_phys_name(phys);
        }
    }
    
    return x86_reg_name_internal(reg_id);
}

__attribute__((unused)) static int resolve_is_float(RegisterAllocator* alloc, int ir_reg) {
    if (!alloc) return x86_reg_is_float(ir_reg);
    PhysicalRegister phys = reg_alloc_get_physical(alloc, ir_reg);
    return phys >= REG_XMM0 && phys <= REG_XMM7;
}

static int result_is_float(IRInstruction* inst) {
    if (inst->result.kind != IR_VALUE_REGISTER) return 0;
    if (inst->result.type.kind == IR_TYPE_F64 || inst->result.type.kind == IR_TYPE_F32) return 1;
    return 0;
}

const char* x86_type_suffix(IRType type) {
    return x86_type_suffix_internal(type);
}

static const char* x86_type_suffix_internal(IRType type) {
    switch (type.kind) {
        case IR_TYPE_I8: return "b";
        case IR_TYPE_I16: return "w";
        case IR_TYPE_I32: return "l";
        case IR_TYPE_I64: case IR_TYPE_PTR: return "q";
        case IR_TYPE_F32: return "ss";
        case IR_TYPE_F64: return "sd";
        default: return "q";
    }
}

static int emit_x86_prologue(IRFunction* func, FILE* out) {
    if (!func || !out) return 0;
    int frame_size = x86_calculate_frame_size(func);
    fprintf(out, ".globl %s\n", func->name);
    fprintf(out, "%s:\n", func->name);
    fprintf(out, "  pushq %%rbp\n");
    fprintf(out, "  movq %%rsp, %%rbp\n");
    if (frame_size > 0) {
        fprintf(out, "  subq $%d, %%rsp\n", frame_size);
    }
    static const char* param_regs[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    for (int i = 0; i < func->param_count && i < 6; i++) {
        fprintf(out, "  movq %s, %s\n", param_regs[i], x86_reg_name_internal(func->params[i].id));
    }
    return 1;
}

static int emit_x86_epilogue(FILE* out) {
    if (!out) return 0;
    fprintf(out, "  leave\n");
    fprintf(out, "  ret\n");
    return 1;
}

static int emit_x86_instruction(IRInstruction* inst, FILE* out, BackendContext* ctx) {
    if (!inst || !out) return 0;

    #define SLOT(r) reg_get_offset(ctx, r)
    #define REG_NAME(r) reg_get_name(ctx, r)
    #define LOAD_TO(r, reg) do { \
        if (ctx && ctx->reg_alloc) { \
            if (reg_alloc_get_physical(ctx->reg_alloc, r) != REG_NONE) { \
                fprintf(out, "  movq %s, %s\n", REG_NAME(r), reg); \
            } else if (reg_alloc_is_spilled(ctx->reg_alloc, r)) { \
                int spill_off = reg_alloc_get_spill_offset(ctx->reg_alloc, r); \
                if (spill_off >= 0) { \
                    fprintf(out, "  movq %d(%%rbp), %s\n", -(ctx->max_reg_id + 1 + spill_off / 8) * 8, reg); \
                } else { \
                    fprintf(out, "  movq %d(%%rbp), %s\n", SLOT(r), reg); \
                } \
            } else { \
                fprintf(out, "  movq %d(%%rbp), %s\n", SLOT(r), reg); \
            } \
        } else { \
            fprintf(out, "  movq %d(%%rbp), %s\n", SLOT(r), reg); \
        } \
    } while(0)
    #define STORE(r) do { \
        if (ctx && ctx->reg_alloc) { \
            if (reg_alloc_get_physical(ctx->reg_alloc, r) != REG_NONE) { \
                fprintf(out, "  movq %%rax, %s\n", REG_NAME(r)); \
            } else if (reg_alloc_is_spilled(ctx->reg_alloc, r)) { \
                int spill_off = reg_alloc_get_spill_offset(ctx->reg_alloc, r); \
                if (spill_off >= 0) { \
                    fprintf(out, "  movq %%rax, %d(%%rbp)\n", -(ctx->max_reg_id + 1 + spill_off / 8) * 8); \
                } else { \
                    fprintf(out, "  movq %%rax, %d(%%rbp)\n", SLOT(r)); \
                } \
            } else { \
                fprintf(out, "  movq %%rax, %d(%%rbp)\n", SLOT(r)); \
            } \
        } else { \
            fprintf(out, "  movq %%rax, %d(%%rbp)\n", SLOT(r)); \
        } \
    } while(0)
    #define LOAD_FLOAT_TO(r, xmm) do { \
        if (ctx && ctx->reg_alloc) { \
            if (reg_alloc_get_physical(ctx->reg_alloc, r) != REG_NONE) { \
                fprintf(out, "  movsd %s, %s\n", REG_NAME(r), xmm); \
            } else if (reg_alloc_is_spilled(ctx->reg_alloc, r)) { \
                int spill_off = reg_alloc_get_spill_offset(ctx->reg_alloc, r); \
                if (spill_off >= 0) { \
                    fprintf(out, "  movsd %d(%%rbp), %s\n", -(ctx->max_reg_id + 1 + spill_off / 8) * 8, xmm); \
                } else { \
                    fprintf(out, "  movsd %d(%%rbp), %s\n", SLOT(r), xmm); \
                } \
            } else { \
                fprintf(out, "  movsd %d(%%rbp), %s\n", SLOT(r), xmm); \
            } \
        } else { \
            fprintf(out, "  movsd %d(%%rbp), %s\n", SLOT(r), xmm); \
        } \
    } while(0)
    #define STORE_FLOAT(r) do { \
        if (ctx && ctx->reg_alloc) { \
            if (reg_alloc_get_physical(ctx->reg_alloc, r) != REG_NONE) { \
                fprintf(out, "  movsd %%xmm0, %s\n", REG_NAME(r)); \
            } else if (reg_alloc_is_spilled(ctx->reg_alloc, r)) { \
                int spill_off = reg_alloc_get_spill_offset(ctx->reg_alloc, r); \
                if (spill_off >= 0) { \
                    fprintf(out, "  movsd %%xmm0, %d(%%rbp)\n", -(ctx->max_reg_id + 1 + spill_off / 8) * 8); \
                } else { \
                    fprintf(out, "  movsd %%xmm0, %d(%%rbp)\n", SLOT(r)); \
                } \
            } else { \
                fprintf(out, "  movsd %%xmm0, %d(%%rbp)\n", SLOT(r)); \
            } \
        } else { \
            fprintf(out, "  movsd %%xmm0, %d(%%rbp)\n", SLOT(r)); \
        } \
    } while(0)

    switch (inst->opcode) {
        case IR_OP_NOP:
            fprintf(out, "  nop\n");
            break;
        case IR_OP_MOV:
            if (inst->result.kind == IR_VALUE_REGISTER && inst->operand_count > 0) {
                if (result_is_float(inst)) {
                    if (inst->operands[0].kind == IR_VALUE_CONSTANT_FLOAT) {
                        fprintf(out, "  movsd .LC_%d(%%rip), %%xmm0\n", inst->result.id);
                        STORE_FLOAT(inst->result.id);
                    } else if (inst->operands[0].kind == IR_VALUE_REGISTER) {
                        LOAD_FLOAT_TO(inst->operands[0].id, "%xmm0");
                        STORE_FLOAT(inst->result.id);
                    }
                } else {
                    if (inst->operands[0].kind == IR_VALUE_CONSTANT_INT) {
                        fprintf(out, "  movq $%lld, %d(%%rbp)\n", inst->operands[0].as.int_val, SLOT(inst->result.id));
                    } else if (inst->operands[0].kind == IR_VALUE_REGISTER) {
                        LOAD_TO(inst->operands[0].id, "%rax");
                        STORE(inst->result.id);
                    }
                }
            }
            break;
        case IR_OP_ADD:
            if (inst->result.kind == IR_VALUE_REGISTER && inst->operand_count >= 2) {
                if (result_is_float(inst)) {
                    LOAD_FLOAT_TO(inst->operands[0].id, "%xmm0");
                    if (inst->operands[1].kind == IR_VALUE_CONSTANT_FLOAT) {
                        fprintf(out, "  movsd .LC_%d(%%rip), %%xmm1\n", inst->result.id);
                        fprintf(out, "  addsd %%xmm1, %%xmm0\n");
                    } else if (inst->operands[1].kind == IR_VALUE_REGISTER) {
                        LOAD_FLOAT_TO(inst->operands[1].id, "%xmm1");
                        fprintf(out, "  addsd %%xmm1, %%xmm0\n");
                    }
                    STORE_FLOAT(inst->result.id);
                } else {
                    LOAD_TO(inst->operands[0].id, "%rax");
                    if (inst->operands[1].kind == IR_VALUE_CONSTANT_INT)
                        fprintf(out, "  addq $%lld, %%rax\n", inst->operands[1].as.int_val);
                    else if (inst->operands[1].kind == IR_VALUE_REGISTER) {
                        /* الإصلاح: حمّل المعامل الثاني بمنطق التخصيص الصحيح بدل قراءة خانة عمياء */
                        LOAD_TO(inst->operands[1].id, "%rcx");
                        fprintf(out, "  addq %%rcx, %%rax\n");
                    }
                    STORE(inst->result.id);
                }
            }
            break;
        case IR_OP_SUB:
            if (inst->result.kind == IR_VALUE_REGISTER && inst->operand_count >= 2) {
                if (result_is_float(inst)) {
                    LOAD_FLOAT_TO(inst->operands[0].id, "%xmm0");
                    if (inst->operands[1].kind == IR_VALUE_CONSTANT_FLOAT) {
                        fprintf(out, "  movsd .LC_%d(%%rip), %%xmm1\n", inst->result.id);
                        fprintf(out, "  subsd %%xmm1, %%xmm0\n");
                    } else if (inst->operands[1].kind == IR_VALUE_REGISTER) {
                        LOAD_FLOAT_TO(inst->operands[1].id, "%xmm1");
                        fprintf(out, "  subsd %%xmm1, %%xmm0\n");
                    }
                    STORE_FLOAT(inst->result.id);
                } else {
                    LOAD_TO(inst->operands[0].id, "%rax");
                    if (inst->operands[1].kind == IR_VALUE_CONSTANT_INT)
                        fprintf(out, "  subq $%lld, %%rax\n", inst->operands[1].as.int_val);
                    else if (inst->operands[1].kind == IR_VALUE_REGISTER) {
                        LOAD_TO(inst->operands[1].id, "%rcx");
                        fprintf(out, "  subq %%rcx, %%rax\n");
                    }
                    STORE(inst->result.id);
                }
            }
            break;
        case IR_OP_MUL:
            if (inst->result.kind == IR_VALUE_REGISTER && inst->operand_count >= 2) {
                if (result_is_float(inst)) {
                    LOAD_FLOAT_TO(inst->operands[0].id, "%xmm0");
                    if (inst->operands[1].kind == IR_VALUE_CONSTANT_FLOAT) {
                        fprintf(out, "  movsd .LC_%d(%%rip), %%xmm1\n", inst->result.id);
                        fprintf(out, "  mulsd %%xmm1, %%xmm0\n");
                    } else if (inst->operands[1].kind == IR_VALUE_REGISTER) {
                        LOAD_FLOAT_TO(inst->operands[1].id, "%xmm1");
                        fprintf(out, "  mulsd %%xmm1, %%xmm0\n");
                    }
                    STORE_FLOAT(inst->result.id);
                } else {
                    LOAD_TO(inst->operands[0].id, "%rax");
                    if (inst->operands[1].kind == IR_VALUE_CONSTANT_INT)
                        fprintf(out, "  imulq $%lld, %%rax\n", inst->operands[1].as.int_val);
                    else if (inst->operands[1].kind == IR_VALUE_REGISTER) {
                        LOAD_TO(inst->operands[1].id, "%rcx");
                        fprintf(out, "  imulq %%rcx, %%rax\n");
                    }
                    STORE(inst->result.id);
                }
            }
            break;
        case IR_OP_DIV:
            if (inst->result.kind == IR_VALUE_REGISTER && inst->operand_count >= 2) {
                if (result_is_float(inst)) {
                    LOAD_FLOAT_TO(inst->operands[0].id, "%xmm0");
                    if (inst->operands[1].kind == IR_VALUE_CONSTANT_FLOAT) {
                        fprintf(out, "  movsd .LC_%d(%%rip), %%xmm1\n", inst->result.id);
                        fprintf(out, "  divsd %%xmm1, %%xmm0\n");
                    } else if (inst->operands[1].kind == IR_VALUE_REGISTER) {
                        LOAD_FLOAT_TO(inst->operands[1].id, "%xmm1");
                        fprintf(out, "  divsd %%xmm1, %%xmm0\n");
                    }
                    STORE_FLOAT(inst->result.id);
                } else {
                    LOAD_TO(inst->operands[0].id, "%rax");
                    fprintf(out, "  cqo\n");
                    if (inst->operands[1].kind == IR_VALUE_CONSTANT_INT) {
                        fprintf(out, "  movq $%lld, %%rcx\n", inst->operands[1].as.int_val);
                        fprintf(out, "  idivq %%rcx\n");
                    } else if (inst->operands[1].kind == IR_VALUE_REGISTER) {
                        LOAD_TO(inst->operands[1].id, "%rcx");
                        fprintf(out, "  idivq %%rcx\n");
                    }
                    STORE(inst->result.id);
                }
            }
            break;
        case IR_OP_MOD:
            if (inst->result.kind == IR_VALUE_REGISTER && inst->operand_count >= 2) {
                LOAD_TO(inst->operands[0].id, "%rax");
                fprintf(out, "  cqo\n");
                if (inst->operands[1].kind == IR_VALUE_CONSTANT_INT) {
                    fprintf(out, "  movq $%lld, %%rcx\n", inst->operands[1].as.int_val);
                    fprintf(out, "  idivq %%rcx\n");
                } else if (inst->operands[1].kind == IR_VALUE_REGISTER) {
                    fprintf(out, "  idivq %d(%%rbp)\n", SLOT(inst->operands[1].id));
                }
                fprintf(out, "  movq %%rdx, %%rax\n");
                STORE(inst->result.id);
            }
            break;
        case IR_OP_AND:
            if (inst->result.kind == IR_VALUE_REGISTER && inst->operand_count >= 2) {
                LOAD_TO(inst->operands[0].id, "%rax");
                if (inst->operands[1].kind == IR_VALUE_CONSTANT_INT)
                    fprintf(out, "  andq $%lld, %%rax\n", inst->operands[1].as.int_val);
                else if (inst->operands[1].kind == IR_VALUE_REGISTER)
                    fprintf(out, "  andq %d(%%rbp), %%rax\n", SLOT(inst->operands[1].id));
                STORE(inst->result.id);
            }
            break;
        case IR_OP_OR:
            if (inst->result.kind == IR_VALUE_REGISTER && inst->operand_count >= 2) {
                LOAD_TO(inst->operands[0].id, "%rax");
                if (inst->operands[1].kind == IR_VALUE_CONSTANT_INT)
                    fprintf(out, "  orq $%lld, %%rax\n", inst->operands[1].as.int_val);
                else if (inst->operands[1].kind == IR_VALUE_REGISTER)
                    fprintf(out, "  orq %d(%%rbp), %%rax\n", SLOT(inst->operands[1].id));
                STORE(inst->result.id);
            }
            break;
        case IR_OP_XOR:
            if (inst->result.kind == IR_VALUE_REGISTER && inst->operand_count >= 2) {
                LOAD_TO(inst->operands[0].id, "%rax");
                if (inst->operands[1].kind == IR_VALUE_CONSTANT_INT)
                    fprintf(out, "  xorq $%lld, %%rax\n", inst->operands[1].as.int_val);
                else if (inst->operands[1].kind == IR_VALUE_REGISTER)
                    fprintf(out, "  xorq %d(%%rbp), %%rax\n", SLOT(inst->operands[1].id));
                STORE(inst->result.id);
            }
            break;
        case IR_OP_SHL:
            if (inst->result.kind == IR_VALUE_REGISTER && inst->operand_count >= 2) {
                LOAD_TO(inst->operands[0].id, "%rax");
                if (inst->operands[1].kind == IR_VALUE_CONSTANT_INT)
                    fprintf(out, "  movq $%lld, %%rcx\n", inst->operands[1].as.int_val);
                else if (inst->operands[1].kind == IR_VALUE_REGISTER)
                    LOAD_TO(inst->operands[1].id, "%rcx");
                fprintf(out, "  shlq %%cl, %%rax\n");
                STORE(inst->result.id);
            }
            break;
        case IR_OP_SHR:
            if (inst->result.kind == IR_VALUE_REGISTER && inst->operand_count >= 2) {
                LOAD_TO(inst->operands[0].id, "%rax");
                if (inst->operands[1].kind == IR_VALUE_CONSTANT_INT)
                    fprintf(out, "  movq $%lld, %%rcx\n", inst->operands[1].as.int_val);
                else if (inst->operands[1].kind == IR_VALUE_REGISTER)
                    LOAD_TO(inst->operands[1].id, "%rcx");
                fprintf(out, "  shrq %%cl, %%rax\n");
                STORE(inst->result.id);
            }
            break;
        case IR_OP_NEG:
            if (inst->result.kind == IR_VALUE_REGISTER && inst->operand_count >= 1) {
                if (inst->operands[0].kind == IR_VALUE_CONSTANT_INT) {
                    fprintf(out, "  movq $%lld, %d(%%rbp)\n", -inst->operands[0].as.int_val, SLOT(inst->result.id));
                } else if (inst->operands[0].kind == IR_VALUE_REGISTER) {
                    LOAD_TO(inst->operands[0].id, "%rax");
                    fprintf(out, "  negq %%rax\n");
                    STORE(inst->result.id);
                }
            }
            break;
        case IR_OP_NOT:
            if (inst->result.kind == IR_VALUE_REGISTER && inst->operand_count >= 1) {
                if (inst->operands[0].kind == IR_VALUE_CONSTANT_INT) {
                    fprintf(out, "  movq $%lld, %d(%%rbp)\n", ~inst->operands[0].as.int_val, SLOT(inst->result.id));
                } else if (inst->operands[0].kind == IR_VALUE_REGISTER) {
                    LOAD_TO(inst->operands[0].id, "%rax");
                    fprintf(out, "  notq %%rax\n");
                    STORE(inst->result.id);
                }
            }
            break;
        case IR_OP_CMP:
            if (inst->operand_count >= 2) {
                if (result_is_float(inst)) {
                    LOAD_FLOAT_TO(inst->operands[0].id, "%xmm0");
                    if (inst->operands[1].kind == IR_VALUE_CONSTANT_FLOAT) {
                        fprintf(out, "  movsd .LC_%d(%%rip), %%xmm1\n", inst->result.id);
                        fprintf(out, "  ucomisd %%xmm1, %%xmm0\n");
                    } else if (inst->operands[1].kind == IR_VALUE_REGISTER) {
                        LOAD_FLOAT_TO(inst->operands[1].id, "%xmm1");
                        fprintf(out, "  ucomisd %%xmm1, %%xmm0\n");
                    }
                } else {
                    if (inst->operands[0].kind == IR_VALUE_REGISTER && inst->operands[1].kind == IR_VALUE_CONSTANT_INT) {
                        LOAD_TO(inst->operands[0].id, "%rax");
                        fprintf(out, "  cmpq $%lld, %%rax\n", inst->operands[1].as.int_val);
                    } else if (inst->operands[0].kind == IR_VALUE_REGISTER && inst->operands[1].kind == IR_VALUE_REGISTER) {
                        LOAD_TO(inst->operands[0].id, "%rax");
                        LOAD_TO(inst->operands[1].id, "%rcx");
                        fprintf(out, "  cmpq %%rcx, %%rax\n");
                    }
                }
                switch (inst->compare_op) {
                    case IR_CMP_EQ: fprintf(out, "  sete %%al\n"); break;
                    case IR_CMP_NE: fprintf(out, "  setne %%al\n"); break;
                    case IR_CMP_GT: fprintf(out, "  setg %%al\n"); break;
                    case IR_CMP_LT: fprintf(out, "  setl %%al\n"); break;
                    case IR_CMP_GE: fprintf(out, "  setge %%al\n"); break;
                    case IR_CMP_LE: fprintf(out, "  setle %%al\n"); break;
                    default: fprintf(out, "  sete %%al\n"); break;
                }
                fprintf(out, "  movzbl %%al, %%eax\n");
                STORE(inst->result.id);
            }
            break;
        case IR_OP_JMP:
            if (inst->operand_count > 0) {
                fprintf(out, "  jmp ");
                emit_label_ref(ctx->current_func, inst->operands[0], out);
                fprintf(out, "\n");
            }
            break;
        case IR_OP_JE:
        case IR_OP_JNE:
        case IR_OP_JG:
        case IR_OP_JL:
        case IR_OP_JGE:
        case IR_OP_JLE:
            if (inst->operand_count >= 4) {
                if (inst->operands[0].kind == IR_VALUE_REGISTER) {
                    LOAD_TO(inst->operands[0].id, "%rax");
                } else if (inst->operands[0].kind == IR_VALUE_CONSTANT_INT) {
                    fprintf(out, "  movq $%lld, %%rax\n", inst->operands[0].as.int_val);
                }
                if (inst->operands[1].kind == IR_VALUE_REGISTER) {
                    LOAD_TO(inst->operands[1].id, "%rcx");
                } else if (inst->operands[1].kind == IR_VALUE_CONSTANT_INT) {
                    fprintf(out, "  movq $%lld, %%rcx\n", inst->operands[1].as.int_val);
                }
                fprintf(out, "  cmpq %%rcx, %%rax\n");
                switch (inst->compare_op) {
                    case IR_CMP_EQ:  fprintf(out, "  je "); break;
                    case IR_CMP_NE:  fprintf(out, "  jne "); break;
                    case IR_CMP_GT:  fprintf(out, "  jg "); break;
                    case IR_CMP_LT:  fprintf(out, "  jl "); break;
                    case IR_CMP_GE:  fprintf(out, "  jge "); break;
                    case IR_CMP_LE:  fprintf(out, "  jle "); break;
                    default: fprintf(out, "  je "); break;
                }
                emit_label_ref(ctx->current_func, inst->operands[2], out);
                fprintf(out, "\n");
                if (inst->operands[3].kind == IR_VALUE_CONSTANT_STRING || inst->operands[3].kind == IR_VALUE_LABEL) {
                    fprintf(out, "  jmp ");
                    emit_label_ref(ctx->current_func, inst->operands[3], out);
                    fprintf(out, "\n");
                }
            }
            break;
        case IR_OP_ALLOCA:
            if (inst->result.kind == IR_VALUE_REGISTER) {
                if (ctx) ctx->alloca_counter++;
                int alloca_off = -(ctx->max_reg_id + 1 + (ctx ? ctx->alloca_counter : 1)) * 8;
                fprintf(out, "  leaq %d(%%rbp), %%rax\n", alloca_off);
                STORE(inst->result.id);
            }
            break;
        case IR_OP_STORE:
            if (inst->operand_count >= 2) {
                if (inst->operands[0].kind == IR_VALUE_REGISTER && inst->operands[1].kind == IR_VALUE_CONSTANT_INT) {
                    LOAD_TO(inst->operands[0].id, "%rax");
                    fprintf(out, "  movq $%lld, (%%rax)\n", inst->operands[1].as.int_val);
                } else if (inst->operands[0].kind == IR_VALUE_REGISTER && inst->operands[1].kind == IR_VALUE_REGISTER) {
                    LOAD_TO(inst->operands[0].id, "%rax");
                    LOAD_TO(inst->operands[1].id, "%rcx");
                    fprintf(out, "  movq %%rcx, (%%rax)\n");
                }
            }
            break;
        case IR_OP_LOAD:
            if (inst->result.kind == IR_VALUE_REGISTER && inst->operand_count > 0) {
                if (inst->operands[0].kind == IR_VALUE_REGISTER) {
                    LOAD_TO(inst->operands[0].id, "%rax");
                    fprintf(out, "  movq (%%rax), %%rax\n");
                    STORE(inst->result.id);
                }
            }
            break;
        case IR_OP_LOAD_ELEMENT:
            if (inst->result.kind == IR_VALUE_REGISTER && inst->operand_count >= 2) {
                LOAD_TO(inst->operands[0].id, "%rax");
                if (inst->operands[1].kind == IR_VALUE_CONSTANT_INT)
                    fprintf(out, "  movq %lld(%%rax), %%rax\n", inst->operands[1].as.int_val * 8);
                else if (inst->operands[1].kind == IR_VALUE_REGISTER) {
                    LOAD_TO(inst->operands[1].id, "%rcx");
                    fprintf(out, "  movq (%%rax,%%rcx,8), %%rax\n");
                }
                STORE(inst->result.id);
            }
            break;
        case IR_OP_STORE_ELEMENT:
            if (inst->operand_count >= 2) {
                LOAD_TO(inst->operands[0].id, "%rax");
                if (inst->operands[1].kind == IR_VALUE_CONSTANT_INT)
                    fprintf(out, "  movq $%lld, (%%rax)\n", inst->operands[1].as.int_val);
                else if (inst->operands[1].kind == IR_VALUE_REGISTER) {
                    LOAD_TO(inst->operands[1].id, "%rcx");
                    fprintf(out, "  movq %%rcx, (%%rax)\n");
                }
            }
            break;
        case IR_OP_MEMBER_ACCESS:
            if (inst->result.kind == IR_VALUE_REGISTER && inst->operand_count >= 2) {
                LOAD_TO(inst->operands[0].id, "%rax");
                if (inst->operands[1].kind == IR_VALUE_CONSTANT_INT) {
                    fprintf(out, "  addq $%lld, %%rax\n", inst->operands[1].as.int_val);
                }
                STORE(inst->result.id);
            }
            break;
        case IR_OP_LEA:
            if (inst->result.kind == IR_VALUE_REGISTER && inst->operand_count >= 2) {
                if (inst->operands[0].kind == IR_VALUE_REGISTER) {
                    fprintf(out, "  movq %d(%%rbp), %%rax\n", SLOT(inst->operands[0].id));
                    if (inst->operands[1].kind == IR_VALUE_CONSTANT_INT && inst->operands[1].as.int_val != 0) {
                        fprintf(out, "  addq $%lld, %%rax\n", inst->operands[1].as.int_val);
                    }
                    STORE(inst->result.id);
                }
            }
            break;
        case IR_OP_CALL:
            if (inst->operand_count > 0) {
                const char* func_name = ir_value_name(inst->operands[0]);
                
                if (strcmp(func_name, "اطبع") == 0) {
                    /* نص: الملصق من مجمّع الوحدة والطول مسجل عند الإصدار */
                    if (inst->operand_count > 1 &&
                        inst->operands[1].kind == IR_VALUE_CONSTANT_STRING) {
                        const char* lbl = inst->operands[1].as.string_val;
                        int len = string_length_of(lbl);
                        if (len < 0) len = 1;
                        /* syscall يدمّر rcx/r11/rax — عاملها حاجزًا يحفظ كل شيء */
                        fprintf(out, "  pushq %%rax\n  pushq %%rcx\n  pushq %%rsi\n"
                                     "  pushq %%rdi\n  pushq %%r8\n  pushq %%r9\n"
                                     "  pushq %%r10\n  pushq %%r11\n");
                        fprintf(out, "  leaq %s(%%rip), %%rsi\n", lbl);
                        fprintf(out, "  movq $%d, %%rdx\n", len);
                        fprintf(out, "  movq $1, %%rax\n");
                        fprintf(out, "  movq $1, %%rdi\n");
                        fprintf(out, "  syscall\n");
                        fprintf(out, "  popq %%r11\n  popq %%r10\n  popq %%r9\n"
                                     "  popq %%r8\n  popq %%rdi\n  popq %%rsi\n"
                                     "  popq %%rcx\n  popq %%rax\n");
                    } else if (inst->operand_count > 1 &&
                               inst->operands[1].kind != IR_VALUE_LABEL) {
                        /* رقم: حمّل إلى rdi ونادِ المساعد مع حفظ السجلات الحية */
                        fprintf(out, "  # اطبع رقم\n");
                        /* احفظ السجلات أولًا: قد يحمل أحدها قيمة حية يعتمدها
                           الكود اللاحق، وتحميل المعامل إلى rdi بعده آمن */
                        fprintf(out, "  pushq %%rax\n  pushq %%rcx\n  pushq %%rsi\n"
                                     "  pushq %%rdi\n  pushq %%r8\n  pushq %%r9\n"
                                     "  pushq %%r10\n  pushq %%r11\n");
                        LOAD_TO(inst->operands[1].id, "%rdi");
                        fprintf(out, "  callq __daad_print_int\n");
                        fprintf(out, "  popq %%r11\n  popq %%r10\n  popq %%r9\n"
                                     "  popq %%r8\n  popq %%rdi\n  popq %%rsi\n"
                                     "  popq %%rcx\n  popq %%rax\n");
                    }
                } else if (strcmp(func_name, "توقف") == 0) {
                    fprintf(out, "  movq $60, %%rax\n");
                    fprintf(out, "  movq $0, %%rdi\n");
                    fprintf(out, "  syscall\n");
                } else if (strcmp(func_name, "ادخل") == 0) {
                    /* ادخل: نادِ __daad_read_int → rax = العدد
                       لا نحفظ/نستعي rax لأنه يحمل النتيجة */
                    fprintf(out, "  # ادخل: قراءة عدد من stdin\n");
                    fprintf(out, "  pushq %%rcx\n  pushq %%rsi\n"
                                 "  pushq %%rdi\n  pushq %%r8\n  pushq %%r9\n"
                                 "  pushq %%r10\n  pushq %%r11\n");
                    fprintf(out, "  callq __daad_read_int\n");
                    fprintf(out, "  popq %%r11\n  popq %%r10\n  popq %%r9\n"
                                 "  popq %%r8\n  popq %%rdi\n  popq %%rsi\n"
                                 "  popq %%rcx\n");
                    if (inst->result.kind == IR_VALUE_REGISTER)
                        STORE(inst->result.id);
                } else if (strcmp(func_name, "افتح") == 0) {
                    fprintf(out, "  # fopen\n");
                    if (inst->operand_count > 1) {
                        if (inst->operands[1].kind == IR_VALUE_CONSTANT_STRING)
                            fprintf(out, "  leaq %s(%%rip), %%rdi\n", inst->operands[1].as.string_val);
                        else if (inst->operands[1].kind == IR_VALUE_REGISTER)
                            LOAD_TO(inst->operands[1].id, "%rdi");
                    }
                    if (inst->operand_count > 2) {
                        if (inst->operands[2].kind == IR_VALUE_CONSTANT_STRING)
                            fprintf(out, "  leaq %s(%%rip), %%rsi\n", inst->operands[2].as.string_val);
                        else if (inst->operands[2].kind == IR_VALUE_REGISTER)
                            LOAD_TO(inst->operands[2].id, "%rsi");
                    }
                    fprintf(out, "  callq fopen\n");
                    if (inst->result.kind == IR_VALUE_REGISTER)
                        STORE(inst->result.id);
                } else if (strcmp(func_name, "اقرأ_ملف") == 0) {
                    fprintf(out, "  # fread\n");
                    if (inst->operand_count > 1) {
                        if (inst->operands[1].kind == IR_VALUE_REGISTER)
                            LOAD_TO(inst->operands[1].id, "%rdi");
                    }
                    if (inst->operand_count > 2) {
                        if (inst->operands[2].kind == IR_VALUE_REGISTER)
                            LOAD_TO(inst->operands[2].id, "%rsi");
                    }
                    fprintf(out, "  movq $1, %%rdx\n");
                    fprintf(out, "  callq fread\n");
                    if (inst->result.kind == IR_VALUE_REGISTER)
                        STORE(inst->result.id);
                } else if (strcmp(func_name, "اكتب_ملف") == 0) {
                    fprintf(out, "  # fwrite\n");
                    if (inst->operand_count > 1) {
                        if (inst->operands[1].kind == IR_VALUE_REGISTER)
                            LOAD_TO(inst->operands[1].id, "%rdi");
                    }
                    if (inst->operand_count > 2) {
                        if (inst->operands[2].kind == IR_VALUE_REGISTER)
                            LOAD_TO(inst->operands[2].id, "%rsi");
                    }
                    fprintf(out, "  movq $1, %%rdx\n");
                    fprintf(out, "  callq fwrite\n");
                    if (inst->result.kind == IR_VALUE_REGISTER)
                        STORE(inst->result.id);
                } else if (strcmp(func_name, "اغلق") == 0) {
                    fprintf(out, "  # fclose\n");
                    if (inst->operand_count > 1) {
                        if (inst->operands[1].kind == IR_VALUE_REGISTER)
                            LOAD_TO(inst->operands[1].id, "%rdi");
                    }
                    fprintf(out, "  callq fclose\n");
                } else if (strcmp(func_name, "احجز") == 0) {
                    fprintf(out, "  # malloc\n");
                    if (inst->operand_count > 1) {
                        if (inst->operands[1].kind == IR_VALUE_CONSTANT_INT)
                            fprintf(out, "  movq $%lld, %%rdi\n", inst->operands[1].as.int_val);
                        else if (inst->operands[1].kind == IR_VALUE_REGISTER)
                            LOAD_TO(inst->operands[1].id, "%rdi");
                    }
                    fprintf(out, "  callq malloc\n");
                    if (inst->result.kind == IR_VALUE_REGISTER)
                        STORE(inst->result.id);
                } else if (strcmp(func_name, "حرر") == 0) {
                    fprintf(out, "  # free\n");
                    if (inst->operand_count > 1) {
                        if (inst->operands[1].kind == IR_VALUE_REGISTER)
                            LOAD_TO(inst->operands[1].id, "%rdi");
                    }
                    fprintf(out, "  callq free\n");
                } else if (strcmp(func_name, "غيّر_حجم") == 0) {
                    fprintf(out, "  # realloc\n");
                    if (inst->operand_count > 1) {
                        if (inst->operands[1].kind == IR_VALUE_REGISTER)
                            LOAD_TO(inst->operands[1].id, "%rdi");
                    }
                    if (inst->operand_count > 2) {
                        if (inst->operands[2].kind == IR_VALUE_CONSTANT_INT)
                            fprintf(out, "  movq $%lld, %%rsi\n", inst->operands[2].as.int_val);
                        else if (inst->operands[2].kind == IR_VALUE_REGISTER)
                            LOAD_TO(inst->operands[2].id, "%rsi");
                    }
                    fprintf(out, "  callq realloc\n");
                    if (inst->result.kind == IR_VALUE_REGISTER)
                        STORE(inst->result.id);
                } else {
                    for (int i = 0; i < ctx->reg_alloc->interval_count; i++) {
                        LiveInterval* iv = &ctx->reg_alloc->intervals[i];
                        if (iv->phys_reg != REG_NONE && !iv->is_spilled &&
                            reg_alloc_is_caller_saved(ctx->reg_alloc, iv->phys_reg)) {
                            fprintf(out, "  movq %s, %d(%%rbp)\n",
                                    reg_alloc_phys_name(iv->phys_reg), -(iv->ir_reg * 8));
                        }
                    }
                    
                    static const char* arg_regs[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
                    for (int a = 1; a < inst->operand_count && a <= 6; a++) {
                        if (inst->operands[a].kind == IR_VALUE_CONSTANT_INT)
                            fprintf(out, "  movq $%lld, %s\n", inst->operands[a].as.int_val, arg_regs[a - 1]);
                        else if (inst->operands[a].kind == IR_VALUE_REGISTER)
                            fprintf(out, "  movq %d(%%rbp), %s\n", SLOT(inst->operands[a].id), arg_regs[a - 1]);
                    }
                    fprintf(out, "  callq %s\n", func_name);
                    if (inst->result.kind == IR_VALUE_REGISTER)
                        STORE(inst->result.id);
                        
                    for (int i = 0; i < ctx->reg_alloc->interval_count; i++) {
                        LiveInterval* iv = &ctx->reg_alloc->intervals[i];
                        if (iv->phys_reg != REG_NONE && !iv->is_spilled &&
                            reg_alloc_is_caller_saved(ctx->reg_alloc, iv->phys_reg)) {
                            fprintf(out, "  movq %d(%%rbp), %s\n",
                                    -(iv->ir_reg * 8), reg_alloc_phys_name(iv->phys_reg));
                        }
                    }
                }
            }
            break;
        case IR_OP_RET:
            if (inst->operand_count > 0) {
                if (inst->operands[0].kind == IR_VALUE_CONSTANT_INT)
                    fprintf(out, "  movq $%lld, %%rax\n", inst->operands[0].as.int_val);
                else if (inst->operands[0].kind == IR_VALUE_REGISTER) {
                    LOAD_TO(inst->operands[0].id, "%rax");
                }
            }
            fprintf(out, "  jmp .L_%s_epilogue\n", ctx->current_func->name);
            break;
        case IR_OP_PHI:
            break;
        default:
            fprintf(out, "  # unimplemented opcode %s\n", ir_opcode_name(inst->opcode));
            break;
    }

    #undef SLOT
    #undef REG_NAME
    #undef LOAD_TO
    #undef STORE
    return 1;
}

static void emit_phi_moves_for_block(IRFunction* func, IRBasicBlock* bb, BackendContext* ctx, FILE* out) {
    (void)func; (void)ctx;
    for (int j = 0; j < bb->instruction_count; j++) {
        IRInstruction* inst = &bb->instructions[j];
        if (inst->opcode != IR_OP_PHI) continue;
        
        for (int p = 0; p < bb->predecessor_count && p < inst->operand_count; p++) {
            IRBasicBlock* pred = bb->predecessors[p];
            if (!pred) continue;
            
            IRValue incoming = inst->operands[p];
            if (incoming.kind == IR_VALUE_NULL) continue;
            
            int last_idx = pred->instruction_count - 1;
            if (last_idx < 0) continue;
            
            IRInstruction* terminator = &pred->instructions[last_idx];
            int is_terminator = (terminator->opcode == IR_OP_JMP || 
                                terminator->opcode == IR_OP_JE ||
                                terminator->opcode == IR_OP_JNE ||
                                terminator->opcode == IR_OP_JG ||
                                terminator->opcode == IR_OP_JL ||
                                terminator->opcode == IR_OP_JGE ||
                                terminator->opcode == IR_OP_JLE ||
                                terminator->opcode == IR_OP_RET);
            
            if (!is_terminator) continue;
            
            if (inst->result.kind == IR_VALUE_REGISTER) {
                if (incoming.kind == IR_VALUE_CONSTANT_INT) {
                    fprintf(out, "  movq $%lld, %d(%%rbp)\n", incoming.as.int_val, -(inst->result.id * 8));
                } else if (incoming.kind == IR_VALUE_REGISTER) {
                    fprintf(out, "  movq %d(%%rbp), %%rax\n", -(incoming.id * 8));
                    fprintf(out, "  movq %%rax, %d(%%rbp)\n", -(inst->result.id * 8));
                }
            }
        }
    }
}

static int emit_x86_function(IRFunction* func, FILE* out) {
    if (!func || !out) return 0;

    BackendContext* ctx = backend_context_create();
    if (!ctx) return 0;
    ctx->current_func = func;

    int max_id = 0;
    for (int i = 0; i < func->block_count; i++) {
        IRBasicBlock* bb = func->blocks[i];
        for (int j = 0; j < bb->instruction_count; j++) {
            IRInstruction* inst = &bb->instructions[j];
            if (inst->result.kind == IR_VALUE_REGISTER && inst->result.id > max_id)
                max_id = inst->result.id;
            for (int o = 0; o < inst->operand_count; o++) {
                if (inst->operands[o].kind == IR_VALUE_REGISTER && inst->operands[o].id > max_id)
                    max_id = inst->operands[o].id;
            }
        }
    }

    ctx->max_reg_id = max_id;
    
    backend_build_intervals(ctx, func);
    backend_allocate_registers(ctx);
    
    int frame_size = (ctx->max_reg_id + 20) * 8;
    if (frame_size % 16 != 0) frame_size = ((frame_size + 15) / 16) * 16;
    if (frame_size < 32) frame_size = 32;

    fprintf(out, ".globl %s\n", func->name);
    fprintf(out, "%s:\n", func->name);
    fprintf(out, "  pushq %%rbp\n");
    fprintf(out, "  movq %%rsp, %%rbp\n");
    
    for (int i = 0; i < ctx->callee_saved_count; i++) {
        fprintf(out, "  pushq %s\n", reg_alloc_phys_name(ctx->callee_saved_regs[i]));
    }
    
    int push_adjustment = ctx->callee_saved_count * 8;
    int total_frame = frame_size + push_adjustment;
    if (total_frame % 16 != 0) total_frame = ((total_frame + 15) / 16) * 16;
    fprintf(out, "  subq $%d, %%rsp\n", total_frame);

    static const char* param_regs[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    for (int i = 0; i < func->param_count && i < 6; i++) {
        int off = reg_get_offset(ctx, func->params[i].id);
        fprintf(out, "  movq %s, %d(%%rbp)\n", param_regs[i], off);
    }

    ctx->alloca_counter = 0;
    int inst_count = 0;
    for (int i = 0; i < func->block_count; i++) {
        IRBasicBlock* bb = func->blocks[i];
        emit_phi_moves_for_block(func, bb, ctx, out);
        if (bb->label) {
            fprintf(out, ".L_%s_%s:\n", func->name, bb->label);
        }
        for (int j = 0; j < bb->instruction_count; j++) {
            inst_count += emit_x86_instruction(&bb->instructions[j], out, ctx);
        }
    }

    fprintf(out, ".L_%s_epilogue:\n", func->name);
    fprintf(out, "  addq $%d, %%rsp\n", total_frame);
    for (int i = ctx->callee_saved_count - 1; i >= 0; i--) {
        fprintf(out, "  popq %s\n", reg_alloc_phys_name(ctx->callee_saved_regs[i]));
    }
    fprintf(out, "  leave\n");
    fprintf(out, "  ret\n");

    backend_context_destroy(ctx);
    return inst_count;
}

int backend_emit_module(IRModule* module, BackendTarget target, FILE* out) {
    Backend* b = backend_get(target);
    if (!b || !b->emit_function) {
        backend_destroy(b);
        return 0;
    }
    fprintf(out, "# Generated by DAAD Compiler\n");

    if (target == BACKEND_DHAD) {
        /* DHAD backend: emit functions only.
         * String data emission is handled by the caller (main.c)
         * to control placement order for 8-bit address compatibility. */
        int total = 0;
        for (int i = 0; i < module->function_count; i++) {
            total += b->emit_function(module->functions[i], out);
            fprintf(out, "\n");
        }
        backend_destroy(b);
        return total;
    }

    fprintf(out, ".text\n");

    if (module->string_count > 0) {
        fprintf(out, ".section .rodata\n");
        for (int i = 0; i < module->string_count && i < DAAD_MAX_STRINGS; i++) {
            snprintf(g_str_labels[g_str_count], 64, "%s", module->strings[i].label);
            g_str_lengths[g_str_count] = strlen(module->strings[i].str) + 1;
            g_str_count++;
            fprintf(out, "%s: .asciz \"%s\\n\"\n",
                    module->strings[i].label, module->strings[i].str);
        }
        fprintf(out, "\n.section .text\n\n");
    }

    int total = 0;
    for (int i = 0; i < module->function_count; i++) {
        total += b->emit_function(module->functions[i], out);
        fprintf(out, "\n");
    }

    backend_destroy(b);
    return total;
}

int backend_emit_function(IRFunction* func, BackendTarget target, FILE* out) {
    Backend* b = backend_get(target);
    if (!b || !b->emit_function) {
        backend_destroy(b);
        return 0;
    }
    int count = b->emit_function(func, out);
    backend_destroy(b);
    return count;
}

int backend_emit_instruction(IRInstruction* inst, FILE* out) {
    return emit_x86_instruction(inst, out, NULL);
}

int backend_emit_prologue(IRFunction* func, FILE* out) {
    return emit_x86_prologue(func, out);
}

int backend_emit_epilogue(FILE* out) {
    return emit_x86_epilogue(out);
}

int backend_emit_param_load(int param_index, IRValue* result, FILE* out) {
    if (!out || !result) return 0;
    static const char* param_regs[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    result->kind = IR_VALUE_REGISTER;
    result->id = param_index + 6;
    if (param_index < 6) {
        fprintf(out, "  movq %s, %s\n", param_regs[param_index],
            x86_reg_name_internal(result->id));
    } else {
        fprintf(out, "  movq %d(%%rbp), %s\n",
            x86_get_param_offset(param_index),
            x86_reg_name_internal(result->id));
    }
    return 1;
}

int backend_emit_return(IRValue value, FILE* out) {
    if (!out) return 0;
    if (value.kind == IR_VALUE_CONSTANT_INT) {
        fprintf(out, "  movq $%lld, %%rax\n", value.as.int_val);
    } else if (value.kind == IR_VALUE_CONSTANT_FLOAT) {
        fprintf(out, "  movsd .LC%df, %%xmm0\n", (int)(value.as.float_val * 1000));
    } else if (value.kind == IR_VALUE_REGISTER) {
        if (x86_reg_is_float(value.id)) {
            fprintf(out, "  movsd %s, %%xmm0\n", x86_reg_name_internal(value.id));
        } else {
            fprintf(out, "  movq %s, %%rax\n", x86_reg_name_internal(value.id));
        }
    }
    fprintf(out, "  leave\n");
    fprintf(out, "  ret\n");
    return 1;
}
