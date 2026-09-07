/**
 * @file ir_builder.c
 * @brief تنفيذ بناء IR من AST
 */

#include "ir_builder.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static unsigned int ir_var_hash(const char* name) {
    unsigned int hash = 5381;
    while (*name) {
        hash = ((hash << 5) + hash) + (unsigned char)*name;
        name++;
    }
    return hash % IR_VAR_HASH_SIZE;
}

IRBuilder* ir_builder_create(const char* module_name, SemanticContext* ctx) {
    IRBuilder* builder = (IRBuilder*)calloc(1, sizeof(IRBuilder));
    if (!builder) return NULL;
    builder->module = ir_module_create(module_name);
    builder->ctx = ctx;
    builder->local_var_capacity = 64;
    builder->local_vars = (IRValue*)calloc(builder->local_var_capacity, sizeof(IRValue));
    builder->local_var_ids = (int*)calloc(builder->local_var_capacity, sizeof(int));
    builder->local_var_names = (char**)calloc(builder->local_var_capacity, sizeof(char*));
    return builder;
}

void ir_builder_destroy(IRBuilder* builder) {
    if (!builder) return;
    for (int i = 0; i < IR_VAR_HASH_SIZE; i++) {
        IRVarEntry* entry = builder->var_hash[i];
        while (entry) {
            IRVarEntry* next = entry->next;
            free(entry->name);
            free(entry);
            entry = next;
        }
    }
    for (int i = 0; i < builder->local_var_count; i++) {
        free(builder->local_var_names[i]);
    }
    free(builder->local_vars);
    free(builder->local_var_ids);
    free(builder->local_var_names);
    free(builder);
}

void ir_builder_add_var(IRBuilder* builder, const char* name, IRValue val, IRType var_type) {
    if (!builder || !name) return;
    if (builder->local_var_count >= builder->local_var_capacity) {
        int new_cap = builder->local_var_capacity * 2;
        IRValue* new_vars = (IRValue*)malloc(new_cap * sizeof(IRValue));
        int* new_ids = (int*)malloc(new_cap * sizeof(int));
        char** new_names = (char**)malloc(new_cap * sizeof(char*));
        if (!new_vars || !new_ids || !new_names) {
            free(new_vars);
            free(new_ids);
            free(new_names);
            return;
        }
        memcpy(new_vars, builder->local_vars, builder->local_var_count * sizeof(IRValue));
        memcpy(new_ids, builder->local_var_ids, builder->local_var_count * sizeof(int));
        memcpy(new_names, builder->local_var_names, builder->local_var_count * sizeof(char*));
        free(builder->local_vars);
        free(builder->local_var_ids);
        free(builder->local_var_names);
        builder->local_vars = new_vars;
        builder->local_var_ids = new_ids;
        builder->local_var_names = new_names;
        builder->local_var_capacity = new_cap;
    }
    int slot = builder->local_var_count++;
    builder->local_vars[slot] = val;
    builder->local_var_ids[slot] = slot;
    builder->local_var_names[slot] = name ? strdup(name) : NULL;

    unsigned int h = ir_var_hash(name);
    IRVarEntry* existing = builder->var_hash[h];
    while (existing) {
        if (existing->name && strcmp(existing->name, name) == 0) {
            existing->value = val;
            existing->var_type = var_type;
            return;
        }
        existing = existing->next;
    }
    IRVarEntry* entry = (IRVarEntry*)malloc(sizeof(IRVarEntry));
    if (entry) {
        entry->name = name ? strdup(name) : NULL;
        entry->value = val;
        entry->var_type = var_type;
        entry->next = builder->var_hash[h];
        builder->var_hash[h] = entry;
    }
}

IRType ir_builder_lookup_var_type(IRBuilder* builder, const char* name) {
    if (!builder || !name) return ir_type_i64();
    unsigned int h = ir_var_hash(name);
    IRVarEntry* entry = builder->var_hash[h];
    while (entry) {
        if (entry->name && strcmp(entry->name, name) == 0) {
            return entry->var_type;
        }
        entry = entry->next;
    }
    return ir_type_i64();
}

IRValue ir_builder_lookup_var(IRBuilder* builder, const char* name) {
    if (!builder || !name) return ir_value_null();
    unsigned int h = ir_var_hash(name);
    IRVarEntry* entry = builder->var_hash[h];
    while (entry) {
        if (entry->name && strcmp(entry->name, name) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }
    return ir_value_null();
}

static IRType ast_type_to_ir(ASTNode* type_node) {
    if (!type_node) return ir_type_i64();
    if (type_node->type == NODE_IDENTIFIER) {
        const char* name = type_node->as.identifier.name;
        if (strcmp(name, "رقم") == 0) return ir_type_i64();
        if (strcmp(name, "رقم_عشري") == 0) return ir_type_f64();
        if (strcmp(name, "حرف") == 0) return ir_type_i8();
        if (strcmp(name, "منطق") == 0) return ir_type_i8();
        if (strcmp(name, "فراغ") == 0) return ir_type_void();
        if (strcmp(name, "نص") == 0) return ir_type_ptr();
    }
    if (type_node->type == NODE_ARRAY_TYPE) {
        return ir_type_ptr();
    }
    if (type_node->type == NODE_STRUCT_TYPE) {
        return ir_type_ptr();
    }
    return ir_type_i64();
}

IRValue ir_builder_build_expr(IRBuilder* builder, ASTNode* expr) {
    if (!builder || !expr || !builder->current_block) return ir_value_null();

    switch (expr->type) {
        case NODE_LITERAL: {
            IRType type = ir_type_i64();
            switch (expr->as.literal.kind) {
                case LITERAL_INT:
                    return ir_value_create_int(expr->as.literal.as.int_value, type);
                case LITERAL_FLOAT:
                    type = ir_type_f64();
                    return ir_value_create_float(expr->as.literal.as.float_value, type);
                case LITERAL_BOOL:
                    type = ir_type_i8();
                    return ir_value_create_int(expr->as.literal.as.bool_value ? 1 : 0, type);
                case LITERAL_CHAR:
                    type = ir_type_i8();
                    return ir_value_create_int(expr->as.literal.as.char_value, type);
                case LITERAL_STRING: {
                    const char* label = ir_module_register_string(builder->module, expr->as.literal.as.string_value);
                    if (label) {
                        return ir_value_create_string_data(label, expr->as.literal.as.string_value);
                    }
                    return ir_value_create_string(expr->as.literal.as.string_value);
                }
            }
            break;
        }
        case NODE_IDENTIFIER: {
            IRValue found = ir_builder_lookup_var(builder, expr->as.identifier.name);
            if (found.kind != IR_VALUE_NULL) {
                IRType var_type = ir_builder_lookup_var_type(builder, expr->as.identifier.name);
                IRValue reg = ir_function_alloc_reg(builder->current_function, var_type);
                IRInstruction inst = ir_inst_create_1(IR_OP_LOAD, reg, found);
                ir_bb_add_instruction(builder->current_block, inst);
                return reg;
            }
            IRValue reg = ir_function_alloc_reg(builder->current_function, ir_type_i64());
            IRInstruction inst = ir_inst_create_1(IR_OP_LOAD, reg, ir_value_create_string(expr->as.identifier.name));
            ir_bb_add_instruction(builder->current_block, inst);
            return reg;
        }
        case NODE_BINARY_EXPRESSION: {
            IRValue left = ir_builder_build_expr(builder, expr->as.binary.left);
            IRValue right = ir_builder_build_expr(builder, expr->as.binary.right);
            IRType result_type = ir_type_i64();
            if (left.type.kind == IR_TYPE_F64 || right.type.kind == IR_TYPE_F64)
                result_type = ir_type_f64();
            IRValue result = ir_function_alloc_reg(builder->current_function, result_type);
            IROpcode op;
            IRCompareOp cmp_op = IR_CMP_EQ;
            switch (expr->as.binary.op) {
                case OP_ADD: op = IR_OP_ADD; break;
                case OP_SUB: op = IR_OP_SUB; break;
                case OP_MUL: op = IR_OP_MUL; break;
                case OP_DIV: op = IR_OP_DIV; break;
                case OP_MOD: op = IR_OP_MOD; break;
                case OP_AND: op = IR_OP_AND; break;
                case OP_OR: op = IR_OP_OR; break;
                case OP_BITWISE_AND: op = IR_OP_AND; break;
                case OP_BITWISE_OR: op = IR_OP_OR; break;
                case OP_BITWISE_XOR: op = IR_OP_XOR; break;
                case OP_SHIFT_LEFT: op = IR_OP_SHL; break;
                case OP_SHIFT_RIGHT: op = IR_OP_SHR; break;
                case OP_EQUAL: op = IR_OP_CMP; cmp_op = IR_CMP_EQ; break;
                case OP_NOT_EQUAL: op = IR_OP_CMP; cmp_op = IR_CMP_NE; break;
                case OP_GREATER: op = IR_OP_CMP; cmp_op = IR_CMP_GT; break;
                case OP_LESS: op = IR_OP_CMP; cmp_op = IR_CMP_LT; break;
                case OP_GREATER_EQUAL: op = IR_OP_CMP; cmp_op = IR_CMP_GE; break;
                case OP_LESS_EQUAL: op = IR_OP_CMP; cmp_op = IR_CMP_LE; break;
                default: op = IR_OP_ADD; break;
            }
            IRInstruction inst = ir_inst_create_2(op, result, left, right);
            inst.compare_op = cmp_op;
            ir_bb_add_instruction(builder->current_block, inst);
            return result;
        }
        case NODE_UNARY_EXPRESSION: {
            IRValue operand = ir_builder_build_expr(builder, expr->as.unary.operand);
            /* Phase 12.5: &expr (address-of) and *expr (dereference) */
            if (expr->as.unary.op == UNARY_ADDRESS_OF) {
                /* &var: operand is already an address (alloca), return it directly */
                return operand;
            }
            if (expr->as.unary.op == UNARY_DEREF) {
                /* *ptr: dereference via LOAD */
                IRValue result = ir_function_alloc_reg(builder->current_function, operand.type);
                IRInstruction inst = ir_inst_create_1(IR_OP_LOAD, result, operand);
                ir_bb_add_instruction(builder->current_block, inst);
                return result;
            }
            IRValue result = ir_function_alloc_reg(builder->current_function, operand.type);
            IROpcode op;
            switch (expr->as.unary.op) {
                case UNARY_NEGATE: op = IR_OP_NEG; break;
                case UNARY_NOT: op = IR_OP_NOT; break;
                case UNARY_BITWISE_NOT: op = IR_OP_NOT; break;
                default: op = IR_OP_NEG; break;
            }
            IRInstruction inst = ir_inst_create_1(op, result, operand);
            ir_bb_add_instruction(builder->current_block, inst);
            return result;
        }
        case NODE_CALL_EXPRESSION: {
            const char* func_name = "";
            if (expr->as.call.callee && expr->as.call.callee->type == NODE_IDENTIFIER) {
                func_name = expr->as.call.callee->as.identifier.name;
            }
            IRValue args[8];
            int arg_count = 0;
            for (size_t i = 0; i < expr->as.call.args.size && i < 8; i++) {
                args[arg_count++] = ir_builder_build_expr(builder, expr->as.call.args.items[i]);
            }
            IRValue result = ir_function_alloc_reg(builder->current_function, ir_type_i64());
            IRInstruction inst = ir_inst_call(result, func_name, args, arg_count);
            ir_bb_add_instruction(builder->current_block, inst);
            return result;
        }
        case NODE_INDEX_EXPRESSION: {
            IRValue array = ir_builder_build_expr(builder, expr->as.index.array);
            IRValue index = ir_builder_build_expr(builder, expr->as.index.index);
            IRValue result = ir_function_alloc_reg(builder->current_function, ir_type_ptr());
            IRInstruction inst = ir_inst_create_2(IR_OP_LOAD_ELEMENT, result, array, index);
            ir_bb_add_instruction(builder->current_block, inst);
            IRValue loaded = ir_function_alloc_reg(builder->current_function, ir_type_i64());
            IRInstruction load = ir_inst_create_1(IR_OP_LOAD, loaded, result);
            ir_bb_add_instruction(builder->current_block, load);
            return loaded;
        }
        case NODE_MEMBER_EXPRESSION: {
            IRValue object = ir_builder_build_expr(builder, expr->as.member.object);
            int field_index = 0;
            const char* field_name = expr->as.member.member;
            ASTNode* obj_node = expr->as.member.object;
            if (obj_node->type == NODE_IDENTIFIER) {
                const char* var_name = obj_node->as.identifier.name;
                for (int s = 0; s < builder->struct_field_count; s++) {
                    if (builder->struct_fields[s].var_name &&
                        strcmp(builder->struct_fields[s].var_name, var_name) == 0) {
                        for (int f = 0; f < builder->struct_fields[s].field_count; f++) {
                            if (builder->struct_fields[s].field_names[f] &&
                                strcmp(builder->struct_fields[s].field_names[f], field_name) == 0) {
                                field_index = f;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
            IRValue offset = ir_value_create_int(field_index * 8, ir_type_i64());
            IRValue result = ir_function_alloc_reg(builder->current_function, ir_type_ptr());
            IRInstruction inst = ir_inst_create_2(IR_OP_LEA, result, object, offset);
            ir_bb_add_instruction(builder->current_block, inst);
            IRValue loaded = ir_function_alloc_reg(builder->current_function, ir_type_i64());
            IRInstruction load = ir_inst_create_1(IR_OP_LOAD, loaded, result);
            ir_bb_add_instruction(builder->current_block, load);
            return loaded;
        }
        case NODE_CAST_EXPRESSION: {
            return ir_builder_build_expr(builder, expr->as.cast.expr);
        }
        case NODE_STRUCT_EXPRESSION: {
            int field_count = expr->as.struct_expr.values.size;
            IRValue size_val = ir_value_create_int(field_count * 8, ir_type_i64());
            IRValue alloca_val = ir_function_alloc_reg(builder->current_function, ir_type_ptr());
            IRInstruction alloca_inst = ir_inst_create_1(IR_OP_ALLOCA, alloca_val, size_val);
            ir_bb_add_instruction(builder->current_block, alloca_inst);
            for (int i = 0; i < field_count; i++) {
                IRValue field_val = ir_builder_build_expr(builder, expr->as.struct_expr.values.items[i]);
                IRValue offset = ir_value_create_int(i * 8, ir_type_i64());
                IRValue addr = ir_function_alloc_reg(builder->current_function, ir_type_ptr());
                IRInstruction add = ir_inst_create_2(IR_OP_ADD, addr, alloca_val, offset);
                ir_bb_add_instruction(builder->current_block, add);
                IRInstruction store = ir_inst_create_2(IR_OP_STORE_ELEMENT, ir_value_null(), addr, field_val);
                ir_bb_add_instruction(builder->current_block, store);
            }
            return alloca_val;
        }
        case NODE_ARRAY_EXPRESSION: {
            int elem_count = expr->as.array.elements.size;
            IRValue size_val = ir_value_create_int(elem_count * 8, ir_type_i64());
            IRValue alloca_val = ir_function_alloc_reg(builder->current_function, ir_type_ptr());
            IRInstruction alloca_inst = ir_inst_create_1(IR_OP_ALLOCA, alloca_val, size_val);
            ir_bb_add_instruction(builder->current_block, alloca_inst);
            for (int i = 0; i < elem_count; i++) {
                IRValue elem_val = ir_builder_build_expr(builder, expr->as.array.elements.items[i]);
                IRValue offset = ir_value_create_int(i * 8, ir_type_i64());
                IRValue addr = ir_function_alloc_reg(builder->current_function, ir_type_ptr());
                IRInstruction add = ir_inst_create_2(IR_OP_ADD, addr, alloca_val, offset);
                ir_bb_add_instruction(builder->current_block, add);
                IRInstruction store = ir_inst_create_2(IR_OP_STORE_ELEMENT, ir_value_null(), addr, elem_val);
                ir_bb_add_instruction(builder->current_block, store);
            }
            return alloca_val;
        }
        default:
            break;
    }
    return ir_value_null();
}

void ir_builder_build_stmt(IRBuilder* builder, ASTNode* stmt) {
    if (!builder || !stmt || !builder->current_block) return;

    switch (stmt->type) {
        case NODE_VARIABLE_DECL: {
            IRType var_type = ast_type_to_ir(stmt->as.var_decl.type_node);
            IRValue alloca_val = ir_function_alloc_reg(builder->current_function, ir_type_ptr());
            IRInstruction alloca_inst = ir_inst_create_1(IR_OP_ALLOCA, alloca_val, ir_value_create_int(ir_type_size_bytes(var_type), ir_type_i64()));
            ir_bb_add_instruction(builder->current_block, alloca_inst);
            ir_function_add_alloca(builder->current_function, alloca_val);
            ir_builder_add_var(builder, stmt->as.var_decl.name, alloca_val, var_type);
            if (stmt->as.var_decl.init_expr) {
                IRValue init_val = ir_builder_build_expr(builder, stmt->as.var_decl.init_expr);
                IRInstruction store = ir_inst_create_2(IR_OP_STORE, ir_value_null(), alloca_val, init_val);
                ir_bb_add_instruction(builder->current_block, store);
                if (stmt->as.var_decl.init_expr->type == NODE_STRUCT_EXPRESSION &&
                    builder->struct_field_count < 64) {
                    ASTNode* se = stmt->as.var_decl.init_expr;
                    IRStructFields* sf = &builder->struct_fields[builder->struct_field_count];
                    sf->var_name = strdup(stmt->as.var_decl.name);
                    sf->field_count = se->as.struct_expr.names.size;
                    for (int i = 0; i < sf->field_count && i < IR_MAX_STRUCT_FIELDS; i++) {
                        ASTNode* name_node = se->as.struct_expr.names.items[i];
                        sf->field_names[i] = (name_node && name_node->type == NODE_IDENTIFIER)
                            ? strdup(name_node->as.identifier.name) : strdup("");
                    }
                    builder->struct_field_count++;
                }
            }
            break;
        }
        case NODE_RETURN_STATEMENT: {
            if (stmt->as.return_stmt.value) {
                IRValue val = ir_builder_build_expr(builder, stmt->as.return_stmt.value);
                IRInstruction inst = ir_inst_ret(val);
                ir_bb_add_instruction(builder->current_block, inst);
            } else {
                IRInstruction inst = ir_inst_ret(ir_value_create_int(0, ir_type_i64()));
                ir_bb_add_instruction(builder->current_block, inst);
            }
            break;
        }
        case NODE_IF_STATEMENT: {
            IRValue cond = ir_builder_build_expr(builder, stmt->as.if_stmt.condition);
            IRBasicBlock* then_bb = ir_function_add_block(builder->current_function, "if_then");
            IRBasicBlock* else_bb = stmt->as.if_stmt.else_block ?
                ir_function_add_block(builder->current_function, "if_else") : NULL;
            IRBasicBlock* merge_bb = ir_function_add_block(builder->current_function, "if_merge");

            IRInstruction cond_jmp = ir_inst_cond_jmp(IR_CMP_NE, cond, ir_value_create_int(0, ir_type_i8()), "if_then", else_bb ? "if_else" : "if_merge");
            ir_bb_add_instruction(builder->current_block, cond_jmp);

            builder->current_block = then_bb;
            if (stmt->as.if_stmt.then_block) {
                ASTNode* body = stmt->as.if_stmt.then_block;
                for (size_t i = 0; i < body->as.block.statements.size; i++) {
                    ir_builder_build_stmt(builder, body->as.block.statements.items[i]);
                }
            }
            if (!ir_bb_terminates(builder->current_block)) {
                IRInstruction jmp = ir_inst_jmp("if_merge");
                ir_bb_add_instruction(builder->current_block, jmp);
            }

            if (else_bb) {
                builder->current_block = else_bb;
                if (stmt->as.if_stmt.else_block) {
                    ASTNode* body = stmt->as.if_stmt.else_block;
                    for (size_t i = 0; i < body->as.block.statements.size; i++) {
                        ir_builder_build_stmt(builder, body->as.block.statements.items[i]);
                    }
                }
                if (!ir_bb_terminates(builder->current_block)) {
                    IRInstruction jmp = ir_inst_jmp("if_merge");
                    ir_bb_add_instruction(builder->current_block, jmp);
                }
            }

            builder->current_block = merge_bb;
            break;
        }
        case NODE_WHILE_STATEMENT: {
            IRBasicBlock* cond_bb = ir_function_add_block(builder->current_function, "while_cond");
            IRBasicBlock* body_bb = ir_function_add_block(builder->current_function, "while_body");
            IRBasicBlock* exit_bb = ir_function_add_block(builder->current_function, "while_exit");

            IRInstruction jmp = ir_inst_jmp("while_cond");
            ir_bb_add_instruction(builder->current_block, jmp);

            builder->current_block = cond_bb;
            IRValue cond = ir_builder_build_expr(builder, stmt->as.while_stmt.condition);
            IRInstruction cond_jmp = ir_inst_cond_jmp(IR_CMP_NE, cond, ir_value_create_int(0, ir_type_i8()), "while_body", "while_exit");
            ir_bb_add_instruction(builder->current_block, cond_jmp);

            builder->current_block = body_bb;
            int prev_loop_depth = builder->loop_depth;
            if (prev_loop_depth < IR_LOOP_STACK_SIZE) {
                builder->loop_depth++;
                builder->loop_stack[prev_loop_depth].break_label = "while_exit";
                builder->loop_stack[prev_loop_depth].continue_label = "while_cond";
            }
            if (stmt->as.while_stmt.body) {
                ASTNode* body = stmt->as.while_stmt.body;
                for (size_t i = 0; i < body->as.block.statements.size; i++) {
                    ir_builder_build_stmt(builder, body->as.block.statements.items[i]);
                }
            }
            builder->loop_depth = prev_loop_depth;
            if (!ir_bb_terminates(builder->current_block)) {
                IRInstruction back = ir_inst_jmp("while_cond");
                ir_bb_add_instruction(builder->current_block, back);
            }

            builder->current_block = exit_bb;
            break;
        }
        case NODE_FOR_STATEMENT: {
            if (stmt->as.for_stmt.init) {
                ir_builder_build_stmt(builder, stmt->as.for_stmt.init);
            }
            IRBasicBlock* cond_bb = ir_function_add_block(builder->current_function, "for_cond");
            IRBasicBlock* body_bb = ir_function_add_block(builder->current_function, "for_body");
            IRBasicBlock* update_bb = ir_function_add_block(builder->current_function, "for_update");
            IRBasicBlock* exit_bb = ir_function_add_block(builder->current_function, "for_exit");

            IRInstruction jmp = ir_inst_jmp("for_cond");
            ir_bb_add_instruction(builder->current_block, jmp);

            builder->current_block = cond_bb;
            if (stmt->as.for_stmt.condition) {
                IRValue cond = ir_builder_build_expr(builder, stmt->as.for_stmt.condition);
                IRInstruction cond_jmp = ir_inst_cond_jmp(IR_CMP_NE, cond, ir_value_create_int(0, ir_type_i8()), "for_body", "for_exit");
                ir_bb_add_instruction(builder->current_block, cond_jmp);
            } else {
                IRInstruction jmp2 = ir_inst_jmp("for_body");
                ir_bb_add_instruction(builder->current_block, jmp2);
            }

            builder->current_block = body_bb;
            int prev_loop_depth = builder->loop_depth;
            if (prev_loop_depth < IR_LOOP_STACK_SIZE) {
                builder->loop_depth++;
                builder->loop_stack[prev_loop_depth].break_label = "for_exit";
                builder->loop_stack[prev_loop_depth].continue_label = "for_update";
            }
            if (stmt->as.for_stmt.body) {
                ASTNode* body = stmt->as.for_stmt.body;
                for (size_t i = 0; i < body->as.block.statements.size; i++) {
                    ir_builder_build_stmt(builder, body->as.block.statements.items[i]);
                }
            }
            builder->loop_depth = prev_loop_depth;
            if (!ir_bb_terminates(builder->current_block)) {
                IRInstruction jmp_to_update = ir_inst_jmp("for_update");
                ir_bb_add_instruction(builder->current_block, jmp_to_update);
            }

            builder->current_block = update_bb;
            if (stmt->as.for_stmt.update) {
                ir_builder_build_stmt(builder, stmt->as.for_stmt.update);
            }
            IRInstruction back = ir_inst_jmp("for_cond");
            ir_bb_add_instruction(builder->current_block, back);

            builder->current_block = exit_bb;
            break;
        }
        case NODE_ASSIGNMENT_EXPRESSION: {
            IRValue val = ir_builder_build_expr(builder, stmt->as.assignment.value);
            ASTNode* target_node = stmt->as.assignment.target;
            if (target_node->type == NODE_IDENTIFIER) {
                IRValue target = ir_builder_lookup_var(builder, target_node->as.identifier.name);
                if (target.kind != IR_VALUE_NULL) {
                    IRInstruction store = ir_inst_create_2(IR_OP_STORE, ir_value_null(), target, val);
                    ir_bb_add_instruction(builder->current_block, store);
                }
            } else if (target_node->type == NODE_INDEX_EXPRESSION) {
                IRValue base = ir_builder_build_expr(builder, target_node->as.index.array);
                IRValue index = ir_builder_build_expr(builder, target_node->as.index.index);
                IRValue offset = ir_function_alloc_reg(builder->current_function, ir_type_i64());
                IRInstruction mul = ir_inst_create_2(IR_OP_MUL, offset, index, ir_value_create_int(8, ir_type_i64()));
                ir_bb_add_instruction(builder->current_block, mul);
                IRValue addr = ir_function_alloc_reg(builder->current_function, ir_type_i64());
                IRInstruction add = ir_inst_create_2(IR_OP_ADD, addr, base, offset);
                ir_bb_add_instruction(builder->current_block, add);
                IRInstruction store = ir_inst_create_2(IR_OP_STORE_ELEMENT, ir_value_null(), addr, val);
                ir_bb_add_instruction(builder->current_block, store);
            } else if (target_node->type == NODE_MEMBER_EXPRESSION) {
                IRValue base = ir_builder_build_expr(builder, target_node->as.member.object);
                int field_index = 0;
                const char* field_name = target_node->as.member.member;
                ASTNode* obj_node = target_node->as.member.object;
                if (obj_node->type == NODE_IDENTIFIER) {
                    const char* var_name = obj_node->as.identifier.name;
                    for (int s = 0; s < builder->struct_field_count; s++) {
                        if (builder->struct_fields[s].var_name &&
                            strcmp(builder->struct_fields[s].var_name, var_name) == 0) {
                            for (int f = 0; f < builder->struct_fields[s].field_count; f++) {
                                if (builder->struct_fields[s].field_names[f] &&
                                    strcmp(builder->struct_fields[s].field_names[f], field_name) == 0) {
                                    field_index = f;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                }
                IRValue member_offset = ir_value_create_int(field_index * 8, ir_type_i64());
                IRValue addr = ir_function_alloc_reg(builder->current_function, ir_type_i64());
                IRInstruction lea = ir_inst_create_2(IR_OP_LEA, addr, base, member_offset);
                ir_bb_add_instruction(builder->current_block, lea);
                IRInstruction store = ir_inst_create_2(IR_OP_STORE_ELEMENT, ir_value_null(), addr, val);
                ir_bb_add_instruction(builder->current_block, store);
            }
            break;
        }
        case NODE_BLOCK: {
            for (size_t i = 0; i < stmt->as.block.statements.size; i++) {
                ir_builder_build_stmt(builder, stmt->as.block.statements.items[i]);
            }
            break;
        }
        case NODE_BREAK_STATEMENT:
            if (builder->loop_depth > 0) {
                IRInstruction jmp = ir_inst_jmp(builder->loop_stack[builder->loop_depth - 1].break_label);
                ir_bb_add_instruction(builder->current_block, jmp);
            }
            break;
        case NODE_CONTINUE_STATEMENT:
            if (builder->loop_depth > 0) {
                IRInstruction jmp = ir_inst_jmp(builder->loop_stack[builder->loop_depth - 1].continue_label);
                ir_bb_add_instruction(builder->current_block, jmp);
            }
            break;
        case NODE_GOTO_STATEMENT: {
            IRInstruction jmp = ir_inst_jmp(stmt->as.goto_stmt.label);
            ir_bb_add_instruction(builder->current_block, jmp);
            break;
        }
        case NODE_LABEL_STATEMENT: {
            IRBasicBlock* label_bb = ir_function_add_block(builder->current_function, stmt->as.label_stmt.name);
            if (!ir_bb_terminates(builder->current_block)) {
                IRInstruction jmp = ir_inst_jmp(stmt->as.label_stmt.name);
                ir_bb_add_instruction(builder->current_block, jmp);
            }
            builder->current_block = label_bb;
            break;
        }
        case NODE_STRUCT_TYPE:
        case NODE_STRUCT_FIELD:
            break;
        case NODE_CALL_EXPRESSION:
        case NODE_UNARY_EXPRESSION:
        case NODE_BINARY_EXPRESSION: {
            if (stmt->type == NODE_CALL_EXPRESSION &&
                stmt->as.call.callee &&
                stmt->as.call.callee->type == NODE_IDENTIFIER &&
                stmt->as.call.callee->as.identifier.name &&
                strcmp(stmt->as.call.callee->as.identifier.name, "ادخل") == 0 &&
                stmt->as.call.args.size >= 1) {
                ASTNode* arg_node = stmt->as.call.args.items[0];
                if (arg_node->type == NODE_IDENTIFIER) {
                    IRValue addr = ir_builder_lookup_var(builder, arg_node->as.identifier.name);
                    if (addr.kind != IR_VALUE_NULL) {
                        IRValue call_result = ir_function_alloc_reg(builder->current_function, ir_type_i64());
                        IRInstruction call_inst = ir_inst_call(call_result, "ادخل", NULL, 0);
                        ir_bb_add_instruction(builder->current_block, call_inst);
                        IRInstruction store = ir_inst_create_2(IR_OP_STORE, ir_value_null(), addr, call_result);
                        ir_bb_add_instruction(builder->current_block, store);
                    }
                }
            } else {
                ir_builder_build_expr(builder, stmt);
            }
            break;
        }
        default:
            break;
    }
}

static int type_size_bytes(ASTNode* type_node) {
    if (!type_node) return 8;
    if (type_node->type == NODE_IDENTIFIER) {
        const char* name = type_node->as.identifier.name;
        if (strcmp(name, "رقم") == 0) return 8;
        if (strcmp(name, "رقم_عشري") == 0) return 8;
        if (strcmp(name, "حرف") == 0) return 1;
        if (strcmp(name, "منطق") == 0) return 1;
    }
    if (type_node->type == NODE_ARRAY_TYPE) {
        int elem_size = type_size_bytes(type_node->as.array_type.element_type);
        if (type_node->as.array_type.size &&
            type_node->as.array_type.size->type == NODE_LITERAL &&
            type_node->as.array_type.size->as.literal.kind == LITERAL_INT) {
            return elem_size * (int)type_node->as.array_type.size->as.literal.as.int_value;
        }
        return elem_size * 10;
    }
    if (type_node->type == NODE_STRUCT_TYPE) {
        int total = 0;
        for (size_t i = 0; i < type_node->as.struct_type.fields.size; i++) {
            ASTNode* field = type_node->as.struct_type.fields.items[i];
            total += type_size_bytes(field->as.struct_field.type_node);
        }
        return total > 0 ? total : 8;
    }
    return 8;
}

IRFunction* ir_builder_build_function(IRBuilder* builder, ASTNode* func_node) {
    if (!builder || !func_node || func_node->type != NODE_FUNCTION_DECL) return NULL;

    IRType ret_type = ast_type_to_ir(func_node->as.func_decl.return_type);
    IRFunction* func = ir_module_add_function(builder->module, func_node->as.func_decl.name, ret_type);
    builder->current_function = func;

    for (size_t i = 0; i < func_node->as.func_decl.params.size; i++) {
        ASTNode* param = func_node->as.func_decl.params.items[i];
        if (param->type == NODE_PARAMETER) {
            ir_function_add_param(func, param->as.param.name, ast_type_to_ir(param->as.param.type_node));
        }
    }

    IRBasicBlock* entry = ir_function_add_block(func, "entry");
    builder->current_block = entry;

    for (int i = 0; i < func->param_count; i++) {
        IRValue alloca_val = ir_function_alloc_reg(func, ir_type_ptr());
        IRInstruction alloca_inst = ir_inst_create_1(IR_OP_ALLOCA, alloca_val, ir_value_create_int(type_size_bytes(NULL), ir_type_i64()));
        ir_bb_add_instruction(builder->current_block, alloca_inst);
        IRInstruction store = ir_inst_create_2(IR_OP_STORE, ir_value_null(), alloca_val, func->params[i]);
        ir_bb_add_instruction(builder->current_block, store);
        ASTNode* param = func_node->as.func_decl.params.items[i];
        if (param->type == NODE_PARAMETER) {
            IRType param_type = ast_type_to_ir(param->as.param.type_node);
            ir_builder_add_var(builder, param->as.param.name, alloca_val, param_type);
        }
    }

    if (func_node->as.func_decl.body) {
        ASTNode* body = func_node->as.func_decl.body;
        for (size_t i = 0; i < body->as.block.statements.size; i++) {
            ir_builder_build_stmt(builder, body->as.block.statements.items[i]);
        }
    }

    if (!ir_bb_terminates(builder->current_block)) {
        IRInstruction ret = ir_inst_ret(ir_value_create_int(0, ir_type_i64()));
        ir_bb_add_instruction(builder->current_block, ret);
    }

    return func;
}

static void ir_builder_build_top_level(IRBuilder* builder, ASTNode* root) {
    if (!builder || !root || root->type != NODE_PROGRAM) return;

    /* Check if there are any non-function declarations */
    int has_top_level = 0;
    for (size_t i = 0; i < root->as.program.declarations.size; i++) {
        ASTNode* decl = root->as.program.declarations.items[i];
        if (decl->type != NODE_FUNCTION_DECL) {
            has_top_level = 1;
            break;
        }
    }
    if (!has_top_level) return;

    /* Create synthetic main function wrapping all top-level statements */
    IRType ret_type = ir_type_i64();
    IRFunction* func = ir_module_add_function(builder->module, "الرئيسية", ret_type);
    builder->current_function = func;

    IRBasicBlock* entry = ir_function_add_block(func, "entry");
    builder->current_block = entry;

    for (size_t i = 0; i < root->as.program.declarations.size; i++) {
        ASTNode* decl = root->as.program.declarations.items[i];
        if (decl->type != NODE_FUNCTION_DECL) {
            ir_builder_build_stmt(builder, decl);
        }
    }

    if (!ir_bb_terminates(builder->current_block)) {
        IRInstruction ret = ir_inst_ret(ir_value_create_int(0, ir_type_i64()));
        ir_bb_add_instruction(builder->current_block, ret);
    }
}

IRModule* ir_builder_build(IRBuilder* builder, ASTNode* root) {
    if (!builder || !root) return NULL;

    if (root->type == NODE_PROGRAM) {
        /* First: build all explicit functions */
        for (size_t i = 0; i < root->as.program.declarations.size; i++) {
            ASTNode* decl = root->as.program.declarations.items[i];
            if (decl->type == NODE_FUNCTION_DECL) {
                ir_builder_build_function(builder, decl);
            }
        }

        /* Second: wrap top-level statements in synthetic main if needed */
        ir_builder_build_top_level(builder, root);
    }

    return builder->module;
}