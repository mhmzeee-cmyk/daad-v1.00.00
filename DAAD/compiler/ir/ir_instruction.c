/**
 * @file ir_instruction.c
 * @brief تنفيذ تعليمات IR
 */

#include "ir_instruction.h"
#include <string.h>

IRInstruction ir_inst_create(IROpcode opcode) {
    IRInstruction inst;
    memset(&inst, 0, sizeof(inst));
    inst.opcode = opcode;
    return inst;
}

IRInstruction ir_inst_create_1(IROpcode opcode, IRValue result, IRValue op1) {
    IRInstruction inst = ir_inst_create(opcode);
    inst.result = result;
    inst.operands[0] = op1;
    inst.operand_count = 1;
    return inst;
}

IRInstruction ir_inst_create_2(IROpcode opcode, IRValue result, IRValue op1, IRValue op2) {
    IRInstruction inst = ir_inst_create(opcode);
    inst.result = result;
    inst.operands[0] = op1;
    inst.operands[1] = op2;
    inst.operand_count = 2;
    return inst;
}

IRInstruction ir_inst_create_3(IROpcode opcode, IRValue result, IRValue op1, IRValue op2, IRValue op3) {
    IRInstruction inst = ir_inst_create(opcode);
    inst.result = result;
    inst.operands[0] = op1;
    inst.operands[1] = op2;
    inst.operands[2] = op3;
    inst.operand_count = 3;
    return inst;
}

IRInstruction ir_inst_nop(void) { return ir_inst_create(IR_OP_NOP); }

IRInstruction ir_inst_ret(IRValue val) {
    return ir_inst_create_1(IR_OP_RET, ir_value_null(), val);
}

IRInstruction ir_inst_jmp(const char* label) {
    IRInstruction inst = ir_inst_create(IR_OP_JMP);
    inst.operands[0] = ir_value_create_label(label);
    inst.operand_count = 1;
    return inst;
}

IRInstruction ir_inst_cond_jmp(IRCompareOp cmp, IRValue left, IRValue right, const char* label_true, const char* label_false) {
    IROpcode opcode;
    switch (cmp) {
        case IR_CMP_EQ: opcode = IR_OP_JE; break;
        case IR_CMP_NE: opcode = IR_OP_JNE; break;
        case IR_CMP_GT: opcode = IR_OP_JG; break;
        case IR_CMP_LT: opcode = IR_OP_JL; break;
        case IR_CMP_GE: opcode = IR_OP_JGE; break;
        case IR_CMP_LE: opcode = IR_OP_JLE; break;
        default: opcode = IR_OP_JE; break;
    }
    IRInstruction inst = ir_inst_create(opcode);
    inst.compare_op = cmp;
    inst.operands[0] = left;
    inst.operands[1] = right;
    inst.operands[2] = ir_value_create_label(label_true);
    inst.operands[3] = ir_value_create_label(label_false);
    inst.operand_count = 4;
    return inst;
}

IRInstruction ir_inst_call(IRValue result, const char* func_name, IRValue* args, int arg_count) {
    IRInstruction inst = ir_inst_create(IR_OP_CALL);
    inst.result = result;
    inst.operands[0] = ir_value_create_string(func_name);
    inst.operand_count = 1;
    for (int i = 0; i < arg_count && i < 7; i++) {
        inst.operands[inst.operand_count++] = args[i];
    }
    return inst;
}

IRInstruction ir_inst_phi(IRValue result, IRValue* incomings, int count) {
    IRInstruction inst = ir_inst_create(IR_OP_PHI);
    inst.result = result;
    inst.operand_count = 0;
    for (int i = 0; i < count && i < 8; i++) {
        inst.operands[inst.operand_count++] = incomings[i];
    }
    return inst;
}

const char* ir_opcode_name(IROpcode op) {
    switch (op) {
        case IR_OP_NOP: return "nop";
        case IR_OP_LOAD: return "load";
        case IR_OP_STORE: return "store";
        case IR_OP_MOV: return "mov";
        case IR_OP_ADD: return "add";
        case IR_OP_SUB: return "sub";
        case IR_OP_MUL: return "mul";
        case IR_OP_DIV: return "div";
        case IR_OP_MOD: return "mod";
        case IR_OP_NEG: return "neg";
        case IR_OP_NOT: return "not";
        case IR_OP_AND: return "and";
        case IR_OP_OR: return "or";
        case IR_OP_XOR: return "xor";
        case IR_OP_SHL: return "shl";
        case IR_OP_SHR: return "shr";
        case IR_OP_CMP: return "cmp";
        case IR_OP_JMP: return "jmp";
        case IR_OP_JE: return "je";
        case IR_OP_JNE: return "jne";
        case IR_OP_JG: return "jg";
        case IR_OP_JL: return "jl";
        case IR_OP_JGE: return "jge";
        case IR_OP_JLE: return "jle";
        case IR_OP_CALL: return "call";
        case IR_OP_RET: return "ret";
        case IR_OP_PUSH: return "push";
        case IR_OP_POP: return "pop";
        case IR_OP_PHI: return "phi";
        case IR_OP_CAST: return "cast";
        case IR_OP_ALLOCA: return "alloca";
        case IR_OP_LOAD_ELEMENT: return "load_elem";
        case IR_OP_STORE_ELEMENT: return "store_elem";
        case IR_OP_MEMBER_ACCESS: return "member";
        case IR_OP_LEA: return "lea";
    }
    return "???";
}

const char* ir_cmp_name(IRCompareOp cmp) {
    switch (cmp) {
        case IR_CMP_EQ: return "eq";
        case IR_CMP_NE: return "ne";
        case IR_CMP_GT: return "gt";
        case IR_CMP_LT: return "lt";
        case IR_CMP_GE: return "ge";
        case IR_CMP_LE: return "le";
    }
    return "???";
}