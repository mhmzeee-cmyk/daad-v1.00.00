/**
 * @file optimizer_pass.c
 * @brief تنفيذ نظام الـ Passes - محدّث مع دعم Cross-block
 */

#include "optimizer_pass.h"
#include <stdlib.h>
#include <string.h>

static int pass_constant_folding(IRFunction* func, OptPass* pass);
static int pass_constant_propagation(IRFunction* func, OptPass* pass);
static int pass_copy_propagation(IRFunction* func, OptPass* pass);
static int pass_dead_code_elimination(IRFunction* func, OptPass* pass);
static int pass_dead_store_elimination(IRFunction* func, OptPass* pass);
static int pass_algebraic_simplification(IRFunction* func, OptPass* pass);
static int pass_strength_reduction(IRFunction* func, OptPass* pass);
static int pass_jump_optimization(IRFunction* func, OptPass* pass);
static int pass_remove_empty_blocks(IRFunction* func, OptPass* pass);
static int pass_merge_blocks(IRFunction* func, OptPass* pass);
static int pass_peephole(IRFunction* func, OptPass* pass);

typedef int (*PassFunc)(IRFunction*, OptPass*);

static PassFunc pass_functions[] = {
    pass_constant_folding,
    pass_constant_propagation,
    pass_copy_propagation,
    pass_dead_code_elimination,
    pass_dead_store_elimination,
    pass_algebraic_simplification,
    pass_strength_reduction,
    pass_jump_optimization,
    pass_remove_empty_blocks,
    pass_merge_blocks,
    pass_peephole
};

Optimizer* optimizer_create(void) {
    Optimizer* opt = (Optimizer*)calloc(1, sizeof(Optimizer));
    return opt;
}

void optimizer_destroy(Optimizer* opt) {
    if (!opt) return;
    OptPass* p = opt->passes;
    while (p) {
        OptPass* next = p->next;
        free(p);
        p = next;
    }
    free(opt);
}

void optimizer_add_pass(Optimizer* opt, OptPassType type) {
    if (!opt) return;
    OptPass* pass = (OptPass*)calloc(1, sizeof(OptPass));
    if (!pass) return;
    pass->type = type;
    pass->name = optimizer_pass_name(type);
    pass->run = pass_functions[type];
    pass->next = opt->passes;
    opt->passes = pass;
    opt->pass_count++;
}

void optimizer_add_default_passes(Optimizer* opt) {
    if (!opt) return;
    optimizer_add_pass(opt, OPT_PASS_CONSTANT_FOLDING);
    optimizer_add_pass(opt, OPT_PASS_CONSTANT_PROPAGATION);
    optimizer_add_pass(opt, OPT_PASS_ALGEBRAIC_SIMPLIFICATION);
    optimizer_add_pass(opt, OPT_PASS_STRENGTH_REDUCTION);
    optimizer_add_pass(opt, OPT_PASS_COPY_PROPAGATION);
    optimizer_add_pass(opt, OPT_PASS_DEAD_CODE_ELIMINATION);
    optimizer_add_pass(opt, OPT_PASS_DEAD_STORE_ELIMINATION);
    optimizer_add_pass(opt, OPT_PASS_JUMP_OPTIMIZATION);
    optimizer_add_pass(opt, OPT_PASS_REMOVE_EMPTY_BLOCKS);
    optimizer_add_pass(opt, OPT_PASS_MERGE_BLOCKS);
    optimizer_add_pass(opt, OPT_PASS_PEEPHOLE);
}

int optimizer_run_all(Optimizer* opt, IRFunction* func) {
    if (!opt || !func) return 0;
    int total = 0;
    for (int round = 0; round < 3; round++) {
        OptPass* p = opt->passes;
        while (p) {
            total += p->run(func, p);
            p = p->next;
        }
    }
    opt->total_transformations = total;
    return total;
}

int optimizer_run_single(Optimizer* opt, IRFunction* func, OptPassType type) {
    if (!opt || !func) return 0;
    OptPass* p = opt->passes;
    while (p) {
        if (p->type == type) return p->run(func, p);
        p = p->next;
    }
    return 0;
}

const char* optimizer_pass_name(OptPassType type) {
    switch (type) {
        case OPT_PASS_CONSTANT_FOLDING: return "Constant Folding";
        case OPT_PASS_CONSTANT_PROPAGATION: return "Constant Propagation";
        case OPT_PASS_COPY_PROPAGATION: return "Copy Propagation";
        case OPT_PASS_DEAD_CODE_ELIMINATION: return "Dead Code Elimination";
        case OPT_PASS_DEAD_STORE_ELIMINATION: return "Dead Store Elimination";
        case OPT_PASS_ALGEBRAIC_SIMPLIFICATION: return "Algebraic Simplification";
        case OPT_PASS_STRENGTH_REDUCTION: return "Strength Reduction";
        case OPT_PASS_JUMP_OPTIMIZATION: return "Jump Optimization";
        case OPT_PASS_REMOVE_EMPTY_BLOCKS: return "Remove Empty Blocks";
        case OPT_PASS_MERGE_BLOCKS: return "Merge Blocks";
        case OPT_PASS_PEEPHOLE: return "Peephole Optimizer";
    }
    return "Unknown";
}

static int pass_constant_folding(IRFunction* func, OptPass* pass) {
    (void)pass;
    int changed = 0;
    for (int i = 0; i < func->block_count; i++) {
        IRBasicBlock* bb = func->blocks[i];
        for (int j = 0; j < bb->instruction_count; j++) {
            IRInstruction* inst = &bb->instructions[j];
            if ((inst->opcode == IR_OP_ADD || inst->opcode == IR_OP_SUB ||
                 inst->opcode == IR_OP_MUL || inst->opcode == IR_OP_DIV ||
                 inst->opcode == IR_OP_MOD || inst->opcode == IR_OP_AND ||
                 inst->opcode == IR_OP_OR || inst->opcode == IR_OP_XOR ||
                 inst->opcode == IR_OP_SHL || inst->opcode == IR_OP_SHR) &&
                inst->operand_count >= 2 &&
                inst->operands[0].kind == IR_VALUE_CONSTANT_INT &&
                inst->operands[1].kind == IR_VALUE_CONSTANT_INT) {
                long long a = inst->operands[0].as.int_val;
                long long b = inst->operands[1].as.int_val;
                long long result = 0;
                int fold = 1;
                switch (inst->opcode) {
                    case IR_OP_ADD: result = a + b; break;
                    case IR_OP_SUB: result = a - b; break;
                    case IR_OP_MUL: result = a * b; break;
                    case IR_OP_DIV: if (b != 0) result = a / b; else fold = 0; break;
                    case IR_OP_MOD: if (b != 0) result = a % b; else fold = 0; break;
                    case IR_OP_AND: result = a & b; break;
                    case IR_OP_OR: result = a | b; break;
                    case IR_OP_XOR: result = a ^ b; break;
                    case IR_OP_SHL: result = a << b; break;
                    case IR_OP_SHR: result = a >> b; break;
                    default: fold = 0; break;
                }
                if (fold) {
                    inst->opcode = IR_OP_MOV;
                    inst->operands[0] = ir_value_create_int(result, inst->result.type);
                    inst->operands[1] = ir_value_null();
                    inst->operand_count = 1;
                    changed++;
                }
            }
        }
    }
    return changed;
}

static int pass_constant_propagation(IRFunction* func, OptPass* pass) {
    (void)pass;
    int changed = 0;
    for (int i = 0; i < func->block_count; i++) {
        IRBasicBlock* bb = func->blocks[i];
        for (int j = 0; j < bb->instruction_count; j++) {
            IRInstruction* inst = &bb->instructions[j];
            if (inst->opcode == IR_OP_MOV && inst->operands[0].kind == IR_VALUE_CONSTANT_INT &&
                inst->result.kind == IR_VALUE_REGISTER) {
                long long val = inst->operands[0].as.int_val;
                int reg_id = inst->result.id;
                for (int k = j + 1; k < bb->instruction_count; k++) {
                    IRInstruction* use = &bb->instructions[k];
                    for (int o = 0; o < use->operand_count; o++) {
                        if (use->operands[o].kind == IR_VALUE_REGISTER && use->operands[o].id == reg_id) {
                            use->operands[o] = ir_value_create_int(val, use->operands[o].type);
                            changed++;
                        }
                    }
                }
            }
        }
    }
    return changed;
}

static int pass_copy_propagation(IRFunction* func, OptPass* pass) {
    (void)pass;
    int changed = 0;
    for (int i = 0; i < func->block_count; i++) {
        IRBasicBlock* bb = func->blocks[i];
        for (int j = 0; j < bb->instruction_count; j++) {
            IRInstruction* inst = &bb->instructions[j];
            if (inst->opcode == IR_OP_MOV && inst->operands[0].kind == IR_VALUE_REGISTER &&
                inst->result.kind == IR_VALUE_REGISTER) {
                int src = inst->operands[0].id;
                int dst = inst->result.id;
                for (int k = j + 1; k < bb->instruction_count; k++) {
                    IRInstruction* use = &bb->instructions[k];
                    for (int o = 0; o < use->operand_count; o++) {
                        if (use->operands[o].kind == IR_VALUE_REGISTER && use->operands[o].id == dst) {
                            use->operands[o].id = src;
                            changed++;
                        }
                    }
                }
            }
        }
    }
    return changed;
}

static int pass_dead_code_elimination(IRFunction* func, OptPass* pass) {
    (void)pass;
    int changed = 0;
    for (int i = 0; i < func->block_count; i++) {
        IRBasicBlock* bb = func->blocks[i];
        for (int j = 0; j < bb->instruction_count; j++) {
            IRInstruction* inst = &bb->instructions[j];
            if (inst->result.kind == IR_VALUE_REGISTER) {
                int reg_id = inst->result.id;
                int used = 0;
                for (int k = j + 1; k < bb->instruction_count; k++) {
                    IRInstruction* use = &bb->instructions[k];
                    for (int o = 0; o < use->operand_count; o++) {
                        if (use->operands[o].kind == IR_VALUE_REGISTER && use->operands[o].id == reg_id) {
                            used = 1;
                            break;
                        }
                    }
                    if (used) break;
                }
                if (!used && inst->opcode != IR_OP_STORE && inst->opcode != IR_OP_CALL &&
                    inst->opcode != IR_OP_RET && inst->opcode != IR_OP_JMP &&
                    inst->opcode != IR_OP_JE && inst->opcode != IR_OP_JNE &&
                    inst->opcode != IR_OP_JG && inst->opcode != IR_OP_JL &&
                    inst->opcode != IR_OP_JGE && inst->opcode != IR_OP_JLE) {
                    inst->opcode = IR_OP_NOP;
                    changed++;
                }
            }
        }
    }
    return changed;
}

static int pass_dead_store_elimination(IRFunction* func, OptPass* pass) {
    (void)pass;
    int changed = 0;
    for (int i = 0; i < func->block_count; i++) {
        IRBasicBlock* bb = func->blocks[i];
        for (int j = 0; j < bb->instruction_count; j++) {
            IRInstruction* inst = &bb->instructions[j];
            if (inst->opcode == IR_OP_STORE && inst->operand_count >= 2 &&
                inst->operands[0].kind == IR_VALUE_REGISTER) {
                int target_reg = inst->operands[0].id;
                int overwritten = 0;
                int loaded_between = 0;
                for (int k = j + 1; k < bb->instruction_count; k++) {
                    IRInstruction* next = &bb->instructions[k];
                    /* Check if the same target is loaded before being overwritten */
                    if (next->opcode == IR_OP_LOAD && next->operand_count >= 1 &&
                        next->operands[0].kind == IR_VALUE_REGISTER &&
                        next->operands[0].id == target_reg) {
                        loaded_between = 1;
                    }
                    if (next->opcode == IR_OP_STORE && next->operand_count >= 2 &&
                        next->operands[0].kind == IR_VALUE_REGISTER &&
                        next->operands[0].id == target_reg) {
                        overwritten = 1;
                        break;
                    }
                }
                if (overwritten && !loaded_between) {
                    inst->opcode = IR_OP_NOP;
                    changed++;
                }
            }
        }
    }
    return changed;
}

static int pass_algebraic_simplification(IRFunction* func, OptPass* pass) {
    (void)pass;
    int changed = 0;
    for (int i = 0; i < func->block_count; i++) {
        IRBasicBlock* bb = func->blocks[i];
        for (int j = 0; j < bb->instruction_count; j++) {
            IRInstruction* inst = &bb->instructions[j];
            if (inst->opcode == IR_OP_ADD && inst->operand_count >= 2) {
                if (inst->operands[1].kind == IR_VALUE_CONSTANT_INT && inst->operands[1].as.int_val == 0) {
                    inst->opcode = IR_OP_MOV;
                    inst->operands[1] = ir_value_null();
                    inst->operand_count = 1;
                    changed++;
                } else if (inst->operands[0].kind == IR_VALUE_CONSTANT_INT && inst->operands[0].as.int_val == 0) {
                    inst->opcode = IR_OP_MOV;
                    inst->operands[0] = inst->operands[1];
                    inst->operands[1] = ir_value_null();
                    inst->operand_count = 1;
                    changed++;
                }
            }
            if (inst->opcode == IR_OP_MUL && inst->operand_count >= 2) {
                if (inst->operands[1].kind == IR_VALUE_CONSTANT_INT && inst->operands[1].as.int_val == 1) {
                    inst->opcode = IR_OP_MOV;
                    inst->operands[1] = ir_value_null();
                    inst->operand_count = 1;
                    changed++;
                } else if (inst->operands[1].kind == IR_VALUE_CONSTANT_INT && inst->operands[1].as.int_val == 0) {
                    inst->opcode = IR_OP_MOV;
                    inst->operands[0] = ir_value_create_int(0, inst->result.type);
                    inst->operands[1] = ir_value_null();
                    inst->operand_count = 1;
                    changed++;
                }
            }
            if (inst->opcode == IR_OP_SUB && inst->operand_count >= 2) {
                if (inst->operands[1].kind == IR_VALUE_CONSTANT_INT && inst->operands[1].as.int_val == 0) {
                    inst->opcode = IR_OP_MOV;
                    inst->operands[1] = ir_value_null();
                    inst->operand_count = 1;
                    changed++;
                }
            }
        }
    }
    return changed;
}

static int pass_strength_reduction(IRFunction* func, OptPass* pass) {
    (void)pass;
    int changed = 0;
    for (int i = 0; i < func->block_count; i++) {
        IRBasicBlock* bb = func->blocks[i];
        for (int j = 0; j < bb->instruction_count; j++) {
            IRInstruction* inst = &bb->instructions[j];
            if (inst->opcode == IR_OP_MUL && inst->operand_count >= 2 &&
                inst->operands[1].kind == IR_VALUE_CONSTANT_INT) {
                long long val = inst->operands[1].as.int_val;
                if (val > 0 && (val & (val - 1)) == 0) {
                    int shift = 0;
                    long long tmp = val;
                    while (tmp > 1) { tmp >>= 1; shift++; }
                    inst->opcode = IR_OP_SHL;
                    inst->operands[1] = ir_value_create_int(shift, inst->operands[1].type);
                    changed++;
                }
            }
        }
    }
    return changed;
}

static int pass_jump_optimization(IRFunction* func, OptPass* pass) {
    (void)pass;
    int changed = 0;
    for (int i = 0; i < func->block_count; i++) {
        IRBasicBlock* bb = func->blocks[i];
        for (int j = 0; j < bb->instruction_count; j++) {
            IRInstruction* inst = &bb->instructions[j];
            if (inst->opcode == IR_OP_JE && inst->operand_count >= 4 &&
                inst->operands[0].kind == IR_VALUE_CONSTANT_INT) {
                int cond = (int)inst->operands[0].as.int_val;
                const char* target = cond ? inst->operands[2].as.label_val : inst->operands[3].as.label_val;
                inst->opcode = IR_OP_JMP;
                inst->operands[0] = ir_value_create_label(target);
                inst->operands[1] = ir_value_null();
                inst->operands[2] = ir_value_null();
                inst->operands[3] = ir_value_null();
                inst->operand_count = 1;
                changed++;
            }
        }
    }
    return changed;
}

static int pass_remove_empty_blocks(IRFunction* func, OptPass* pass) {
    (void)pass;
    int changed = 0;
    for (int i = func->block_count - 1; i >= 0; i--) {
        IRBasicBlock* bb = func->blocks[i];
        if (bb == func->entry) continue;
        int has_real = 0;
        for (int j = 0; j < bb->instruction_count; j++) {
            if (bb->instructions[j].opcode != IR_OP_NOP) {
                has_real = 1;
                break;
            }
        }
        if (!has_real && bb->successor_count == 1) {
            IRBasicBlock* succ = bb->successors[0];
            for (int p = 0; p < bb->predecessor_count; p++) {
                IRBasicBlock* pred = bb->predecessors[p];
                for (int s = 0; s < pred->successor_count; s++) {
                    if (pred->successors[s] == bb) {
                        pred->successors[s] = succ;
                        break;
                    }
                }
                int already_pred = 0;
                for (int s = 0; s < succ->predecessor_count; s++) {
                    if (succ->predecessors[s] == pred) {
                        already_pred = 1;
                        break;
                    }
                }
                if (!already_pred && succ->predecessor_count < 4) {
                    succ->predecessors[succ->predecessor_count++] = pred;
                }
            }
            bb->instruction_count = 0;
            bb->predecessor_count = 0;
            bb->successor_count = 0;
            changed++;
        }
    }
    return changed;
}

static int pass_merge_blocks(IRFunction* func, OptPass* pass) {
    (void)pass;
    int changed = 0;
    for (int i = 0; i < func->block_count - 1; i++) {
        IRBasicBlock* bb = func->blocks[i];
        if (bb->instruction_count > 0 && bb->successor_count == 1 &&
            bb->predecessor_count <= 1) {
            IRInstruction* last = ir_bb_get_last_instruction(bb);
            if (last && last->opcode == IR_OP_JMP && last->operand_count > 0) {
                const char* target_label = last->operands[0].as.label_val;
                for (int k = 0; k < func->block_count; k++) {
                    if (k != i && func->blocks[k]->label &&
                        strcmp(func->blocks[k]->label, target_label) == 0) {
                        if (func->blocks[k]->predecessor_count == 1) {
                            last->opcode = IR_OP_NOP;
                            last->operand_count = 0;
                            for (int m = 0; m < func->blocks[k]->instruction_count; m++) {
                                if (bb->instruction_count < IR_MAX_INSTRUCTIONS_PER_BLOCK) {
                                    bb->instructions[bb->instruction_count++] = func->blocks[k]->instructions[m];
                                }
                            }
                            func->blocks[k]->instruction_count = 0;
                            changed++;
                        }
                        break;
                    }
                }
            }
        }
    }
    return changed;
}

static int pass_peephole(IRFunction* func, OptPass* pass) {
    (void)pass;
    int changed = 0;
    for (int i = 0; i < func->block_count; i++) {
        IRBasicBlock* bb = func->blocks[i];
        for (int j = 0; j < bb->instruction_count - 2; j++) {
            IRInstruction* i1 = &bb->instructions[j];
            IRInstruction* i2 = &bb->instructions[j + 1];
            IRInstruction* i3 = &bb->instructions[j + 2];
            if (i1->opcode == IR_OP_MOV && i2->opcode == IR_OP_MOV &&
                i1->result.kind == IR_VALUE_REGISTER && i2->operands[0].kind == IR_VALUE_REGISTER &&
                i2->operands[0].id == i1->result.id && i3->opcode == IR_OP_MOV &&
                i3->operands[0].kind == IR_VALUE_REGISTER && i3->operands[0].id == i1->result.id) {
                i2->opcode = IR_OP_NOP;
                i3->operands[0] = i1->operands[0];
                changed++;
            }
            if (i1->opcode == IR_OP_MOV && i2->opcode == IR_OP_MOV &&
                i1->result.kind == IR_VALUE_REGISTER && i1->operands[0].kind == IR_VALUE_CONSTANT_INT &&
                i2->result.kind == IR_VALUE_REGISTER && i2->operands[0].kind == IR_VALUE_REGISTER &&
                i2->operands[0].id == i1->result.id) {
                i2->opcode = IR_OP_MOV;
                i2->operands[0] = i1->operands[0];
                i1->opcode = IR_OP_NOP;
                changed++;
            }
        }
    }
    return changed;
}
