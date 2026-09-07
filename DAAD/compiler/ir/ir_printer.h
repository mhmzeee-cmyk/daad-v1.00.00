/**
 * @file ir_printer.h
 * @brief طباعة IR
 */

#ifndef DAAD_IR_PRINTER_H
#define DAAD_IR_PRINTER_H

#include "ir_module.h"
#include <stdio.h>

void ir_print_module(IRModule* module, FILE* out);
void ir_print_function(IRFunction* func, FILE* out);
void ir_print_block(IRBasicBlock* bb, FILE* out);
void ir_print_instruction(IRInstruction* inst, FILE* out);
void ir_print_value(IRValue val, FILE* out);
void ir_print_type(IRType type, FILE* out);

#endif