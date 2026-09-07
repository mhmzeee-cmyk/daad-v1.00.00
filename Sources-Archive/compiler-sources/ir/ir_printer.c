/**
 * @file ir_printer.c
 * @brief تنفيذ طباعة IR
 */

#include "ir_printer.h"
#include <stdio.h>

void ir_print_type(IRType type, FILE* out) {
    if (!out) return;
    switch (type.kind) {
        case IR_TYPE_VOID: fprintf(out, "void"); break;
        case IR_TYPE_I8: fprintf(out, "i8"); break;
        case IR_TYPE_I16: fprintf(out, "i16"); break;
        case IR_TYPE_I32: fprintf(out, "i32"); break;
        case IR_TYPE_I64: fprintf(out, "i64"); break;
        case IR_TYPE_F32: fprintf(out, "f32"); break;
        case IR_TYPE_F64: fprintf(out, "f64"); break;
        case IR_TYPE_PTR: fprintf(out, "ptr"); break;
        case IR_TYPE_LABEL: fprintf(out, "label"); break;
    }
}

void ir_print_value(IRValue val, FILE* out) {
    if (!out) return;
    switch (val.kind) {
        case IR_VALUE_REGISTER:
            fprintf(out, "r%d", val.id);
            break;
        case IR_VALUE_CONSTANT_INT:
            fprintf(out, "%lld", val.as.int_val);
            break;
        case IR_VALUE_CONSTANT_FLOAT:
            fprintf(out, "%f", val.as.float_val);
            break;
        case IR_VALUE_CONSTANT_STRING:
            fprintf(out, "\"");
            if (val.as.string_val) {
                for (const char* p = val.as.string_val; *p; p++) {
                    switch (*p) {
                        case '\\': fprintf(out, "\\\\"); break;
                        case '"': fprintf(out, "\\\""); break;
                        case '\n': fprintf(out, "\\n"); break;
                        case '\r': fprintf(out, "\\r"); break;
                        case '\t': fprintf(out, "\\t"); break;
                        default: fprintf(out, "%c", *p); break;
                    }
                }
            }
            fprintf(out, "\"");
            break;
        case IR_VALUE_LABEL:
            fprintf(out, "%s", val.as.label_val ? val.as.label_val : "label");
            break;
        case IR_VALUE_NULL:
            fprintf(out, "null");
            break;
    }
}

void ir_print_instruction(IRInstruction* inst, FILE* out) {
    if (!inst || !out) return;

    if (inst->opcode == IR_OP_PHI) {
        fprintf(out, "  ");
        if (inst->result.kind != IR_VALUE_NULL) {
            ir_print_value(inst->result, out);
            fprintf(out, " = ");
        }
        fprintf(out, "phi ");
        for (int i = 0; i < inst->operand_count; i++) {
            if (i > 0) fprintf(out, ", ");
            ir_print_value(inst->operands[i], out);
        }
        fprintf(out, "\n");
        return;
    }

    if (inst->opcode == IR_OP_ALLOCA) {
        fprintf(out, "  ");
        ir_print_value(inst->result, out);
        fprintf(out, " = alloca ");
        ir_print_value(inst->operands[0], out);
        fprintf(out, "\n");
        return;
    }

    if (inst->opcode == IR_OP_RET) {
        fprintf(out, "  ret ");
        if (inst->operand_count > 0) ir_print_value(inst->operands[0], out);
        fprintf(out, "\n");
        return;
    }

    if (inst->opcode == IR_OP_JMP) {
        fprintf(out, "  jmp ");
        if (inst->operand_count > 0) ir_print_value(inst->operands[0], out);
        fprintf(out, "\n");
        return;
    }

    if (inst->opcode == IR_OP_JE || inst->opcode == IR_OP_JNE ||
        inst->opcode == IR_OP_JG || inst->opcode == IR_OP_JL ||
        inst->opcode == IR_OP_JGE || inst->opcode == IR_OP_JLE) {
        fprintf(out, "  %s ", ir_opcode_name(inst->opcode));
        if (inst->operand_count >= 2) {
            ir_print_value(inst->operands[0], out);
            fprintf(out, ", ");
            ir_print_value(inst->operands[1], out);
            fprintf(out, ", ");
        }
        if (inst->operand_count >= 3) ir_print_value(inst->operands[2], out);
        if (inst->operand_count >= 4) {
            fprintf(out, ", ");
            ir_print_value(inst->operands[3], out);
        }
        fprintf(out, "\n");
        return;
    }

    if (inst->opcode == IR_OP_CALL) {
        fprintf(out, "  ");
        if (inst->result.kind != IR_VALUE_NULL) {
            ir_print_value(inst->result, out);
            fprintf(out, " = ");
        }
        fprintf(out, "call ");
        if (inst->operand_count > 0) ir_print_value(inst->operands[0], out);
        fprintf(out, "(");
        for (int i = 1; i < inst->operand_count; i++) {
            if (i > 1) fprintf(out, ", ");
            ir_print_value(inst->operands[i], out);
        }
        fprintf(out, ")\n");
        return;
    }

    if (inst->opcode == IR_OP_STORE) {
        fprintf(out, "  store ");
        if (inst->operand_count >= 2) {
            ir_print_value(inst->operands[1], out);
            fprintf(out, ", ");
            ir_print_value(inst->operands[0], out);
        }
        fprintf(out, "\n");
        return;
    }

    fprintf(out, "  ");
    if (inst->result.kind != IR_VALUE_NULL) {
        ir_print_value(inst->result, out);
        fprintf(out, " = ");
    }
    fprintf(out, "%s", ir_opcode_name(inst->opcode));
    for (int i = 0; i < inst->operand_count; i++) {
        fprintf(out, i == 0 ? " " : ", ");
        ir_print_value(inst->operands[i], out);
    }
    fprintf(out, "\n");
}

void ir_print_block(IRBasicBlock* bb, FILE* out) {
    if (!bb || !out) return;
    fprintf(out, "%s:\n", bb->label ? bb->label : "bb?");
    for (int i = 0; i < bb->instruction_count; i++) {
        ir_print_instruction(&bb->instructions[i], out);
    }
}

void ir_print_function(IRFunction* func, FILE* out) {
    if (!func || !out) return;
    fprintf(out, "function @%s(", func->name);
    for (int i = 0; i < func->param_count; i++) {
        if (i > 0) fprintf(out, ", ");
        ir_print_type(func->param_types[i], out);
        fprintf(out, " ");
        ir_print_value(func->params[i], out);
    }
    fprintf(out, ") -> ");
    ir_print_type(func->return_type, out);
    fprintf(out, " {\n");
    for (int i = 0; i < func->block_count; i++) {
        ir_print_block(func->blocks[i], out);
        fprintf(out, "\n");
    }
    fprintf(out, "}\n");
}

void ir_print_module(IRModule* module, FILE* out) {
    if (!module || !out) return;
    fprintf(out, "; Module: %s\n\n", module->name);
    for (int i = 0; i < module->global_count; i++) {
        fprintf(out, "@%s = ", module->globals[i].name);
        if (module->globals[i].is_constant) fprintf(out, "constant ");
        else fprintf(out, "global ");
        ir_print_type(module->globals[i].type, out);
        fprintf(out, " ");
        ir_print_value(module->globals[i].init_val, out);
        fprintf(out, "\n");
    }
    if (module->global_count > 0) fprintf(out, "\n");
    for (int i = 0; i < module->function_count; i++) {
        ir_print_function(module->functions[i], out);
        fprintf(out, "\n");
    }
}