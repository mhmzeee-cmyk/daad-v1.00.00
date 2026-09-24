/**
 * @file ast_printer.c
 * @brief تنفيذ طباعة AST في لغة ض Core
 */

#include "ast_printer.h"
#include <stdio.h>

static void print_indent(FILE* file, int depth) {
    for (int i = 0; i < depth; i++) {
        fprintf(file, "  ");
    }
}

static void print_operator(BinaryOperator op, FILE* file) {
    switch (op) {
        case OP_ADD: fprintf(file, "+"); break;
        case OP_SUB: fprintf(file, "-"); break;
        case OP_MUL: fprintf(file, "*"); break;
        case OP_DIV: fprintf(file, "/"); break;
        case OP_MOD: fprintf(file, "%%"); break;
        case OP_AND: fprintf(file, "&&"); break;
        case OP_OR: fprintf(file, "||"); break;
        case OP_BITWISE_AND: fprintf(file, "&"); break;
        case OP_BITWISE_OR: fprintf(file, "|"); break;
        case OP_BITWISE_XOR: fprintf(file, "^"); break;
        case OP_SHIFT_LEFT: fprintf(file, "<<"); break;
        case OP_SHIFT_RIGHT: fprintf(file, ">>"); break;
        case OP_EQUAL: fprintf(file, "=="); break;
        case OP_NOT_EQUAL: fprintf(file, "!="); break;
        case OP_GREATER: fprintf(file, ">"); break;
        case OP_LESS: fprintf(file, "<"); break;
        case OP_GREATER_EQUAL: fprintf(file, ">="); break;
        case OP_LESS_EQUAL: fprintf(file, "<="); break;
    }
}

static void print_unary_operator(UnaryOperator op, FILE* file) {
    switch (op) {
        case UNARY_NOT: fprintf(file, "!"); break;
        case UNARY_NEGATE: fprintf(file, "-"); break;
        case UNARY_BITWISE_NOT: fprintf(file, "~"); break;
        case UNARY_ADDRESS_OF: fprintf(file, "&"); break;
        case UNARY_DEREF: fprintf(file, "*"); break;
    }
}

static void print_assignment_operator(AssignmentOperator op, FILE* file) {
    switch (op) {
        case ASSIGN: fprintf(file, "="); break;
        case ASSIGN_ADD: fprintf(file, "+="); break;
        case ASSIGN_SUB: fprintf(file, "-="); break;
        case ASSIGN_MUL: fprintf(file, "*="); break;
        case ASSIGN_DIV: fprintf(file, "/="); break;
        case ASSIGN_MOD: fprintf(file, "%%="); break;
        case ASSIGN_SHIFT_LEFT: fprintf(file, "<<="); break;
        case ASSIGN_SHIFT_RIGHT: fprintf(file, ">>="); break;
    }
}

static const char* node_type_name(NodeType type) {
    switch (type) {
        case NODE_PROGRAM: return "Program";
        case NODE_BLOCK: return "Block";
        case NODE_VARIABLE_DECL: return "VariableDecl";
        case NODE_CONSTANT_DECL: return "ConstantDecl";
        case NODE_FUNCTION_DECL: return "FunctionDecl";
        case NODE_PARAMETER: return "Parameter";
        case NODE_RETURN_STATEMENT: return "Return";
        case NODE_IF_STATEMENT: return "If";
        case NODE_WHILE_STATEMENT: return "While";
        case NODE_FOR_STATEMENT: return "For";
        case NODE_BREAK_STATEMENT: return "Break";
        case NODE_CONTINUE_STATEMENT: return "Continue";
        case NODE_ASSIGNMENT_EXPRESSION: return "Assignment";
        case NODE_BINARY_EXPRESSION: return "BinaryExpr";
        case NODE_UNARY_EXPRESSION: return "UnaryExpr";
        case NODE_LITERAL: return "Literal";
        case NODE_IDENTIFIER: return "Identifier";
        case NODE_CALL_EXPRESSION: return "CallExpr";
        case NODE_ARRAY_EXPRESSION: return "ArrayExpr";
        case NODE_STRUCT_EXPRESSION: return "StructExpr";
        case NODE_POINTER_EXPRESSION: return "PointerExpr";
        case NODE_MEMBER_EXPRESSION: return "MemberExpr";
        case NODE_INDEX_EXPRESSION: return "IndexExpr";
        case NODE_CAST_EXPRESSION: return "CastExpr";
        case NODE_ARRAY_TYPE: return "ArrayType";
        case NODE_STRUCT_TYPE: return "StructType";
        case NODE_STRUCT_FIELD: return "StructField";
        case NODE_GOTO_STATEMENT: return "Goto";
        case NODE_LABEL_STATEMENT: return "Label";
        case NODE_TRY_STATEMENT: return "Try";
        case NODE_CATCH_STATEMENT: return "Catch";
        case NODE_THROW_STATEMENT: return "Throw";
    }
    return "Unknown";
}

static void ast_print_to_file_depth(ASTNode* node, int depth, FILE* file);

static void print_list_depth(ASTNodeList* list, const char* name, int depth, FILE* file) {
    print_indent(file, depth);
    fprintf(file, "%s (%zu items):\n", name, list->size);
    for (size_t i = 0; i < list->size; i++) {
        ast_print_to_file_depth(list->items[i], depth + 1, file);
    }
}

static void ast_print_to_file_depth(ASTNode* node, int depth, FILE* file) {
    if (!node) return;

    print_indent(file, depth);
    fprintf(file, "%s", node_type_name(node->type));

    switch (node->type) {
        case NODE_PROGRAM:
            fprintf(file, "\n");
            print_list_depth(&node->as.program.declarations, "declarations", depth + 1, file);
            break;

        case NODE_BLOCK:
            fprintf(file, "\n");
            print_list_depth(&node->as.block.statements, "statements", depth + 1, file);
            break;

        case NODE_VARIABLE_DECL:
            fprintf(file, " '%s'\n", node->as.var_decl.name ? node->as.var_decl.name : "?");
            if (node->as.var_decl.type_node) {
                print_indent(file, depth + 1);
                fprintf(file, "type:\n");
                ast_print_to_file_depth(node->as.var_decl.type_node, depth + 2, file);
            }
            if (node->as.var_decl.init_expr) {
                print_indent(file, depth + 1);
                fprintf(file, "init:\n");
                ast_print_to_file_depth(node->as.var_decl.init_expr, depth + 2, file);
            }
            break;

        case NODE_CONSTANT_DECL:
            fprintf(file, " '%s'\n", node->as.const_decl.name ? node->as.const_decl.name : "?");
            if (node->as.const_decl.value) {
                print_indent(file, depth + 1);
                fprintf(file, "value:\n");
                ast_print_to_file_depth(node->as.const_decl.value, depth + 2, file);
            }
            break;

        case NODE_FUNCTION_DECL:
            fprintf(file, " '%s'\n", node->as.func_decl.name ? node->as.func_decl.name : "?");
            print_list_depth(&node->as.func_decl.params, "params", depth + 1, file);
            if (node->as.func_decl.return_type) {
                print_indent(file, depth + 1);
                fprintf(file, "return_type:\n");
                ast_print_to_file_depth(node->as.func_decl.return_type, depth + 2, file);
            }
            if (node->as.func_decl.body) {
                print_indent(file, depth + 1);
                fprintf(file, "body:\n");
                ast_print_to_file_depth(node->as.func_decl.body, depth + 2, file);
            }
            break;

        case NODE_PARAMETER:
            fprintf(file, " '%s'\n", node->as.param.name ? node->as.param.name : "?");
            if (node->as.param.type_node) {
                print_indent(file, depth + 1);
                fprintf(file, "type:\n");
                ast_print_to_file_depth(node->as.param.type_node, depth + 2, file);
            }
            break;

        case NODE_RETURN_STATEMENT:
            fprintf(file, "\n");
            if (node->as.return_stmt.value) {
                print_indent(file, depth + 1);
                fprintf(file, "value:\n");
                ast_print_to_file_depth(node->as.return_stmt.value, depth + 2, file);
            }
            break;

        case NODE_IF_STATEMENT:
            fprintf(file, "\n");
            if (node->as.if_stmt.condition) {
                print_indent(file, depth + 1);
                fprintf(file, "condition:\n");
                ast_print_to_file_depth(node->as.if_stmt.condition, depth + 2, file);
            }
            if (node->as.if_stmt.then_block) {
                print_indent(file, depth + 1);
                fprintf(file, "then:\n");
                ast_print_to_file_depth(node->as.if_stmt.then_block, depth + 2, file);
            }
            if (node->as.if_stmt.else_block) {
                print_indent(file, depth + 1);
                fprintf(file, "else:\n");
                ast_print_to_file_depth(node->as.if_stmt.else_block, depth + 2, file);
            }
            break;

        case NODE_WHILE_STATEMENT:
            fprintf(file, "\n");
            if (node->as.while_stmt.condition) {
                print_indent(file, depth + 1);
                fprintf(file, "condition:\n");
                ast_print_to_file_depth(node->as.while_stmt.condition, depth + 2, file);
            }
            if (node->as.while_stmt.body) {
                print_indent(file, depth + 1);
                fprintf(file, "body:\n");
                ast_print_to_file_depth(node->as.while_stmt.body, depth + 2, file);
            }
            break;

        case NODE_FOR_STATEMENT:
            fprintf(file, "\n");
            if (node->as.for_stmt.init) {
                print_indent(file, depth + 1);
                fprintf(file, "init:\n");
                ast_print_to_file_depth(node->as.for_stmt.init, depth + 2, file);
            }
            if (node->as.for_stmt.condition) {
                print_indent(file, depth + 1);
                fprintf(file, "condition:\n");
                ast_print_to_file_depth(node->as.for_stmt.condition, depth + 2, file);
            }
            if (node->as.for_stmt.update) {
                print_indent(file, depth + 1);
                fprintf(file, "update:\n");
                ast_print_to_file_depth(node->as.for_stmt.update, depth + 2, file);
            }
            if (node->as.for_stmt.body) {
                print_indent(file, depth + 1);
                fprintf(file, "body:\n");
                ast_print_to_file_depth(node->as.for_stmt.body, depth + 2, file);
            }
            break;

        case NODE_BREAK_STATEMENT:
        case NODE_CONTINUE_STATEMENT:
            fprintf(file, "\n");
            break;

        case NODE_ASSIGNMENT_EXPRESSION:
            fprintf(file, " target:");
            if (node->as.assignment.target) {
                ast_print_to_file_depth(node->as.assignment.target, depth + 1, file);
            }
            fprintf(file, " op=");
            print_assignment_operator(node->as.assignment.op, file);
            fprintf(file, "\n");
            if (node->as.assignment.value) {
                print_indent(file, depth + 1);
                fprintf(file, "value:\n");
                ast_print_to_file_depth(node->as.assignment.value, depth + 2, file);
            }
            break;

        case NODE_BINARY_EXPRESSION:
            fprintf(file, " op=");
            print_operator(node->as.binary.op, file);
            fprintf(file, "\n");
            if (node->as.binary.left) {
                print_indent(file, depth + 1);
                fprintf(file, "left:\n");
                ast_print_to_file_depth(node->as.binary.left, depth + 2, file);
            }
            if (node->as.binary.right) {
                print_indent(file, depth + 1);
                fprintf(file, "right:\n");
                ast_print_to_file_depth(node->as.binary.right, depth + 2, file);
            }
            break;

        case NODE_UNARY_EXPRESSION:
            fprintf(file, " op=");
            print_unary_operator(node->as.unary.op, file);
            fprintf(file, "\n");
            if (node->as.unary.operand) {
                print_indent(file, depth + 1);
                fprintf(file, "operand:\n");
                ast_print_to_file_depth(node->as.unary.operand, depth + 2, file);
            }
            break;

        case NODE_LITERAL:
            switch (node->as.literal.kind) {
                case LITERAL_INT:
                    fprintf(file, " int=%lld\n", node->as.literal.as.int_value);
                    break;
                case LITERAL_FLOAT:
                    fprintf(file, " float=%g\n", node->as.literal.as.float_value);
                    break;
                case LITERAL_STRING:
                    fprintf(file, " string=\"%s\"\n", node->as.literal.as.string_value ? node->as.literal.as.string_value : "");
                    break;
                case LITERAL_CHAR:
                    fprintf(file, " char='%c'\n", node->as.literal.as.char_value);
                    break;
                case LITERAL_BOOL:
                    fprintf(file, " bool=%s\n", node->as.literal.as.bool_value ? "true" : "false");
                    break;
            }
            break;

        case NODE_IDENTIFIER:
            fprintf(file, " '%s'\n", node->as.identifier.name ? node->as.identifier.name : "?");
            break;

        case NODE_CALL_EXPRESSION:
            fprintf(file, "\n");
            if (node->as.call.callee) {
                print_indent(file, depth + 1);
                fprintf(file, "callee:\n");
                ast_print_to_file_depth(node->as.call.callee, depth + 2, file);
            }
            print_list_depth(&node->as.call.args, "args", depth + 1, file);
            break;

        case NODE_ARRAY_EXPRESSION:
            fprintf(file, "\n");
            print_list_depth(&node->as.array.elements, "elements", depth + 1, file);
            break;

        case NODE_STRUCT_EXPRESSION:
            fprintf(file, "\n");
            print_list_depth(&node->as.struct_expr.names, "names", depth + 1, file);
            print_list_depth(&node->as.struct_expr.values, "values", depth + 1, file);
            break;

        case NODE_POINTER_EXPRESSION:
            fprintf(file, "\n");
            if (node->as.pointer.type_node) {
                print_indent(file, depth + 1);
                fprintf(file, "type:\n");
                ast_print_to_file_depth(node->as.pointer.type_node, depth + 2, file);
            }
            break;

        case NODE_MEMBER_EXPRESSION:
            fprintf(file, ".%s\n", node->as.member.member ? node->as.member.member : "?");
            if (node->as.member.object) {
                print_indent(file, depth + 1);
                fprintf(file, "object:\n");
                ast_print_to_file_depth(node->as.member.object, depth + 2, file);
            }
            break;

        case NODE_INDEX_EXPRESSION:
            fprintf(file, "\n");
            if (node->as.index.array) {
                print_indent(file, depth + 1);
                fprintf(file, "array:\n");
                ast_print_to_file_depth(node->as.index.array, depth + 2, file);
            }
            if (node->as.index.index) {
                print_indent(file, depth + 1);
                fprintf(file, "index:\n");
                ast_print_to_file_depth(node->as.index.index, depth + 2, file);
            }
            break;

        case NODE_CAST_EXPRESSION:
            fprintf(file, "\n");
            if (node->as.cast.type_node) {
                print_indent(file, depth + 1);
                fprintf(file, "type:\n");
                ast_print_to_file_depth(node->as.cast.type_node, depth + 2, file);
            }
            if (node->as.cast.expr) {
                print_indent(file, depth + 1);
                fprintf(file, "expr:\n");
                ast_print_to_file_depth(node->as.cast.expr, depth + 2, file);
            }
            break;

        case NODE_ARRAY_TYPE:
            fprintf(file, "\n");
            if (node->as.array_type.element_type) {
                print_indent(file, depth + 1);
                fprintf(file, "element_type:\n");
                ast_print_to_file_depth(node->as.array_type.element_type, depth + 2, file);
            }
            if (node->as.array_type.size) {
                print_indent(file, depth + 1);
                fprintf(file, "size:\n");
                ast_print_to_file_depth(node->as.array_type.size, depth + 2, file);
            }
            break;

        case NODE_STRUCT_TYPE:
            fprintf(file, "\n");
            print_list_depth(&node->as.struct_type.fields, "fields", depth + 1, file);
            break;

        case NODE_STRUCT_FIELD:
            fprintf(file, " '%s'\n", node->as.struct_field.name ? node->as.struct_field.name : "?");
            if (node->as.struct_field.type_node) {
                print_indent(file, depth + 1);
                fprintf(file, "type:\n");
                ast_print_to_file_depth(node->as.struct_field.type_node, depth + 2, file);
            }
            break;

        case NODE_GOTO_STATEMENT:
            fprintf(file, " '%s'\n", node->as.goto_stmt.label ? node->as.goto_stmt.label : "?");
            break;

        case NODE_LABEL_STATEMENT:
            fprintf(file, " '%s'\n", node->as.label_stmt.name ? node->as.label_stmt.name : "?");
            break;

        case NODE_TRY_STATEMENT:
        case NODE_CATCH_STATEMENT:
        case NODE_THROW_STATEMENT:
            fprintf(file, "\n");
            break;
    }
}

void ast_print_to_file(ASTNode* node, FILE* file) {
    ast_print_to_file_depth(node, 0, file);
}

void ast_print(ASTNode* node) {
    ast_print_to_file(node, stdout);
}
