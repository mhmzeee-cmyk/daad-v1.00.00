/**
 * @file ast.c
 * @brief تنفيذ واجهة AST في لغة ض Core
 * 
 * @version 0.1.0
 * @date 2026-08-03
 */

#include "ast.h"
#include <stdlib.h>
#include <string.h>

#define LIST_INITIAL_CAPACITY 8

ASTNode* ast_node_create(NodeType type) {
    ASTNode* node = (ASTNode*)calloc(1, sizeof(ASTNode));
    if (!node) return NULL;
    node->type = type;
    node->error_flag = 0;
    return node;
}

static void ast_node_list_destroy_inner(ASTNodeList* list);

void ast_node_destroy(ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case NODE_PROGRAM:
            ast_node_list_destroy_inner(&node->as.program.declarations);
            break;

        case NODE_BLOCK:
            ast_node_list_destroy_inner(&node->as.block.statements);
            break;

        case NODE_VARIABLE_DECL:
            free(node->as.var_decl.name);
            ast_node_destroy(node->as.var_decl.type_node);
            ast_node_destroy(node->as.var_decl.init_expr);
            break;

        case NODE_CONSTANT_DECL:
            free(node->as.const_decl.name);
            ast_node_destroy(node->as.const_decl.value);
            break;

        case NODE_FUNCTION_DECL:
            free(node->as.func_decl.name);
            ast_node_list_destroy_inner(&node->as.func_decl.params);
            ast_node_destroy(node->as.func_decl.return_type);
            ast_node_destroy(node->as.func_decl.body);
            break;

        case NODE_PARAMETER:
            free(node->as.param.name);
            ast_node_destroy(node->as.param.type_node);
            break;

        case NODE_RETURN_STATEMENT:
            ast_node_destroy(node->as.return_stmt.value);
            break;

        case NODE_IF_STATEMENT:
            ast_node_destroy(node->as.if_stmt.condition);
            ast_node_destroy(node->as.if_stmt.then_block);
            ast_node_destroy(node->as.if_stmt.else_block);
            break;

        case NODE_WHILE_STATEMENT:
            ast_node_destroy(node->as.while_stmt.condition);
            ast_node_destroy(node->as.while_stmt.body);
            break;

        case NODE_FOR_STATEMENT:
            ast_node_destroy(node->as.for_stmt.init);
            ast_node_destroy(node->as.for_stmt.condition);
            ast_node_destroy(node->as.for_stmt.update);
            ast_node_destroy(node->as.for_stmt.body);
            break;

        case NODE_BREAK_STATEMENT:
        case NODE_CONTINUE_STATEMENT:
            break;

        case NODE_ASSIGNMENT_EXPRESSION:
            ast_node_destroy(node->as.assignment.target);
            ast_node_destroy(node->as.assignment.value);
            break;

        case NODE_BINARY_EXPRESSION:
            ast_node_destroy(node->as.binary.left);
            ast_node_destroy(node->as.binary.right);
            break;

        case NODE_UNARY_EXPRESSION:
            ast_node_destroy(node->as.unary.operand);
            break;

        case NODE_LITERAL:
            if (node->as.literal.kind == LITERAL_STRING) {
                free(node->as.literal.as.string_value);
            }
            break;

        case NODE_IDENTIFIER:
            free(node->as.identifier.name);
            break;

        case NODE_CALL_EXPRESSION:
            ast_node_destroy(node->as.call.callee);
            ast_node_list_destroy_inner(&node->as.call.args);
            break;

        case NODE_ARRAY_EXPRESSION:
            ast_node_list_destroy_inner(&node->as.array.elements);
            break;

        case NODE_STRUCT_EXPRESSION:
            ast_node_list_destroy_inner(&node->as.struct_expr.names);
            ast_node_list_destroy_inner(&node->as.struct_expr.values);
            break;

        case NODE_POINTER_EXPRESSION:
            ast_node_destroy(node->as.pointer.type_node);
            break;

        case NODE_MEMBER_EXPRESSION:
            ast_node_destroy(node->as.member.object);
            free(node->as.member.member);
            break;

        case NODE_INDEX_EXPRESSION:
            ast_node_destroy(node->as.index.array);
            ast_node_destroy(node->as.index.index);
            break;

        case NODE_CAST_EXPRESSION:
            ast_node_destroy(node->as.cast.type_node);
            ast_node_destroy(node->as.cast.expr);
            break;

        case NODE_ARRAY_TYPE:
            ast_node_destroy(node->as.array_type.element_type);
            ast_node_destroy(node->as.array_type.size);
            break;

        case NODE_STRUCT_TYPE:
            ast_node_list_destroy_inner(&node->as.struct_type.fields);
            break;

        case NODE_STRUCT_FIELD:
            free(node->as.struct_field.name);
            ast_node_destroy(node->as.struct_field.type_node);
            break;

        case NODE_GOTO_STATEMENT:
            free(node->as.goto_stmt.label);
            break;

        case NODE_LABEL_STATEMENT:
            free(node->as.label_stmt.name);
            break;

        case NODE_TRY_STATEMENT:
            ast_node_destroy(node->as.try_stmt.body);
            ast_node_destroy(node->as.try_stmt.catch_body);
            free(node->as.try_stmt.error_var);
            break;

        case NODE_CATCH_STATEMENT:
            ast_node_destroy(node->as.catch_stmt.body);
            break;

        case NODE_THROW_STATEMENT:
            ast_node_destroy(node->as.throw_stmt.value);
            break;
    }

    free(node->filename);
    free(node);
}

static void ast_node_list_destroy_inner(ASTNodeList* list) {
    if (!list) return;
    for (size_t i = 0; i < list->size; i++) {
        ast_node_destroy(list->items[i]);
    }
    free(list->items);
    list->items = NULL;
    list->size = 0;
    list->capacity = 0;
}

void ast_node_list_init(ASTNodeList* list) {
    if (!list) return;
    list->items = NULL;
    list->size = 0;
    list->capacity = 0;
}

void ast_node_list_add(ASTNodeList* list, ASTNode* node) {
    if (!list || !node) return;

    if (list->size >= list->capacity) {
        size_t new_cap = list->capacity == 0 ? LIST_INITIAL_CAPACITY : list->capacity * 2;
        ASTNode** tmp = (ASTNode**)realloc(list->items, new_cap * sizeof(ASTNode*));
        if (!tmp) return;
        list->items = tmp;
        list->capacity = new_cap;
    }

    list->items[list->size++] = node;
}

void ast_node_list_destroy(ASTNodeList* list) {
    ast_node_list_destroy_inner(list);
}

void ast_node_set_position(ASTNode* node, size_t line, size_t column, size_t offset) {
    if (!node) return;
    node->start.line = line;
    node->start.column = column;
    node->start.offset = offset;
    node->end.line = line;
    node->end.column = column;
    node->end.offset = offset;
}

void ast_node_set_filename(ASTNode* node, const char* filename) {
    if (!node) return;
    free(node->filename);
    node->filename = filename ? strdup(filename) : NULL;
}

void ast_node_set_positions(ASTNode* node, Position start, Position end) {
    if (!node) return;
    node->start = start;
    node->end = end;
}
