/**
 * @file ir_visitor.c
 * @brief تنفيذ زائر IR
 */

#include "ir_visitor.h"
#include <stddef.h>

IRVisitor ir_visitor_create(void) {
    IRVisitor v;
    v.visit_module = NULL;
    v.visit_function = NULL;
    v.visit_block = NULL;
    v.visit_instruction = NULL;
    return v;
}

void ir_visit_module(IRModule* module, IRVisitor* visitor, void* data) {
    if (!module || !visitor) return;
    if (visitor->visit_module) visitor->visit_module(module, data);
    for (int i = 0; i < module->function_count; i++) {
        ir_visit_function(module->functions[i], visitor, data);
    }
}

void ir_visit_function(IRFunction* func, IRVisitor* visitor, void* data) {
    if (!func || !visitor) return;
    if (visitor->visit_function) visitor->visit_function(func, data);
    for (int i = 0; i < func->block_count; i++) {
        IRBasicBlock* bb = func->blocks[i];
        if (visitor->visit_block) visitor->visit_block(bb, data);
        for (int j = 0; j < bb->instruction_count; j++) {
            if (visitor->visit_instruction) visitor->visit_instruction(&bb->instructions[j], data);
        }
    }
}