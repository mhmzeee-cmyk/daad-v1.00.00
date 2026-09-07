/**
 * @file ast_visitor.c
 * @brief تنفيذ الزيارة في AST في لغة ض Core
 * 
 * @version 0.1.0
 * @date 2026-08-03
 */

#include "ast_visitor.h"
#include <stddef.h>

ASTVisitor ast_visitor_create(void) {
    ASTVisitor visitor;
    visitor.visit_program = NULL;
    visitor.visit_block = NULL;
    visitor.visit_variable_decl = NULL;
    visitor.visit_constant_decl = NULL;
    visitor.visit_function_decl = NULL;
    visitor.visit_parameter = NULL;
    visitor.visit_return_statement = NULL;
    visitor.visit_if_statement = NULL;
    visitor.visit_while_statement = NULL;
    visitor.visit_for_statement = NULL;
    visitor.visit_break_statement = NULL;
    visitor.visit_continue_statement = NULL;
    visitor.visit_assignment_expression = NULL;
    visitor.visit_binary_expression = NULL;
    visitor.visit_unary_expression = NULL;
    visitor.visit_literal = NULL;
    visitor.visit_identifier = NULL;
    visitor.visit_call_expression = NULL;
    visitor.visit_array_expression = NULL;
    visitor.visit_struct_expression = NULL;
    visitor.visit_pointer_expression = NULL;
    visitor.visit_member_expression = NULL;
    visitor.visit_index_expression = NULL;
    visitor.visit_cast_expression = NULL;
    return visitor;
}

void ast_visit(ASTNode* node, ASTVisitor* visitor, void* data) {
    if (!node || !visitor) return;

    switch (node->type) {
        case NODE_PROGRAM:
            if (visitor->visit_program) visitor->visit_program(node, data);
            break;
        case NODE_BLOCK:
            if (visitor->visit_block) visitor->visit_block(node, data);
            break;
        case NODE_VARIABLE_DECL:
            if (visitor->visit_variable_decl) visitor->visit_variable_decl(node, data);
            break;
        case NODE_CONSTANT_DECL:
            if (visitor->visit_constant_decl) visitor->visit_constant_decl(node, data);
            break;
        case NODE_FUNCTION_DECL:
            if (visitor->visit_function_decl) visitor->visit_function_decl(node, data);
            break;
        case NODE_PARAMETER:
            if (visitor->visit_parameter) visitor->visit_parameter(node, data);
            break;
        case NODE_RETURN_STATEMENT:
            if (visitor->visit_return_statement) visitor->visit_return_statement(node, data);
            break;
        case NODE_IF_STATEMENT:
            if (visitor->visit_if_statement) visitor->visit_if_statement(node, data);
            break;
        case NODE_WHILE_STATEMENT:
            if (visitor->visit_while_statement) visitor->visit_while_statement(node, data);
            break;
        case NODE_FOR_STATEMENT:
            if (visitor->visit_for_statement) visitor->visit_for_statement(node, data);
            break;
        case NODE_BREAK_STATEMENT:
            if (visitor->visit_break_statement) visitor->visit_break_statement(node, data);
            break;
        case NODE_CONTINUE_STATEMENT:
            if (visitor->visit_continue_statement) visitor->visit_continue_statement(node, data);
            break;
        case NODE_ASSIGNMENT_EXPRESSION:
            if (visitor->visit_assignment_expression) visitor->visit_assignment_expression(node, data);
            break;
        case NODE_BINARY_EXPRESSION:
            if (visitor->visit_binary_expression) visitor->visit_binary_expression(node, data);
            break;
        case NODE_UNARY_EXPRESSION:
            if (visitor->visit_unary_expression) visitor->visit_unary_expression(node, data);
            break;
        case NODE_LITERAL:
            if (visitor->visit_literal) visitor->visit_literal(node, data);
            break;
        case NODE_IDENTIFIER:
            if (visitor->visit_identifier) visitor->visit_identifier(node, data);
            break;
        case NODE_CALL_EXPRESSION:
            if (visitor->visit_call_expression) visitor->visit_call_expression(node, data);
            break;
        case NODE_ARRAY_EXPRESSION:
            if (visitor->visit_array_expression) visitor->visit_array_expression(node, data);
            break;
        case NODE_STRUCT_EXPRESSION:
            if (visitor->visit_struct_expression) visitor->visit_struct_expression(node, data);
            break;
        case NODE_POINTER_EXPRESSION:
            if (visitor->visit_pointer_expression) visitor->visit_pointer_expression(node, data);
            break;
        case NODE_MEMBER_EXPRESSION:
            if (visitor->visit_member_expression) visitor->visit_member_expression(node, data);
            break;
        case NODE_INDEX_EXPRESSION:
            if (visitor->visit_index_expression) visitor->visit_index_expression(node, data);
            break;
        case NODE_CAST_EXPRESSION:
            if (visitor->visit_cast_expression) visitor->visit_cast_expression(node, data);
            break;
        case NODE_ARRAY_TYPE:
        case NODE_STRUCT_TYPE:
        case NODE_STRUCT_FIELD:
        case NODE_GOTO_STATEMENT:
        case NODE_LABEL_STATEMENT:
            break;
        case NODE_TRY_STATEMENT:
            if (visitor->visit_try_statement) visitor->visit_try_statement(node, data);
            break;
        case NODE_CATCH_STATEMENT:
            if (visitor->visit_catch_statement) visitor->visit_catch_statement(node, data);
            break;
        case NODE_THROW_STATEMENT:
            if (visitor->visit_throw_statement) visitor->visit_throw_statement(node, data);
            break;
    }
}
