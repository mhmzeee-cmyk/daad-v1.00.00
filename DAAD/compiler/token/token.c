/**
 * @file token.c
 * @brief تنفيذ دوال Token
 * 
 * @version 0.1.0
 * @date 2026-08-03
 */

#include "token.h"
#include <stdlib.h>
#include <string.h>

const char* token_type_name(TokenType type) {
    switch (type) {
        /* كلمات مفتاحية */
        case TOKEN_KEYWORD_VARIABLE: return "VARIABLE";
        case TOKEN_KEYWORD_CONSTANT: return "CONSTANT";
        case TOKEN_KEYWORD_TYPE_DEF: return "TYPE_DEF";
        case TOKEN_KEYWORD_TYPEOF: return "TYPEOF";
        case TOKEN_KEYWORD_STRUCT: return "STRUCT";
        case TOKEN_KEYWORD_ARRAY: return "ARRAY";
        case TOKEN_KEYWORD_POINTER_TYPE: return "POINTER_TYPE";
        case TOKEN_KEYWORD_INT: return "INT";
        case TOKEN_KEYWORD_FLOAT: return "FLOAT";
        case TOKEN_KEYWORD_CHAR: return "CHAR";
        case TOKEN_KEYWORD_BOOL: return "BOOL";
        case TOKEN_KEYWORD_VOID: return "VOID";
        case TOKEN_KEYWORD_STRING: return "STRING";
        case TOKEN_KEYWORD_ADD: return "ADD";
        case TOKEN_KEYWORD_SUB: return "SUB";
        case TOKEN_KEYWORD_MUL: return "MUL";
        case TOKEN_KEYWORD_DIV: return "DIV";
        case TOKEN_KEYWORD_MOD: return "MOD";
        case TOKEN_KEYWORD_AND: return "AND";
        case TOKEN_KEYWORD_OR: return "OR";
        case TOKEN_KEYWORD_NOT: return "NOT";
        case TOKEN_KEYWORD_BITWISE_AND: return "BITWISE_AND";
        case TOKEN_KEYWORD_BITWISE_OR: return "BITWISE_OR";
        case TOKEN_KEYWORD_BITWISE_XOR: return "BITWISE_XOR";
        case TOKEN_KEYWORD_BITWISE_NOT: return "BITWISE_NOT";
        case TOKEN_KEYWORD_SHIFT_LEFT: return "SHIFT_LEFT";
        case TOKEN_KEYWORD_SHIFT_RIGHT: return "SHIFT_RIGHT";
        case TOKEN_KEYWORD_EQUALS: return "KEYWORD_EQUALS";
        case TOKEN_KEYWORD_NOT_EQUALS: return "KEYWORD_NOT_EQUALS";
        case TOKEN_KEYWORD_GREATER: return "KEYWORD_GREATER";
        case TOKEN_KEYWORD_LESS: return "KEYWORD_LESS";
        case TOKEN_KEYWORD_GREATER_EQUALS: return "KEYWORD_GREATER_EQUALS";
        case TOKEN_KEYWORD_LESS_EQUALS: return "KEYWORD_LESS_EQUALS";
        case TOKEN_KEYWORD_LOAD: return "LOAD";
        case TOKEN_KEYWORD_STORE: return "STORE";
        case TOKEN_KEYWORD_READ_MEM: return "READ_MEM";
        case TOKEN_KEYWORD_READ_STACK: return "READ_STACK";
        case TOKEN_KEYWORD_PUSH: return "PUSH";
        case TOKEN_KEYWORD_POP: return "POP";
        case TOKEN_KEYWORD_MOVE: return "MOVE";
        case TOKEN_KEYWORD_COPY_MEM: return "COPY_MEM";
        case TOKEN_KEYWORD_CLEAR_MEM: return "CLEAR_MEM";
        case TOKEN_KEYWORD_ALLOC: return "ALLOC";
        case TOKEN_KEYWORD_FREE: return "FREE";
        case TOKEN_KEYWORD_RESIZE: return "RESIZE";
        case TOKEN_KEYWORD_IF: return "IF";
        case TOKEN_KEYWORD_ELSE: return "ELSE";
        case TOKEN_KEYWORD_WHILE: return "WHILE";
        case TOKEN_KEYWORD_FOR: return "FOR";
        case TOKEN_KEYWORD_RETURN: return "RETURN";
        case TOKEN_KEYWORD_GOTO: return "GOTO";
        case TOKEN_KEYWORD_EXIT: return "EXIT";
        case TOKEN_KEYWORD_LABEL: return "LABEL";
        case TOKEN_KEYWORD_FUNCTION: return "FUNCTION";
        case TOKEN_KEYWORD_SYSCALL: return "SYSCALL";
        case TOKEN_KEYWORD_END: return "END";
        case TOKEN_KEYWORD_LOCAL: return "LOCAL";
        case TOKEN_KEYWORD_FORWARD: return "FORWARD";
        case TOKEN_KEYWORD_TRY: return "TRY";
        case TOKEN_KEYWORD_CATCH: return "CATCH";
        case TOKEN_KEYWORD_THROW: return "THROW";
        case TOKEN_KEYWORD_PRINT: return "PRINT";
        case TOKEN_KEYWORD_INPUT: return "INPUT";
        case TOKEN_KEYWORD_APPEND: return "APPEND";
        case TOKEN_KEYWORD_OPEN: return "OPEN";
        case TOKEN_KEYWORD_READ_FILE: return "READ_FILE";
        case TOKEN_KEYWORD_WRITE_FILE: return "WRITE_FILE";
        case TOKEN_KEYWORD_CLOSE: return "CLOSE";
        case TOKEN_KEYWORD_CAST: return "CAST";
        case TOKEN_KEYWORD_SIZEOF_TYPE: return "SIZEOF_TYPE";
        case TOKEN_KEYWORD_SIZEOF_VALUE: return "SIZEOF_VALUE";
        case TOKEN_KEYWORD_TRUE: return "TRUE";
        case TOKEN_KEYWORD_FALSE: return "FALSE";
        case TOKEN_KEYWORD_NULL: return "NULL";
        
        /* أنواع البيانات */
        case TOKEN_INTEGER: return "INTEGER";
        case TOKEN_FLOAT: return "FLOAT_LIT";
        case TOKEN_STRING: return "STRING_LIT";
        case TOKEN_CHARACTER: return "CHAR_LIT";
        
        /* المعرفات */
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        
        /* المعاملات */
        case TOKEN_PLUS: return "PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_STAR: return "STAR";
        case TOKEN_SLASH: return "SLASH";
        case TOKEN_PERCENT: return "PERCENT";
        case TOKEN_ASSIGN: return "ASSIGN";
        case TOKEN_PLUS_ASSIGN: return "PLUS_ASSIGN";
        case TOKEN_MINUS_ASSIGN: return "MINUS_ASSIGN";
        case TOKEN_STAR_ASSIGN: return "STAR_ASSIGN";
        case TOKEN_SLASH_ASSIGN: return "SLASH_ASSIGN";
        case TOKEN_PERCENT_ASSIGN: return "PERCENT_ASSIGN";
        case TOKEN_SHIFT_LEFT_ASSIGN: return "SHIFT_LEFT_ASSIGN";
        case TOKEN_SHIFT_RIGHT_ASSIGN: return "SHIFT_RIGHT_ASSIGN";
        case TOKEN_EQUAL: return "EQUAL";
        case TOKEN_NOT_EQUAL: return "NOT_EQUAL";
        case TOKEN_GREATER: return "GREATER";
        case TOKEN_LESS: return "LESS";
        case TOKEN_GREATER_EQUAL: return "GREATER_EQUAL";
        case TOKEN_LESS_EQUAL: return "LESS_EQUAL";
        case TOKEN_AND: return "LOGIC_AND";
        case TOKEN_OR: return "LOGIC_OR";
        case TOKEN_NOT: return "LOGIC_NOT";
        case TOKEN_BITWISE_AND: return "BITWISE_AND";
        case TOKEN_BITWISE_OR: return "BITWISE_OR";
        case TOKEN_BITWISE_XOR: return "BITWISE_XOR";
        case TOKEN_BITWISE_NOT: return "BITWISE_NOT";
        case TOKEN_SHIFT_LEFT: return "SHIFT_LEFT";
        case TOKEN_SHIFT_RIGHT: return "SHIFT_RIGHT";
        
        /* الفواصل */
        case TOKEN_COLON: return "COLON";
        case TOKEN_SEMICOLON: return "SEMICOLON";
        case TOKEN_ARROW: return "ARROW";
        case TOKEN_COLON_ARROW: return "COLON_ARROW";
        case TOKEN_COMMA: return "COMMA";
        case TOKEN_DOT: return "DOT";
        case TOKEN_AT: return "AT";
        case TOKEN_LPAREN: return "LPAREN";
        case TOKEN_RPAREN: return "RPAREN";
        case TOKEN_LBRACE: return "LBRACE";
        case TOKEN_RBRACE: return "RBRACE";
        case TOKEN_LBRACKET: return "LBRACKET";
        case TOKEN_RBRACKET: return "RBRACKET";
        
        /* التعليقات */
        case TOKEN_COMMENT_SINGLE: return "COMMENT";
        case TOKEN_COMMENT_MULTI_START: return "COMMENT_START";
        case TOKEN_COMMENT_MULTI_END: return "COMMENT_END";
        
        /* خاص */
        case TOKEN_NEWLINE: return "NEWLINE";
        case TOKEN_EOF: return "EOF";
        case TOKEN_ERROR: return "ERROR";
        
        default: return "UNKNOWN";
    }
}

Token token_create(TokenType type, const char* value, size_t length) {
    Token token;
    token.type = type;
    token.value = value ? strdup(value) : NULL;
    token.length = length;
    token.start.line = 0;
    token.start.column = 0;
    token.start.offset = 0;
    token.end = token.start;
    token.filename = NULL;
    token.has_literal = 0;
    return token;
}

void token_destroy(Token* token) {
    if (!token) return;
    
    if (token->value) {
        free(token->value);
        token->value = NULL;
    }
}

Token token_copy(const Token* src) {
    if (!src) {
        Token empty = {0};
        return empty;
    }
    
    Token copy = *src;
    if (src->value) {
        copy.value = strdup(src->value);
    }
    return copy;
}

int token_is_type(const Token* token, TokenType type) {
    return token && token->type == type;
}

int token_is_value(const Token* token, const char* text) {
    if (!token || !text || !token->value) return 0;
    return strcmp(token->value, text) == 0;
}
