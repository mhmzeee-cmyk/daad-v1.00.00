/**
 * @file ast_node.h
 * @brief تعريف عقد AST في لغة ض Core
 * 
 * هذا الملف يحتوي على أنواع العقد والبيانات اللازمة لتمثيل شجرة الـ AST.
 * 
 * @version 0.1.0
 * @date 2026-08-03
 */

#ifndef DAAD_AST_NODE_H
#define DAAD_AST_NODE_H

#include "../token/token.h"
#include <stddef.h>

/**
 * @brief أنواع عقد AST
 */
typedef enum {
    NODE_PROGRAM,
    NODE_BLOCK,
    NODE_VARIABLE_DECL,
    NODE_CONSTANT_DECL,
    NODE_FUNCTION_DECL,
    NODE_PARAMETER,
    NODE_RETURN_STATEMENT,
    NODE_IF_STATEMENT,
    NODE_WHILE_STATEMENT,
    NODE_FOR_STATEMENT,
    NODE_BREAK_STATEMENT,
    NODE_CONTINUE_STATEMENT,
    NODE_ASSIGNMENT_EXPRESSION,
    NODE_BINARY_EXPRESSION,
    NODE_UNARY_EXPRESSION,
    NODE_LITERAL,
    NODE_IDENTIFIER,
    NODE_CALL_EXPRESSION,
    NODE_ARRAY_EXPRESSION,
    NODE_STRUCT_EXPRESSION,
    NODE_POINTER_EXPRESSION,
    NODE_MEMBER_EXPRESSION,
    NODE_INDEX_EXPRESSION,
    NODE_CAST_EXPRESSION,
    NODE_ARRAY_TYPE,
    NODE_STRUCT_TYPE,
    NODE_STRUCT_FIELD,
    NODE_GOTO_STATEMENT,
    NODE_LABEL_STATEMENT,
    NODE_TRY_STATEMENT,
    NODE_CATCH_STATEMENT,
    NODE_THROW_STATEMENT
} NodeType;

/**
 * @brief عمليات ثنائية
 */
typedef enum {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_AND,
    OP_OR,
    OP_BITWISE_AND,
    OP_BITWISE_OR,
    OP_BITWISE_XOR,
    OP_SHIFT_LEFT,
    OP_SHIFT_RIGHT,
    OP_EQUAL,
    OP_NOT_EQUAL,
    OP_GREATER,
    OP_LESS,
    OP_GREATER_EQUAL,
    OP_LESS_EQUAL
} BinaryOperator;

/**
 * @brief عمليات أحادية
 */
typedef enum {
    UNARY_NOT,
    UNARY_NEGATE,
    UNARY_BITWISE_NOT,
    UNARY_ADDRESS_OF,   /* Phase 12.5: &expr — address-of */
    UNARY_DEREF         /* Phase 12.5: *expr — pointer dereference */
} UnaryOperator;

/**
 * @brief عمليات الإسناد
 */
typedef enum {
    ASSIGN,
    ASSIGN_ADD,
    ASSIGN_SUB,
    ASSIGN_MUL,
    ASSIGN_DIV,
    ASSIGN_MOD,
    ASSIGN_SHIFT_LEFT,
    ASSIGN_SHIFT_RIGHT
} AssignmentOperator;

/* Forward declarations */
typedef struct ASTNode ASTNode;
typedef struct ASTNodeList ASTNodeList;

/**
 * @brief قائمة ديناميكية من عقد AST
 */
struct ASTNodeList {
    ASTNode** items;
    size_t size;
    size_t capacity;
};

/**
 * @brief بنية عقدة AST
 */
struct ASTNode {
    NodeType type;
    Position start;
    Position end;
    char* filename;
    int error_flag;

    union {
        /* NODE_PROGRAM */
        struct {
            ASTNodeList declarations;
        } program;

        /* NODE_BLOCK */
        struct {
            ASTNodeList statements;
        } block;

        /* NODE_VARIABLE_DECL */
        struct {
            char* name;
            ASTNode* type_node;
            ASTNode* init_expr;
        } var_decl;

        /* NODE_CONSTANT_DECL */
        struct {
            char* name;
            ASTNode* value;
        } const_decl;

        /* NODE_FUNCTION_DECL */
        struct {
            char* name;
            ASTNodeList params;
            ASTNode* return_type;
            ASTNode* body;
        } func_decl;

        /* NODE_PARAMETER */
        struct {
            char* name;
            ASTNode* type_node;
        } param;

        /* NODE_RETURN_STATEMENT */
        struct {
            ASTNode* value;
        } return_stmt;

        /* NODE_IF_STATEMENT */
        struct {
            ASTNode* condition;
            ASTNode* then_block;
            ASTNode* else_block;
        } if_stmt;

        /* NODE_WHILE_STATEMENT */
        struct {
            ASTNode* condition;
            ASTNode* body;
        } while_stmt;

        /* NODE_FOR_STATEMENT */
        struct {
            ASTNode* init;
            ASTNode* condition;
            ASTNode* update;
            ASTNode* body;
        } for_stmt;

        /* NODE_ASSIGNMENT_EXPRESSION */
        struct {
            ASTNode* target;
            AssignmentOperator op;
            ASTNode* value;
        } assignment;

        /* NODE_BINARY_EXPRESSION */
        struct {
            BinaryOperator op;
            ASTNode* left;
            ASTNode* right;
        } binary;

        /* NODE_UNARY_EXPRESSION */
        struct {
            UnaryOperator op;
            ASTNode* operand;
        } unary;

        /* NODE_LITERAL */
        struct {
            enum {
                LITERAL_INT,
                LITERAL_FLOAT,
                LITERAL_STRING,
                LITERAL_CHAR,
                LITERAL_BOOL
            } kind;
            union {
                long long int_value;
                double float_value;
                char* string_value;
                char char_value;
                int bool_value;
            } as;
        } literal;

        /* NODE_IDENTIFIER */
        struct {
            char* name;
        } identifier;

        /* NODE_CALL_EXPRESSION */
        struct {
            ASTNode* callee;
            ASTNodeList args;
        } call;

        /* NODE_ARRAY_EXPRESSION */
        struct {
            ASTNodeList elements;
        } array;

        /* NODE_STRUCT_EXPRESSION */
        struct {
            ASTNodeList names;
            ASTNodeList values;
        } struct_expr;

        /* NODE_POINTER_EXPRESSION */
        struct {
            ASTNode* type_node;
        } pointer;

        /* NODE_MEMBER_EXPRESSION */
        struct {
            ASTNode* object;
            char* member;
        } member;

        /* NODE_INDEX_EXPRESSION */
        struct {
            ASTNode* array;
            ASTNode* index;
        } index;

        /* NODE_CAST_EXPRESSION */
        struct {
            ASTNode* type_node;
            ASTNode* expr;
        } cast;

        /* NODE_ARRAY_TYPE */
        struct {
            ASTNode* element_type;
            ASTNode* size;
        } array_type;

        /* NODE_STRUCT_TYPE */
        struct {
            ASTNodeList fields;
        } struct_type;

        /* NODE_STRUCT_FIELD */
        struct {
            char* name;
            ASTNode* type_node;
        } struct_field;

        /* NODE_GOTO_STATEMENT */
        struct {
            char* label;
        } goto_stmt;

        /* NODE_LABEL_STATEMENT */
        struct {
            char* name;
        } label_stmt;

        /* NODE_TRY_STATEMENT */
        struct {
            ASTNode* body;
            ASTNode* catch_body;
            char* error_var;
        } try_stmt;

        /* NODE_CATCH_STATEMENT */
        struct {
            ASTNode* body;
        } catch_stmt;

        /* NODE_THROW_STATEMENT */
        struct {
            ASTNode* value;
        } throw_stmt;
    } as;
};

#endif /* DAAD_AST_NODE_H */
