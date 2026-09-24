/**
 * @file ir_validate.c
 * @brief تنفيذ التحقق من صحة IR
 */

#include "ir_validate.h"
#include <stdio.h>
#include <string.h>

static IRValidationResult make_valid(void) {
    IRValidationResult r;
    r.is_valid = 1;
    r.error_message[0] = '\0';
    r.error_line = 0;
    r.error_col = 0;
    return r;
}

static IRValidationResult make_invalid(const char* msg, size_t line, size_t col) {
    IRValidationResult r;
    r.is_valid = 0;
    strncpy(r.error_message, msg, sizeof(r.error_message) - 1);
    r.error_message[sizeof(r.error_message) - 1] = '\0';
    r.error_line = line;
    r.error_col = col;
    return r;
}

IRValidationResult ir_validate_module(IRModule* module) {
    if (!module) return make_invalid("Module is NULL", 0, 0);
    for (int i = 0; i < module->function_count; i++) {
        IRValidationResult r = ir_validate_function(module->functions[i]);
        if (!r.is_valid) return r;
    }
    return make_valid();
}

IRValidationResult ir_validate_function(IRFunction* func) {
    if (!func) return make_invalid("Function is NULL", 0, 0);
    if (!func->name) return make_invalid("Function name is NULL", 0, 0);
    if (func->block_count == 0) return make_invalid("Function has no blocks", 0, 0);
    if (!func->entry) return make_invalid("Function has no entry block", 0, 0);
    if (!func->exit) return make_invalid("Function has no exit block", 0, 0);

    for (int i = 0; i < func->block_count; i++) {
        IRValidationResult r = ir_validate_basic_block(func->blocks[i], func);
        if (!r.is_valid) return r;
    }
    return make_valid();
}

IRValidationResult ir_validate_basic_block(IRBasicBlock* bb, IRFunction* func) {
    if (!bb) return make_invalid("Basic block is NULL", 0, 0);
    if (!bb->label) return make_invalid("Basic block label is NULL", 0, 0);

    for (int i = 0; i < bb->instruction_count; i++) {
        IRValidationResult r = ir_validate_instruction(&bb->instructions[i], func);
        if (!r.is_valid) return r;
    }

    if (bb->instruction_count > 0) {
        IRInstruction* last = &bb->instructions[bb->instruction_count - 1];
        int is_terminator = (last->opcode == IR_OP_JMP ||
                            last->opcode == IR_OP_JE ||
                            last->opcode == IR_OP_JNE ||
                            last->opcode == IR_OP_JG ||
                            last->opcode == IR_OP_JL ||
                            last->opcode == IR_OP_JGE ||
                            last->opcode == IR_OP_JLE ||
                            last->opcode == IR_OP_RET);
        if (!is_terminator) {
            return make_invalid("Basic block does not end with terminator", 0, 0);
        }
    }

    return make_valid();
}

IRValidationResult ir_validate_instruction(IRInstruction* inst, IRFunction* func) {
    if (!inst) return make_invalid("Instruction is NULL", 0, 0);
    (void)func;

    switch (inst->opcode) {
        case IR_OP_ALLOCA:
        case IR_OP_LOAD:
        case IR_OP_STORE:
        case IR_OP_MOV:
        case IR_OP_ADD:
        case IR_OP_SUB:
        case IR_OP_MUL:
        case IR_OP_DIV:
        case IR_OP_MOD:
        case IR_OP_NEG:
        case IR_OP_NOT:
        case IR_OP_AND:
        case IR_OP_OR:
        case IR_OP_XOR:
        case IR_OP_SHL:
        case IR_OP_SHR:
        case IR_OP_CMP:
        case IR_OP_CAST:
            if (inst->result.kind == IR_VALUE_NULL) {
                return make_invalid("Instruction missing result", inst->source_line, inst->source_col);
            }
            break;
        case IR_OP_JMP:
        case IR_OP_JE:
        case IR_OP_JNE:
        case IR_OP_JG:
        case IR_OP_JL:
        case IR_OP_JGE:
        case IR_OP_JLE:
        case IR_OP_RET:
        case IR_OP_PUSH:
        case IR_OP_POP:
        case IR_OP_PHI:
        case IR_OP_LOAD_ELEMENT:
        case IR_OP_STORE_ELEMENT:
        case IR_OP_MEMBER_ACCESS:
        case IR_OP_LEA:
        case IR_OP_CALL:
        case IR_OP_NOP:
            break;
    }

    return make_valid();
}
