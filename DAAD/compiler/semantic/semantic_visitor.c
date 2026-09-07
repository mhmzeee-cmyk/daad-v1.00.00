/**
 * @file semantic_visitor.c
 * @brief تنفيذ زائر التحليل الدلالي
 */

#include "semantic_visitor.h"
#include "constant_folding.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void visit_program(ASTNode* node, void* data);
static void visit_block(ASTNode* node, void* data);
static void visit_variable_decl(ASTNode* node, void* data);
static void visit_constant_decl(ASTNode* node, void* data);
static void visit_function_decl(ASTNode* node, void* data);
static void visit_parameter(ASTNode* node, void* data);
static void visit_return_statement(ASTNode* node, void* data);
static void visit_if_statement(ASTNode* node, void* data);
static void visit_while_statement(ASTNode* node, void* data);
static void visit_for_statement(ASTNode* node, void* data);
static void visit_break_statement(ASTNode* node, void* data);
static void visit_continue_statement(ASTNode* node, void* data);
static void visit_assignment_expression(ASTNode* node, void* data);
static void visit_binary_expression(ASTNode* node, void* data);
static void visit_unary_expression(ASTNode* node, void* data);
static void visit_literal(ASTNode* node, void* data);
static void visit_identifier(ASTNode* node, void* data);
static void visit_call_expression(ASTNode* node, void* data);
static void visit_array_expression(ASTNode* node, void* data);
static void visit_struct_expression(ASTNode* node, void* data);
static void visit_pointer_expression(ASTNode* node, void* data);
static void visit_member_expression(ASTNode* node, void* data);
static void visit_index_expression(ASTNode* node, void* data);
static void visit_cast_expression(ASTNode* node, void* data);

SemanticContext* semantic_context_create(void) {
    SemanticContext* ctx = (SemanticContext*)calloc(1, sizeof(SemanticContext));
    if (!ctx) return NULL;
    ctx->scopes = scope_stack_create();
    if (!ctx->scopes) { free(ctx); return NULL; }
    ctx->type_registry = type_registry_create();
    if (!ctx->type_registry) { scope_stack_destroy(ctx->scopes); free(ctx); return NULL; }
    ctx->errors = semantic_error_list_create();
    if (!ctx->errors) { type_registry_destroy(ctx->type_registry); scope_stack_destroy(ctx->scopes); free(ctx); return NULL; }
    ctx->type_checker = type_checker_create(ctx->type_registry, ctx->errors);
    if (!ctx->type_checker) { semantic_error_list_destroy(ctx->errors); type_registry_destroy(ctx->type_registry); scope_stack_destroy(ctx->scopes); free(ctx); return NULL; }
    ctx->in_loop = 0;
    ctx->current_return_type = NULL;
    return ctx;
}

void semantic_context_destroy(SemanticContext* ctx) {
    if (!ctx) return;
    type_checker_destroy(ctx->type_checker);
    semantic_error_list_destroy(ctx->errors);
    type_registry_destroy(ctx->type_registry);
    scope_stack_destroy(ctx->scopes);
    free(ctx);
}

ASTVisitor semantic_create_visitor(void) {
    ASTVisitor visitor = ast_visitor_create();
    visitor.visit_program = visit_program;
    visitor.visit_block = visit_block;
    visitor.visit_variable_decl = visit_variable_decl;
    visitor.visit_constant_decl = visit_constant_decl;
    visitor.visit_function_decl = visit_function_decl;
    visitor.visit_parameter = visit_parameter;
    visitor.visit_return_statement = visit_return_statement;
    visitor.visit_if_statement = visit_if_statement;
    visitor.visit_while_statement = visit_while_statement;
    visitor.visit_for_statement = visit_for_statement;
    visitor.visit_break_statement = visit_break_statement;
    visitor.visit_continue_statement = visit_continue_statement;
    visitor.visit_assignment_expression = visit_assignment_expression;
    visitor.visit_binary_expression = visit_binary_expression;
    visitor.visit_unary_expression = visit_unary_expression;
    visitor.visit_literal = visit_literal;
    visitor.visit_identifier = visit_identifier;
    visitor.visit_call_expression = visit_call_expression;
    visitor.visit_array_expression = visit_array_expression;
    visitor.visit_struct_expression = visit_struct_expression;
    visitor.visit_pointer_expression = visit_pointer_expression;
    visitor.visit_member_expression = visit_member_expression;
    visitor.visit_index_expression = visit_index_expression;
    visitor.visit_cast_expression = visit_cast_expression;
    return visitor;
}

static DaadType* resolve_type_node(ASTNode* type_node, SemanticContext* ctx) {
    if (!type_node) return ctx->type_registry->void_type;
    if (type_node->type == NODE_IDENTIFIER) {
        const char* name = type_node->as.identifier.name;
        if (strcmp(name, "رقم") == 0) return ctx->type_registry->int_type;
        if (strcmp(name, "رقم_عشري") == 0) return ctx->type_registry->float_type;
        if (strcmp(name, "حرف") == 0) return ctx->type_registry->char_type;
        if (strcmp(name, "منطق") == 0) return ctx->type_registry->bool_type;
        if (strcmp(name, "فراغ") == 0) return ctx->type_registry->void_type;
        if (strcmp(name, "نص") == 0) return ctx->type_registry->string_type;
        Symbol* sym = scope_stack_lookup(ctx->scopes, name);
        if (sym && sym->type) return sym->type;
        semantic_error_add(ctx->errors, S003_UNDEFINED_TYPE, type_node->start.line, type_node->start.column, type_node->filename, "النوع غير معرف: %s", name);
        return NULL;
    }
    if (type_node->type == NODE_ARRAY_TYPE) {
        DaadType* elem_type = resolve_type_node(type_node->as.array_type.element_type, ctx);
        if (elem_type) {
            int length = 0;
            if (type_node->as.array_type.size && type_node->as.array_type.size->type == NODE_LITERAL) {
                length = (int)type_node->as.array_type.size->as.literal.as.int_value;
            }
            return daad_type_create_array(elem_type, length);
        }
        return ctx->type_registry->int_type;
    }
    if (type_node->type == NODE_POINTER_EXPRESSION) {
        DaadType* pointee = resolve_type_node(type_node->as.pointer.type_node, ctx);
        if (pointee) return daad_type_create_pointer(pointee);
        return ctx->type_registry->int_type;
    }
    if (type_node->type == NODE_STRUCT_TYPE) {
        int field_count = type_node->as.struct_type.fields.size;
        DaadType** field_types = NULL;
        char** f_names = NULL;
        if (field_count > 0) {
            field_types = (DaadType**)malloc(field_count * sizeof(DaadType*));
            f_names = (char**)malloc(field_count * sizeof(char*));
        }
        for (int i = 0; i < field_count; i++) {
            ASTNode* field = type_node->as.struct_type.fields.items[i];
            f_names[i] = strdup(field->as.struct_field.name);
            field_types[i] = resolve_type_node(field->as.struct_field.type_node, ctx);
            if (!field_types[i]) field_types[i] = ctx->type_registry->int_type;
        }
        DaadType* struct_type = daad_type_create_struct(f_names, field_types, field_count);
        for (int i = 0; i < field_count; i++) {
            free(f_names[i]);
        }
        free(field_types);
        free(f_names);
        return struct_type;
    }
    semantic_error_add(ctx->errors, S003_UNDEFINED_TYPE, type_node->start.line, type_node->start.column, type_node->filename, "تعبير النوع غير صالح");
    return NULL;
}

static DaadType* infer_expression_type(ASTNode* expr, SemanticContext* ctx) {
    if (!expr) return ctx->type_registry->void_type;
    switch (expr->type) {
        case NODE_LITERAL:
            switch (expr->as.literal.kind) {
                case LITERAL_INT: return ctx->type_registry->int_type;
                case LITERAL_FLOAT: return ctx->type_registry->float_type;
                case LITERAL_BOOL: return ctx->type_registry->bool_type;
                case LITERAL_CHAR: return ctx->type_registry->char_type;
                case LITERAL_STRING: return ctx->type_registry->string_type;
            }
            break;
        case NODE_IDENTIFIER: {
            Symbol* sym = scope_stack_lookup(ctx->scopes, expr->as.identifier.name);
            if (sym && sym->type) return sym->type;
            break;
        }
        case NODE_BINARY_EXPRESSION: {
            DaadType* left = infer_expression_type(expr->as.binary.left, ctx);
            DaadType* right = infer_expression_type(expr->as.binary.right, ctx);
            if (left && right) {
                BinaryOperator op = expr->as.binary.op;
                if (op == OP_EQUAL || op == OP_NOT_EQUAL || op == OP_GREATER ||
                    op == OP_LESS || op == OP_GREATER_EQUAL || op == OP_LESS_EQUAL ||
                    op == OP_AND || op == OP_OR) {
                    return ctx->type_registry->bool_type;
                }
                if (left->kind == TYPE_FLOAT || right->kind == TYPE_FLOAT) {
                    return ctx->type_registry->float_type;
                }
                return left;
            }
            break;
        }
        case NODE_UNARY_EXPRESSION: {
            DaadType* operand = infer_expression_type(expr->as.unary.operand, ctx);
            if (operand) {
                if (expr->as.unary.op == UNARY_NOT) return ctx->type_registry->bool_type;
                if (expr->as.unary.op == UNARY_ADDRESS_OF)
                    return daad_type_create_pointer(operand);
                if (expr->as.unary.op == UNARY_DEREF) {
                    if (operand->kind == TYPE_POINTER)
                        return operand->as.pointer.pointee;
                }
                return operand;
            }
            break;
        }
        case NODE_CALL_EXPRESSION: {
            if (expr->as.call.callee && expr->as.call.callee->type == NODE_IDENTIFIER) {
                const char* func_name = expr->as.call.callee->as.identifier.name;
                Symbol* func_sym = scope_stack_lookup(ctx->scopes, func_name);
                if (func_sym && func_sym->type && func_sym->type->kind == TYPE_FUNCTION) {
                    return func_sym->type->as.function.return_type;
                }
            }
            return ctx->type_registry->int_type;
        }
        default:
            break;
    }
    return ctx->type_registry->int_type;
}

static void register_builtins(SemanticContext* ctx) {
    DaadType* void_type = ctx->type_registry->void_type;
    DaadType* int_type = ctx->type_registry->int_type;
    DaadType* str_type = ctx->type_registry->string_type;

    DaadType** print_params = (DaadType**)malloc(sizeof(DaadType*));
    print_params[0] = str_type;
    DaadType* print_type = daad_type_create_function(print_params, 1, void_type);
    Symbol* print_sym = symbol_create("اطبع", SYMBOL_FUNCTION, print_type, 0);
    print_sym->is_initialized = 1;
    print_sym->is_variadic = 1;
    scope_stack_define(ctx->scopes, print_sym);

    DaadType** input_params = (DaadType**)malloc(sizeof(DaadType*));
    input_params[0] = int_type;
    DaadType* input_type = daad_type_create_function(input_params, 1, int_type);
    Symbol* input_sym = symbol_create("ادخل", SYMBOL_FUNCTION, input_type, 0);
    input_sym->is_initialized = 1;
    scope_stack_define(ctx->scopes, input_sym);

    DaadType* exit_type = daad_type_create_function(input_params, 0, void_type);
    Symbol* exit_sym = symbol_create("توقف", SYMBOL_FUNCTION, exit_type, 0);
    exit_sym->is_initialized = 1;
    scope_stack_define(ctx->scopes, exit_sym);

    DaadType** open_params = (DaadType**)malloc(sizeof(DaadType*) * 2);
    open_params[0] = str_type;
    open_params[1] = str_type;
    DaadType* open_type = daad_type_create_function(open_params, 2, int_type);
    Symbol* open_sym = symbol_create("افتح", SYMBOL_FUNCTION, open_type, 0);
    open_sym->is_initialized = 1;
    scope_stack_define(ctx->scopes, open_sym);

    DaadType** read_file_params = (DaadType**)malloc(sizeof(DaadType*) * 2);
    read_file_params[0] = int_type;
    read_file_params[1] = str_type;
    DaadType* read_file_type = daad_type_create_function(read_file_params, 2, int_type);
    Symbol* read_file_sym = symbol_create("اقرأ_ملف", SYMBOL_FUNCTION, read_file_type, 0);
    read_file_sym->is_initialized = 1;
    scope_stack_define(ctx->scopes, read_file_sym);

    DaadType** write_file_params = (DaadType**)malloc(sizeof(DaadType*) * 2);
    write_file_params[0] = int_type;
    write_file_params[1] = str_type;
    DaadType* write_file_type = daad_type_create_function(write_file_params, 2, int_type);
    Symbol* write_file_sym = symbol_create("اكتب_ملف", SYMBOL_FUNCTION, write_file_type, 0);
    write_file_sym->is_initialized = 1;
    scope_stack_define(ctx->scopes, write_file_sym);

    DaadType** close_params = (DaadType**)malloc(sizeof(DaadType*));
    close_params[0] = int_type;
    DaadType* close_type = daad_type_create_function(close_params, 1, void_type);
    Symbol* close_sym = symbol_create("اغلق", SYMBOL_FUNCTION, close_type, 0);
    close_sym->is_initialized = 1;
    scope_stack_define(ctx->scopes, close_sym);

    DaadType** alloc_params = (DaadType**)malloc(sizeof(DaadType*));
    alloc_params[0] = int_type;
    DaadType* alloc_type = daad_type_create_function(alloc_params, 1, int_type);
    Symbol* alloc_sym = symbol_create("احجز", SYMBOL_FUNCTION, alloc_type, 0);
    alloc_sym->is_initialized = 1;
    scope_stack_define(ctx->scopes, alloc_sym);

    DaadType** free_params = (DaadType**)malloc(sizeof(DaadType*));
    free_params[0] = int_type;
    DaadType* free_type = daad_type_create_function(free_params, 1, void_type);
    Symbol* free_sym = symbol_create("حرر", SYMBOL_FUNCTION, free_type, 0);
    free_sym->is_initialized = 1;
    scope_stack_define(ctx->scopes, free_sym);

    DaadType** resize_params = (DaadType**)malloc(sizeof(DaadType*) * 2);
    resize_params[0] = int_type;
    resize_params[1] = int_type;
    DaadType* resize_type = daad_type_create_function(resize_params, 2, int_type);
    Symbol* resize_sym = symbol_create("غيّر_حجم", SYMBOL_FUNCTION, resize_type, 0);
    resize_sym->is_initialized = 1;
    scope_stack_define(ctx->scopes, resize_sym);
}

static void visit_program(ASTNode* node, void* data) {
    SemanticContext* ctx = (SemanticContext*)data;
    register_builtins(ctx);
    ASTVisitor vis = semantic_create_visitor();
    for (size_t i = 0; i < node->as.program.declarations.size; i++) {
        ast_visit(node->as.program.declarations.items[i], &vis, ctx);
    }
}

static void visit_block(ASTNode* node, void* data) {
    SemanticContext* ctx = (SemanticContext*)data;
    ASTVisitor vis = semantic_create_visitor();
    for (size_t i = 0; i < node->as.block.statements.size; i++) {
        ast_visit(node->as.block.statements.items[i], &vis, ctx);
    }
}

static void visit_variable_decl(ASTNode* node, void* data) {
    SemanticContext* ctx = (SemanticContext*)data;
    const char* name = node->as.var_decl.name;

    if (scope_stack_peek(ctx->scopes) && scope_lookup_current(scope_stack_peek(ctx->scopes), name)) {
        semantic_error_add(ctx->errors, S004_DUPLICATE_VARIABLE, node->start.line, node->start.column, node->filename, "المتغير معرّف مسبقاً: %s", name);
        return;
    }

    DaadType* var_type = resolve_type_node(node->as.var_decl.type_node, ctx);
    if (node->as.var_decl.init_expr) {
        ASTVisitor vis2 = semantic_create_visitor();
        ast_visit(node->as.var_decl.init_expr, &vis2, ctx);
        node->as.var_decl.init_expr = constant_fold(node->as.var_decl.init_expr);
        if (!node->as.var_decl.type_node || (var_type && var_type->kind == TYPE_VOID)) {
            DaadType* inferred = infer_expression_type(node->as.var_decl.init_expr, ctx);
            if (inferred && inferred->kind != TYPE_VOID) {
                var_type = inferred;
            }
        }
        Symbol* sym = symbol_create(name, SYMBOL_VARIABLE, var_type, ctx->scopes->current_level);
        if (!sym) return;
        sym->is_initialized = 1;
        scope_stack_define(ctx->scopes, sym);
    } else {
        Symbol* sym = symbol_create(name, SYMBOL_VARIABLE, var_type, ctx->scopes->current_level);
        if (!sym) return;
        scope_stack_define(ctx->scopes, sym);
    }
}

static void visit_constant_decl(ASTNode* node, void* data) {
    SemanticContext* ctx = (SemanticContext*)data;
    const char* name = node->as.const_decl.name;

    if (scope_stack_peek(ctx->scopes) && scope_lookup_current(scope_stack_peek(ctx->scopes), name)) {
        semantic_error_add(ctx->errors, S004_DUPLICATE_VARIABLE, node->start.line, node->start.column, node->filename, "الثابت معرّف مسبقاً: %s", name);
        return;
    }

    if (node->as.const_decl.value) {
        ASTVisitor vis3 = semantic_create_visitor();
        ast_visit(node->as.const_decl.value, &vis3, ctx);
        node->as.const_decl.value = constant_fold(node->as.const_decl.value);
    }

    DaadType* const_type = ctx->type_registry->int_type;
    if (node->as.const_decl.value) {
        const_type = infer_expression_type(node->as.const_decl.value, ctx);
    }
    Symbol* sym = symbol_create(name, SYMBOL_CONSTANT, const_type, ctx->scopes->current_level);
    if (!sym) return;
    sym->is_mutable = 0;
    sym->is_initialized = 1;
    scope_stack_define(ctx->scopes, sym);
}

static void visit_function_decl(ASTNode* node, void* data) {
    SemanticContext* ctx = (SemanticContext*)data;
    const char* name = node->as.func_decl.name;

    Symbol* existing = scope_stack_peek(ctx->scopes) ? scope_lookup_current(scope_stack_peek(ctx->scopes), name) : NULL;
    if (existing) {
        semantic_error_add(ctx->errors, S005_DUPLICATE_FUNCTION, node->start.line, node->start.column, node->filename, "الدالة معرّفة مسبقاً: %s", name);
        return;
    }

    DaadType* ret_type = resolve_type_node(node->as.func_decl.return_type, ctx);

    DaadType** param_types = NULL;
    int param_count = (int)node->as.func_decl.params.size;
    if (param_count > 0) {
        param_types = (DaadType**)malloc(sizeof(DaadType*) * param_count);
        if (!param_types) return;
        for (int i = 0; i < param_count; i++) {
            ASTNode* param = node->as.func_decl.params.items[i];
            if (param->type == NODE_PARAMETER) {
                param_types[i] = resolve_type_node(param->as.param.type_node, ctx);
            } else {
                param_types[i] = ctx->type_registry->int_type;
            }
        }
    }
    DaadType* func_type = daad_type_create_function(param_types, param_count, ret_type);

    Symbol* sym = symbol_create(name, SYMBOL_FUNCTION, func_type, ctx->scopes->current_level);
    if (!sym) return;
    sym->is_initialized = 1;
    scope_stack_define(ctx->scopes, sym);

    scope_stack_push(ctx->scopes);
    DaadType* prev_return = ctx->current_return_type;
    ctx->current_return_type = ret_type;

    ASTVisitor vis4 = semantic_create_visitor();
    for (size_t i = 0; i < node->as.func_decl.params.size; i++) {
        ast_visit(node->as.func_decl.params.items[i], &vis4, ctx);
    }

    if (node->as.func_decl.body) {
        ast_visit(node->as.func_decl.body, &vis4, ctx);
    }

    ctx->current_return_type = prev_return;
    scope_stack_pop(ctx->scopes);
}

static void visit_return_statement(ASTNode* node, void* data) {
    SemanticContext* ctx = (SemanticContext*)data;
    if (node->as.return_stmt.value) {
        ASTVisitor vis5 = semantic_create_visitor();
        ast_visit(node->as.return_stmt.value, &vis5, ctx);
        DaadType* actual = infer_expression_type(node->as.return_stmt.value, ctx);
        type_checker_check_return(ctx->type_checker, ctx->current_return_type, actual, node->start.line, node->start.column, node->filename);
    }
}

static void visit_if_statement(ASTNode* node, void* data) {
    SemanticContext* ctx = (SemanticContext*)data;
    ASTVisitor vis6 = semantic_create_visitor();
    if (node->as.if_stmt.condition) {
        ast_visit(node->as.if_stmt.condition, &vis6, ctx);
    }
    scope_stack_push(ctx->scopes);
    if (node->as.if_stmt.then_block) {
        ast_visit(node->as.if_stmt.then_block, &vis6, ctx);
    }
    scope_stack_pop(ctx->scopes);
    if (node->as.if_stmt.else_block) {
        scope_stack_push(ctx->scopes);
        ast_visit(node->as.if_stmt.else_block, &vis6, ctx);
        scope_stack_pop(ctx->scopes);
    }
}

static void visit_while_statement(ASTNode* node, void* data) {
    SemanticContext* ctx = (SemanticContext*)data;
    ASTVisitor vis7 = semantic_create_visitor();
    if (node->as.while_stmt.condition) {
        ast_visit(node->as.while_stmt.condition, &vis7, ctx);
    }
    int prev_in_loop = ctx->in_loop;
    ctx->in_loop = 1;
    scope_stack_push(ctx->scopes);
    if (node->as.while_stmt.body) {
        ast_visit(node->as.while_stmt.body, &vis7, ctx);
    }
    scope_stack_pop(ctx->scopes);
    ctx->in_loop = prev_in_loop;
}

static void visit_for_statement(ASTNode* node, void* data) {
    SemanticContext* ctx = (SemanticContext*)data;
    ASTVisitor vis8 = semantic_create_visitor();
    scope_stack_push(ctx->scopes);
    if (node->as.for_stmt.init) {
        ast_visit(node->as.for_stmt.init, &vis8, ctx);
    }
    if (node->as.for_stmt.condition) {
        ast_visit(node->as.for_stmt.condition, &vis8, ctx);
    }
    int prev_in_loop = ctx->in_loop;
    ctx->in_loop = 1;
    if (node->as.for_stmt.update) {
        ast_visit(node->as.for_stmt.update, &vis8, ctx);
    }
    if (node->as.for_stmt.body) {
        ast_visit(node->as.for_stmt.body, &vis8, ctx);
    }
    ctx->in_loop = prev_in_loop;
    scope_stack_pop(ctx->scopes);
}

static void visit_assignment_expression(ASTNode* node, void* data) {
    SemanticContext* ctx = (SemanticContext*)data;
    ASTNode* target_node = node->as.assignment.target;
    
    if (target_node->type == NODE_IDENTIFIER) {
        const char* name = target_node->as.identifier.name;
        Symbol* sym = scope_stack_lookup(ctx->scopes, name);
        if (!sym) {
            semantic_error_add(ctx->errors, S001_UNDEFINED_VARIABLE, node->start.line, node->start.column, node->filename, "المتغير غير معرف: %s", name);
            return;
        }
        if (!sym->is_mutable) {
            semantic_error_add(ctx->errors, S014_CONSTANT_REASSIGNMENT, node->start.line, node->start.column, node->filename, "لا يمكن إعادة تعيين الثابت: %s", name);
            return;
        }
        if (node->as.assignment.value) {
            ASTVisitor vis9 = semantic_create_visitor();
            ast_visit(node->as.assignment.value, &vis9, ctx);
            DaadType* value_type = infer_expression_type(node->as.assignment.value, ctx);
            type_checker_check_assignment(ctx->type_checker, sym->type, value_type, node->start.line, node->start.column, node->filename);
        }
        sym->is_initialized = 1;
    } else if (target_node->type == NODE_INDEX_EXPRESSION) {
        ASTVisitor vis = semantic_create_visitor();
        ast_visit(target_node, &vis, ctx);
        if (node->as.assignment.value) {
            ASTVisitor vis2 = semantic_create_visitor();
            ast_visit(node->as.assignment.value, &vis2, ctx);
        }
    } else if (target_node->type == NODE_MEMBER_EXPRESSION) {
        ASTVisitor vis = semantic_create_visitor();
        ast_visit(target_node, &vis, ctx);
        if (node->as.assignment.value) {
            ASTVisitor vis2 = semantic_create_visitor();
            ast_visit(node->as.assignment.value, &vis2, ctx);
        }
    }
}

static void visit_binary_expression(ASTNode* node, void* data) {
    SemanticContext* ctx = (SemanticContext*)data;
    ASTVisitor vis = semantic_create_visitor();
    if (node->as.binary.left) ast_visit(node->as.binary.left, &vis, ctx);
    if (node->as.binary.right) ast_visit(node->as.binary.right, &vis, ctx);
    DaadType* left_type = infer_expression_type(node->as.binary.left, ctx);
    DaadType* right_type = infer_expression_type(node->as.binary.right, ctx);
    type_checker_check_binary(ctx->type_checker, node->as.binary.op, left_type, right_type, node->start.line, node->start.column, node->filename);
}

static void visit_unary_expression(ASTNode* node, void* data) {
    SemanticContext* ctx = (SemanticContext*)data;
    ASTVisitor vis = semantic_create_visitor();
    if (node->as.unary.operand) ast_visit(node->as.unary.operand, &vis, ctx);
    DaadType* operand_type = infer_expression_type(node->as.unary.operand, ctx);
    type_checker_check_unary(ctx->type_checker, node->as.unary.op, operand_type, node->start.line, node->start.column, node->filename);
}

static void visit_literal(ASTNode* node, void* data) {
    (void)node; (void)data;
}

static void visit_identifier(ASTNode* node, void* data) {
    SemanticContext* ctx = (SemanticContext*)data;
    const char* name = node->as.identifier.name;
    Symbol* sym = scope_stack_lookup(ctx->scopes, name);
    if (!sym) {
        semantic_error_add(ctx->errors, S001_UNDEFINED_VARIABLE, node->start.line, node->start.column, node->filename, "المتغير غير معرف: %s", name);
    }
}

static void visit_call_expression(ASTNode* node, void* data) {
    SemanticContext* ctx = (SemanticContext*)data;
    ASTVisitor vis = semantic_create_visitor();
    if (node->as.call.callee) {
        ast_visit(node->as.call.callee, &vis, ctx);
    }
    for (size_t i = 0; i < node->as.call.args.size; i++) {
        ast_visit(node->as.call.args.items[i], &vis, ctx);
    }

    if (node->as.call.callee && node->as.call.callee->type == NODE_IDENTIFIER) {
        const char* func_name = node->as.call.callee->as.identifier.name;
        Symbol* func_sym = scope_stack_lookup(ctx->scopes, func_name);
        if (func_sym && func_sym->type && func_sym->type->kind == TYPE_FUNCTION) {
            DaadType* func_type = func_sym->type;
            DaadType** arg_types = NULL;
            int arg_count = (int)node->as.call.args.size;
            if (arg_count > 0) {
                arg_types = (DaadType**)malloc(sizeof(DaadType*) * arg_count);
                if (arg_types) {
                    for (int i = 0; i < arg_count; i++) {
                        arg_types[i] = infer_expression_type(node->as.call.args.items[i], ctx);
                    }
                }
            }
            type_checker_check_call(ctx->type_checker, func_type, arg_types, arg_count, node->start.line, node->start.column, node->filename);
            free(arg_types);
        }
    }
}

static void visit_cast_expression(ASTNode* node, void* data) {
    SemanticContext* ctx = (SemanticContext*)data;
    ASTVisitor vis = semantic_create_visitor();
    if (node->as.cast.expr) {
        ast_visit(node->as.cast.expr, &vis, ctx);
    }
    DaadType* from_type = infer_expression_type(node->as.cast.expr, ctx);
    DaadType* to_type = resolve_type_node(node->as.cast.type_node, ctx);
    type_checker_check_cast(ctx->type_checker, from_type, to_type, node->start.line, node->start.column, node->filename);
}

static void visit_parameter(ASTNode* node, void* data) {
    SemanticContext* ctx = (SemanticContext*)data;
    const char* name = node->as.param.name;
    DaadType* param_type = resolve_type_node(node->as.param.type_node, ctx);
    Symbol* sym = symbol_create(name, SYMBOL_PARAMETER, param_type, ctx->scopes->current_level);
    if (!sym) return;
    sym->is_initialized = 1;
    scope_stack_define(ctx->scopes, sym);
}

static void visit_break_statement(ASTNode* node, void* data) {
    SemanticContext* ctx = (SemanticContext*)data;
    if (!ctx->in_loop) {
        semantic_error_add(ctx->errors, S017_BREAK_OUTSIDE_LOOP,
            node->start.line, node->start.column, node->filename,
            "break خارج حلقة");
    }
}

static void visit_continue_statement(ASTNode* node, void* data) {
    SemanticContext* ctx = (SemanticContext*)data;
    if (!ctx->in_loop) {
        semantic_error_add(ctx->errors, S018_CONTINUE_OUTSIDE_LOOP,
            node->start.line, node->start.column, node->filename,
            "continue خارج حلقة");
    }
}

static void visit_array_expression(ASTNode* node, void* data) {
    SemanticContext* ctx = (SemanticContext*)data;
    ASTVisitor vis = semantic_create_visitor();
    for (size_t i = 0; i < node->as.array.elements.size; i++) {
        ast_visit(node->as.array.elements.items[i], &vis, ctx);
    }
}

static void visit_struct_expression(ASTNode* node, void* data) {
    SemanticContext* ctx = (SemanticContext*)data;
    ASTVisitor vis = semantic_create_visitor();
    for (size_t i = 0; i < node->as.struct_expr.values.size; i++) {
        ast_visit(node->as.struct_expr.values.items[i], &vis, ctx);
    }
}

static void visit_pointer_expression(ASTNode* node, void* data) {
    SemanticContext* ctx = (SemanticContext*)data;
    if (node->as.pointer.type_node) {
        resolve_type_node(node->as.pointer.type_node, ctx);
    }
}

static void visit_member_expression(ASTNode* node, void* data) {
    SemanticContext* ctx = (SemanticContext*)data;
    ASTVisitor vis = semantic_create_visitor();
    if (node->as.member.object) {
        ast_visit(node->as.member.object, &vis, ctx);
    }
}

static void visit_index_expression(ASTNode* node, void* data) {
    SemanticContext* ctx = (SemanticContext*)data;
    ASTVisitor vis = semantic_create_visitor();
    if (node->as.index.array) {
        ast_visit(node->as.index.array, &vis, ctx);
    }
    if (node->as.index.index) {
        ast_visit(node->as.index.index, &vis, ctx);
    }
}

int semantic_analyze(ASTNode* root, SemanticContext* ctx) {
    if (!root || !ctx) return 0;
    ASTVisitor visitor = semantic_create_visitor();
    ast_visit(root, &visitor, ctx);
    return (int)ctx->errors->size;
}