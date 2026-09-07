/**
 * @file ir_visitor.h
 * @brief زائر IR
 */

#ifndef DAAD_IR_VISITOR_H
#define DAAD_IR_VISITOR_H

#include "ir_module.h"

typedef struct {
    void (*visit_module)(IRModule* module, void* data);
    void (*visit_function)(IRFunction* func, void* data);
    void (*visit_block)(IRBasicBlock* bb, void* data);
    void (*visit_instruction)(IRInstruction* inst, void* data);
} IRVisitor;

IRVisitor ir_visitor_create(void);
void ir_visit_module(IRModule* module, IRVisitor* visitor, void* data);
void ir_visit_function(IRFunction* func, IRVisitor* visitor, void* data);

#endif