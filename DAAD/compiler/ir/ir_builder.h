/**
 * @file ir_builder.h
 * @brief بناء IR من AST
 */

#ifndef DAAD_IR_BUILDER_H
#define DAAD_IR_BUILDER_H

#include "ir_module.h"
#include "ir_function.h"
#include "ir_basic_block.h"
#include "../ast/ast_node.h"
#include "../semantic/semantic_visitor.h"

#define IR_VAR_HASH_SIZE 128
#define IR_LOOP_STACK_SIZE 32
#define IR_MAX_STRUCT_FIELDS 32

typedef struct {
    char* var_name;
    char* field_names[IR_MAX_STRUCT_FIELDS];
    int field_count;
} IRStructFields;

typedef struct IRVarEntry {
    char* name;
    IRValue value;
    IRType var_type;
    struct IRVarEntry* next;
} IRVarEntry;

typedef struct {
    const char* break_label;
    const char* continue_label;
} IRLoopContext;

typedef struct {
    IRModule* module;
    IRFunction* current_function;
    IRBasicBlock* current_block;
    IRVarEntry* var_hash[IR_VAR_HASH_SIZE];
    IRValue* local_vars;
    int* local_var_ids;
    char** local_var_names;
    int local_var_count;
    int local_var_capacity;
    IRLoopContext loop_stack[IR_LOOP_STACK_SIZE];
    int loop_depth;
    SemanticContext* ctx;
    IRStructFields struct_fields[64];
    int struct_field_count;
    ASTNode* program_root;
} IRBuilder;

IRBuilder* ir_builder_create(const char* module_name, SemanticContext* ctx);
void ir_builder_destroy(IRBuilder* builder);
IRModule* ir_builder_build(IRBuilder* builder, ASTNode* root);
IRFunction* ir_builder_build_function(IRBuilder* builder, ASTNode* func_node);
IRValue ir_builder_build_expr(IRBuilder* builder, ASTNode* expr);
void ir_builder_build_stmt(IRBuilder* builder, ASTNode* stmt);
IRValue ir_builder_lookup_var(IRBuilder* builder, const char* name);
IRType ir_builder_lookup_var_type(IRBuilder* builder, const char* name);
void ir_builder_add_var(IRBuilder* builder, const char* name, IRValue val, IRType var_type);

#endif