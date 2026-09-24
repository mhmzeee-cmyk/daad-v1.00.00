/**
 * @file ir_verifier.c
 * @brief تنفيذ مدقق IR - IR Verifier
 */

#include "ir_verifier.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

IRVerifier* ir_verifier_create(void) {
    IRVerifier* verifier = (IRVerifier*)calloc(1, sizeof(IRVerifier));
    return verifier;
}

void ir_verifier_destroy(IRVerifier* verifier) {
    free(verifier);
}

int ir_verifier_verify_function(IRVerifier* verifier, IRFunction* func) {
    if (!verifier || !func) return 0;
    verifier->errors_found = 0;
    verifier->warnings_found = 0;
    if (!func->name) {
        snprintf(verifier->last_error, sizeof(verifier->last_error),
            "Function has no name");
        verifier->errors_found++;
        return 0;
    }
    if (func->block_count == 0) {
        snprintf(verifier->last_error, sizeof(verifier->last_error),
            "Function '%s' has no blocks", func->name);
        verifier->errors_found++;
        return 0;
    }
    ir_verifier_verify_instructions(verifier, func);
    ir_verifier_verify_operands(verifier, func);
    ir_verifier_verify_registers(verifier, func);
    ir_verifier_verify_phi(verifier, func);
    return verifier->errors_found == 0;
}

int ir_verifier_verify_module(IRVerifier* verifier, IRModule* module) {
    if (!verifier || !module) return 0;
    int all_valid = 1;
    for (int i = 0; i < module->function_count; i++) {
        if (!ir_verifier_verify_function(verifier, module->functions[i])) {
            all_valid = 0;
        }
    }
    return all_valid;
}

int ir_verifier_verify_cfg(IRVerifier* verifier, CFG* cfg) {
    if (!verifier || !cfg) return 0;
    for (int i = 0; i < cfg->block_count; i++) {
        IRBasicBlock* bb = cfg->blocks[i];
        for (int p = 0; p < bb->predecessor_count; p++) {
            IRBasicBlock* pred = bb->predecessors[p];
            int found = 0;
            for (int s = 0; s < pred->successor_count; s++) {
                if (pred->successors[s] == bb) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                snprintf(verifier->last_error, sizeof(verifier->last_error),
                    "Block '%s' has predecessor '%s' but predecessor doesn't have this as successor",
                    bb->label ? bb->label : "?", pred->label ? pred->label : "?");
                verifier->errors_found++;
            }
        }
        for (int s = 0; s < bb->successor_count; s++) {
            IRBasicBlock* succ = bb->successors[s];
            int found = 0;
            for (int p = 0; p < succ->predecessor_count; p++) {
                if (succ->predecessors[p] == bb) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                snprintf(verifier->last_error, sizeof(verifier->last_error),
                    "Block '%s' has successor '%s' but successor doesn't have this as predecessor",
                    bb->label ? bb->label : "?", succ->label ? succ->label : "?");
                verifier->errors_found++;
            }
        }
    }
    return verifier->errors_found == 0;
}

int ir_verifier_verify_instructions(IRVerifier* verifier, IRFunction* func) {
    if (!verifier || !func) return 0;
    for (int i = 0; i < func->block_count; i++) {
        IRBasicBlock* bb = func->blocks[i];
        for (int j = 0; j < bb->instruction_count; j++) {
            IRInstruction* inst = &bb->instructions[j];
            if (inst->opcode < 0 || inst->opcode > IR_OP_LEA) {
                snprintf(verifier->last_error, sizeof(verifier->last_error),
                    "Invalid opcode %d in block '%s'", inst->opcode,
                    bb->label ? bb->label : "?");
                verifier->errors_found++;
            }
            if (inst->operand_count < 0 || inst->operand_count > 4) {
                snprintf(verifier->last_error, sizeof(verifier->last_error),
                    "Invalid operand count %d in block '%s'", inst->operand_count,
                    bb->label ? bb->label : "?");
                verifier->errors_found++;
            }
        }
    }
    return verifier->errors_found == 0;
}

int ir_verifier_verify_operands(IRVerifier* verifier, IRFunction* func) {
    if (!verifier || !func) return 0;
    for (int i = 0; i < func->block_count; i++) {
        IRBasicBlock* bb = func->blocks[i];
        for (int j = 0; j < bb->instruction_count; j++) {
            IRInstruction* inst = &bb->instructions[j];
            for (int o = 0; o < inst->operand_count; o++) {
                IRValue* op = &inst->operands[o];
                if (op->kind == IR_VALUE_REGISTER) {
                    if (op->id < 0 || op->id > func->next_reg) {
                        snprintf(verifier->last_error, sizeof(verifier->last_error),
                            "Register operand %d out of range in block '%s'", op->id,
                            bb->label ? bb->label : "?");
                        verifier->errors_found++;
                    }
                }
            }
            if (inst->result.kind == IR_VALUE_REGISTER) {
                if (inst->result.id < 0 || inst->result.id > func->next_reg) {
                    snprintf(verifier->last_error, sizeof(verifier->last_error),
                        "Result register %d out of range in block '%s'", inst->result.id,
                        bb->label ? bb->label : "?");
                    verifier->errors_found++;
                }
            }
        }
    }
    return verifier->errors_found == 0;
}

int ir_verifier_verify_registers(IRVerifier* verifier, IRFunction* func) {
    if (!verifier || !func) return 0;
    int* defined = (int*)calloc(func->next_reg + 1, sizeof(int));
    if (!defined) return 0;
    for (int i = 0; i < func->block_count; i++) {
        IRBasicBlock* bb = func->blocks[i];
        for (int j = 0; j < bb->instruction_count; j++) {
            IRInstruction* inst = &bb->instructions[j];
            if (inst->result.kind == IR_VALUE_REGISTER) {
                if (defined[inst->result.id]) {
                    snprintf(verifier->last_warning, sizeof(verifier->last_warning),
                        "Register %d defined multiple times in block '%s'", inst->result.id,
                        bb->label ? bb->label : "?");
                    verifier->warnings_found++;
                }
                defined[inst->result.id] = 1;
            }
        }
    }
    free(defined);
    return verifier->errors_found == 0;
}

int ir_verifier_verify_phi(IRVerifier* verifier, IRFunction* func) {
    if (!verifier || !func) return 0;
    for (int i = 0; i < func->block_count; i++) {
        IRBasicBlock* bb = func->blocks[i];
        for (int j = 0; j < bb->instruction_count; j++) {
            IRInstruction* inst = &bb->instructions[j];
            if (inst->opcode == IR_OP_PHI) {
                if (inst->operand_count % 2 != 0) {
                    snprintf(verifier->last_error, sizeof(verifier->last_error),
                        "PHI instruction has odd number of operands in block '%s'",
                        bb->label ? bb->label : "?");
                    verifier->errors_found++;
                }
            }
        }
    }
    return verifier->errors_found == 0;
}

const char* ir_verifier_get_error(IRVerifier* verifier) {
    if (!verifier) return NULL;
    return verifier->errors_found > 0 ? verifier->last_error : NULL;
}

const char* ir_verifier_get_warning(IRVerifier* verifier) {
    if (!verifier) return NULL;
    return verifier->warnings_found > 0 ? verifier->last_warning : NULL;
}
