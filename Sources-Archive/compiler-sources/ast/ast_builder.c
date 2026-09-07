/**
 * @file ast_builder.c
 * @brief تنفيذ واجهة بناء عقد AST في لغة ض Core
 * 
 * @version 0.1.0
 * @date 2026-08-03
 */

#include "ast_builder.h"
#include "ast.h"
#include <stdlib.h>
#include <string.h>

static char* safe_strdup(const char* s) {
    if (!s) return NULL;
    return strdup(s);
}

ASTNode* ast_build_program(ASTNodeList* declarations) {
    ASTNode* node = ast_node_create(NODE_PROGRAM);
    if (!node) return NULL;
    if (declarations) {
        node->as.program.declarations = *declarations;
        free(declarations);
    } else {
        ast_node_list_init(&node->as.program.declarations);
    }
    return node;
}

ASTNode* ast_build_block(ASTNodeList* statements) {
    ASTNode* node = ast_node_create(NODE_BLOCK);
    if (!node) return NULL;
    if (statements) {
        node->as.block.statements = *statements;
        free(statements);
    } else {
        ast_node_list_init(&node->as.block.statements);
    }
    return node;
}

ASTNode* ast_build_variable_decl(const char* name, ASTNode* type_node, ASTNode* init_expr) {
    ASTNode* node = ast_node_create(NODE_VARIABLE_DECL);
    if (!node) return NULL;
    node->as.var_decl.name = safe_strdup(name);
    node->as.var_decl.type_node = type_node;
    node->as.var_decl.init_expr = init_expr;
    return node;
}

ASTNode* ast_build_constant_decl(const char* name, ASTNode* value) {
    ASTNode* node = ast_node_create(NODE_CONSTANT_DECL);
    if (!node) return NULL;
    node->as.const_decl.name = safe_strdup(name);
    node->as.const_decl.value = value;
    return node;
}

ASTNode* ast_build_function_decl(const char* name, ASTNodeList* params, ASTNode* return_type, ASTNode* body) {
    ASTNode* node = ast_node_create(NODE_FUNCTION_DECL);
    if (!node) return NULL;
    node->as.func_decl.name = safe_strdup(name);
    if (params) {
        node->as.func_decl.params = *params;
        free(params);
    } else {
        ast_node_list_init(&node->as.func_decl.params);
    }
    node->as.func_decl.return_type = return_type;
    node->as.func_decl.body = body;
    return node;
}

ASTNode* ast_build_parameter(const char* name, ASTNode* type_node) {
    ASTNode* node = ast_node_create(NODE_PARAMETER);
    if (!node) return NULL;
    node->as.param.name = safe_strdup(name);
    node->as.param.type_node = type_node;
    return node;
}

ASTNode* ast_build_return(ASTNode* value) {
    ASTNode* node = ast_node_create(NODE_RETURN_STATEMENT);
    if (!node) return NULL;
    node->as.return_stmt.value = value;
    return node;
}

ASTNode* ast_build_if(ASTNode* condition, ASTNode* then_block, ASTNode* else_block) {
    ASTNode* node = ast_node_create(NODE_IF_STATEMENT);
    if (!node) return NULL;
    node->as.if_stmt.condition = condition;
    node->as.if_stmt.then_block = then_block;
    node->as.if_stmt.else_block = else_block;
    return node;
}

ASTNode* ast_build_while(ASTNode* condition, ASTNode* body) {
    ASTNode* node = ast_node_create(NODE_WHILE_STATEMENT);
    if (!node) return NULL;
    node->as.while_stmt.condition = condition;
    node->as.while_stmt.body = body;
    return node;
}

ASTNode* ast_build_for(ASTNode* init, ASTNode* condition, ASTNode* update, ASTNode* body) {
    ASTNode* node = ast_node_create(NODE_FOR_STATEMENT);
    if (!node) return NULL;
    node->as.for_stmt.init = init;
    node->as.for_stmt.condition = condition;
    node->as.for_stmt.update = update;
    node->as.for_stmt.body = body;
    return node;
}

ASTNode* ast_build_break(void) {
    return ast_node_create(NODE_BREAK_STATEMENT);
}

ASTNode* ast_build_continue(void) {
    return ast_node_create(NODE_CONTINUE_STATEMENT);
}

ASTNode* ast_build_assignment(const char* name, AssignmentOperator op, ASTNode* value) {
    ASTNode* node = ast_node_create(NODE_ASSIGNMENT_EXPRESSION);
    if (!node) return NULL;
    node->as.assignment.target = ast_build_identifier(name);
    node->as.assignment.op = op;
    node->as.assignment.value = value;
    return node;
}

ASTNode* ast_build_assignment_target(ASTNode* target, AssignmentOperator op, ASTNode* value) {
    ASTNode* node = ast_node_create(NODE_ASSIGNMENT_EXPRESSION);
    if (!node) return NULL;
    node->as.assignment.target = target;
    node->as.assignment.op = op;
    node->as.assignment.value = value;
    return node;
}

ASTNode* ast_build_binary(BinaryOperator op, ASTNode* left, ASTNode* right) {
    ASTNode* node = ast_node_create(NODE_BINARY_EXPRESSION);
    if (!node) return NULL;
    node->as.binary.op = op;
    node->as.binary.left = left;
    node->as.binary.right = right;
    return node;
}

ASTNode* ast_build_unary(UnaryOperator op, ASTNode* operand) {
    ASTNode* node = ast_node_create(NODE_UNARY_EXPRESSION);
    if (!node) return NULL;
    node->as.unary.op = op;
    node->as.unary.operand = operand;
    return node;
}

ASTNode* ast_build_literal_int(long long value) {
    ASTNode* node = ast_node_create(NODE_LITERAL);
    if (!node) return NULL;
    node->as.literal.kind = LITERAL_INT;
    node->as.literal.as.int_value = value;
    return node;
}

ASTNode* ast_build_literal_float(double value) {
    ASTNode* node = ast_node_create(NODE_LITERAL);
    if (!node) return NULL;
    node->as.literal.kind = LITERAL_FLOAT;
    node->as.literal.as.float_value = value;
    return node;
}

ASTNode* ast_build_literal_string(const char* value) {
    ASTNode* node = ast_node_create(NODE_LITERAL);
    if (!node) return NULL;
    node->as.literal.kind = LITERAL_STRING;
    node->as.literal.as.string_value = safe_strdup(value);
    return node;
}

ASTNode* ast_build_literal_char(char value) {
    ASTNode* node = ast_node_create(NODE_LITERAL);
    if (!node) return NULL;
    node->as.literal.kind = LITERAL_CHAR;
    node->as.literal.as.char_value = value;
    return node;
}

ASTNode* ast_build_literal_bool(int value) {
    ASTNode* node = ast_node_create(NODE_LITERAL);
    if (!node) return NULL;
    node->as.literal.kind = LITERAL_BOOL;
    node->as.literal.as.bool_value = value;
    return node;
}

ASTNode* ast_build_identifier(const char* name) {
    ASTNode* node = ast_node_create(NODE_IDENTIFIER);
    if (!node) return NULL;
    node->as.identifier.name = safe_strdup(name);
    return node;
}

ASTNode* ast_build_call(ASTNode* callee, ASTNodeList* args) {
    ASTNode* node = ast_node_create(NODE_CALL_EXPRESSION);
    if (!node) return NULL;
    node->as.call.callee = callee;
    if (args) {
        node->as.call.args = *args;
        free(args);
    } else {
        ast_node_list_init(&node->as.call.args);
    }
    return node;
}

ASTNode* ast_build_array(ASTNodeList* elements) {
    ASTNode* node = ast_node_create(NODE_ARRAY_EXPRESSION);
    if (!node) return NULL;
    if (elements) {
        node->as.array.elements = *elements;
        free(elements);
    } else {
        ast_node_list_init(&node->as.array.elements);
    }
    return node;
}

ASTNode* ast_build_struct(ASTNodeList* names, ASTNodeList* values) {
    ASTNode* node = ast_node_create(NODE_STRUCT_EXPRESSION);
    if (!node) return NULL;
    if (names) {
        node->as.struct_expr.names = *names;
        free(names);
    } else {
        ast_node_list_init(&node->as.struct_expr.names);
    }
    if (values) {
        node->as.struct_expr.values = *values;
        free(values);
    } else {
        ast_node_list_init(&node->as.struct_expr.values);
    }
    return node;
}

ASTNode* ast_build_pointer(ASTNode* type_node) {
    ASTNode* node = ast_node_create(NODE_POINTER_EXPRESSION);
    if (!node) return NULL;
    node->as.pointer.type_node = type_node;
    return node;
}

ASTNode* ast_build_member(ASTNode* object, const char* member) {
    ASTNode* node = ast_node_create(NODE_MEMBER_EXPRESSION);
    if (!node) return NULL;
    node->as.member.object = object;
    node->as.member.member = safe_strdup(member);
    return node;
}

ASTNode* ast_build_index(ASTNode* array, ASTNode* index) {
    ASTNode* node = ast_node_create(NODE_INDEX_EXPRESSION);
    if (!node) return NULL;
    node->as.index.array = array;
    node->as.index.index = index;
    return node;
}

ASTNode* ast_build_cast(ASTNode* type_node, ASTNode* expr) {
    ASTNode* node = ast_node_create(NODE_CAST_EXPRESSION);
    if (!node) return NULL;
    node->as.cast.type_node = type_node;
    node->as.cast.expr = expr;
    return node;
}
