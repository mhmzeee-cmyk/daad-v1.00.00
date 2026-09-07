/**
 * @file parser.c
 * @brief تنفيذ المحلل النحوي (Pratt Parser) لغة ض Core
 *
 * @version 0.1.0
 * @date 2026-08-03
 */

#include "parser_internal.h"
#include <stdlib.h>
#include <string.h>

/* ===== دوال مساعدة ===== */

static int at_end(Parser* p) {
    return p->current >= p->token_count ||
           p->tokens[p->current].type == TOKEN_EOF;
}

int parser_is_at_end(Parser* p) {
    return at_end(p);
}

Token* parser_current(Parser* p) {
    if (p->current < p->token_count)
        return &p->tokens[p->current];
    return &p->tokens[p->token_count > 0 ? p->token_count - 1 : 0];
}

Token* parser_peek(Parser* p) {
    return parser_current(p);
}

Token* parser_advance(Parser* p) {
    if (!at_end(p)) p->current++;
    return &p->tokens[p->current - 1];
}

int parser_check(Parser* p, TokenType type) {
    if (at_end(p)) return 0;
    return parser_current(p)->type == type;
}

int parser_match(Parser* p, TokenType type) {
    if (parser_check(p, type)) {
        parser_advance(p);
        return 1;
    }
    return 0;
}

int parser_match_value(Parser* p, TokenType type, const char* value) {
    if (at_end(p)) return 0;
    Token* tok = parser_current(p);
    if (tok->type == type && tok->value && strcmp(tok->value, value) == 0) {
        parser_advance(p);
        return 1;
    }
    return 0;
}

static void parser_report_error(Parser* p, const char* code, const char* msg) {
    Token* tok = parser_current(p);
    parser_error_list_add(p->errors, code, msg,
                          tok->start.line, tok->start.column, p->filename);
    p->had_error = 1;
}

Token* parser_expect(Parser* p, TokenType type, const char* error_msg) {
    if (parser_check(p, type)) {
        return parser_advance(p);
    }
    parser_report_error(p, "P001", error_msg);
    parser_synchronize(p);
    return parser_current(p);
}

/* ===== بناء عقد AST ===== */

ASTNode* parser_create_node(NodeType type, const Token* tok) {
    ASTNode* node = ast_node_create(type);
    if (!node) return NULL;
    if (tok) {
        node->start = tok->start;
        node->end = tok->end;
        ast_node_set_filename(node, tok->filename);
    }
    return node;
}

/* ===== تعريف الدوال الأمامية ===== */

static ASTNode* parse_statement(Parser* p);
static ASTNode* parse_expression(Parser* p);
static ASTNode* parse_precedence(Parser* p, int min_prec);
static ASTNode* parse_unary(Parser* p);
static ASTNode* parse_postfix(Parser* p);
static ASTNode* parse_primary(Parser* p);
static ASTNode* parse_type(Parser* p);
static ASTNodeList parse_param_list(Parser* p);
static ASTNodeList parse_arg_list(Parser* p);
static ASTNode* parse_block_inner(Parser* p);
static ASTNode* parse_if_stmt_no_end(Parser* p);
static ASTNode* parse_variable_decl(Parser* p);
static ASTNode* parse_constant_decl(Parser* p);
static ASTNode* parse_function_decl(Parser* p);
static ASTNode* parse_forward_decl(Parser* p);
static ASTNode* parse_if_stmt(Parser* p);
static ASTNode* parse_while_stmt(Parser* p);
static ASTNode* parse_for_stmt(Parser* p);
static ASTNode* parse_return_stmt(Parser* p);
static ASTNode* parse_assignment(Parser* p, ASTNode* left);
static ASTNode* parse_struct_type_decl(Parser* p);
static ASTNode* parse_array_decl(Parser* p);
static ASTNode* parse_try_stmt(Parser* p);
static ASTNode* parse_throw_stmt(Parser* p);

/* ===== التحليل: البرنامج ===== */

static ASTNode* parse_program(Parser* p) {
    ASTNode* prog = parser_create_node(NODE_PROGRAM, &p->tokens[0]);
    ast_node_list_init(&prog->as.program.declarations);

    while (!at_end(p)) {
        int before = p->current;
        Token* tok = parser_current(p);
        if (tok->type == TOKEN_NEWLINE) {
            parser_advance(p);
            continue;
        }

        ASTNode* decl = parse_statement(p);
        if (decl) {
            ast_node_list_add(&prog->as.program.declarations, decl);
        } else if (p->current == (int)before) {
            parser_advance(p);
        }
    }
    return prog;
}

/* ===== التحليل: تعليمة ===== */

static ASTNode* parse_statement(Parser* p) {
    Token* tok = parser_current(p);

    switch (tok->type) {
        case TOKEN_KEYWORD_VARIABLE:  return parse_variable_decl(p);
        case TOKEN_KEYWORD_CONSTANT:  return parse_constant_decl(p);
        case TOKEN_KEYWORD_FUNCTION:  return parse_function_decl(p);
        case TOKEN_KEYWORD_FORWARD:   return parse_forward_decl(p);
        case TOKEN_KEYWORD_TYPE_DEF:  return parse_struct_type_decl(p);
        case TOKEN_KEYWORD_LOCAL:     return parse_variable_decl(p);
        case TOKEN_KEYWORD_ARRAY:     return parse_array_decl(p);
        case TOKEN_KEYWORD_IF:        return parse_if_stmt(p);
        case TOKEN_KEYWORD_WHILE:     return parse_while_stmt(p);
        case TOKEN_KEYWORD_FOR:       return parse_for_stmt(p);
        case TOKEN_KEYWORD_RETURN:    return parse_return_stmt(p);
        case TOKEN_KEYWORD_GOTO: {
            ASTNode* node = parser_create_node(NODE_GOTO_STATEMENT, tok);
            parser_advance(p);
            Token* label = parser_expect(p, TOKEN_IDENTIFIER, "متوقع اسم العلامة بعد 'اقفز'");
            node->as.goto_stmt.label = strdup(label->value);
            parser_match(p, TOKEN_SEMICOLON);
            return node;
        }
        case TOKEN_AT: {
            parser_advance(p);
            Token* label = parser_expect(p, TOKEN_IDENTIFIER, "متوقع اسم العلامة بعد '@'");
            ASTNode* node = parser_create_node(NODE_LABEL_STATEMENT, tok);
            node->as.label_stmt.name = strdup(label->value);
            parser_match(p, TOKEN_COLON);
            parser_match(p, TOKEN_SEMICOLON);
            return node;
        }
        case TOKEN_KEYWORD_EXIT: {
            ASTNode* node = parser_create_node(NODE_CALL_EXPRESSION, tok);
            parser_advance(p);
            node->as.call.callee = parser_create_node(NODE_IDENTIFIER, tok);
            node->as.call.callee->as.identifier.name = strdup("توقف");
            ast_node_list_init(&node->as.call.args);
            parser_match(p, TOKEN_SEMICOLON);
            return node;
        }
        case TOKEN_KEYWORD_TRY:    return parse_try_stmt(p);
        case TOKEN_KEYWORD_THROW:  return parse_throw_stmt(p);
        case TOKEN_KEYWORD_PRINT: {
            ASTNode* node = parser_create_node(NODE_CALL_EXPRESSION, tok);
            parser_advance(p);
            parser_expect(p, TOKEN_LPAREN, "متوقع '(' بعد 'اطبع'");
            ASTNodeList args = parse_arg_list(p);
            parser_expect(p, TOKEN_RPAREN, "متوقع ')' بعد قائمة المعاملات");
            node->as.call.callee = parser_create_node(NODE_IDENTIFIER, tok);
            node->as.call.callee->as.identifier.name = strdup("اطبع");
            node->as.call.args = args;
            parser_match(p, TOKEN_SEMICOLON);
            return node;
        }
        case TOKEN_KEYWORD_INPUT: {
            ASTNode* node = parser_create_node(NODE_CALL_EXPRESSION, tok);
            parser_advance(p);
            parser_expect(p, TOKEN_LPAREN, "متوقع '(' بعد 'ادخل'");
            ASTNodeList args = parse_arg_list(p);
            parser_expect(p, TOKEN_RPAREN, "متوقع ')' بعد قائمة المعاملات");
            node->as.call.callee = parser_create_node(NODE_IDENTIFIER, tok);
            node->as.call.callee->as.identifier.name = strdup("ادخل");
            node->as.call.args = args;
            parser_match(p, TOKEN_SEMICOLON);
            return node;
        }
        case TOKEN_KEYWORD_OPEN: {
            ASTNode* node = parser_create_node(NODE_CALL_EXPRESSION, tok);
            parser_advance(p);
            parser_expect(p, TOKEN_LPAREN, "متوقع '(' بعد 'افتح'");
            ASTNodeList args = parse_arg_list(p);
            parser_expect(p, TOKEN_RPAREN, "متوقع ')' بعد قائمة المعاملات");
            node->as.call.callee = parser_create_node(NODE_IDENTIFIER, tok);
            node->as.call.callee->as.identifier.name = strdup("افتح");
            node->as.call.args = args;
            parser_match(p, TOKEN_SEMICOLON);
            return node;
        }
        case TOKEN_KEYWORD_READ_FILE: {
            ASTNode* node = parser_create_node(NODE_CALL_EXPRESSION, tok);
            parser_advance(p);
            parser_expect(p, TOKEN_LPAREN, "متوقع '(' بعد 'اقرأ_ملف'");
            ASTNodeList args = parse_arg_list(p);
            parser_expect(p, TOKEN_RPAREN, "متوقع ')' بعد قائمة المعاملات");
            node->as.call.callee = parser_create_node(NODE_IDENTIFIER, tok);
            node->as.call.callee->as.identifier.name = strdup("اقرأ_ملف");
            node->as.call.args = args;
            parser_match(p, TOKEN_SEMICOLON);
            return node;
        }
        case TOKEN_KEYWORD_WRITE_FILE: {
            ASTNode* node = parser_create_node(NODE_CALL_EXPRESSION, tok);
            parser_advance(p);
            parser_expect(p, TOKEN_LPAREN, "متوقع '(' بعد 'اكتب_ملف'");
            ASTNodeList args = parse_arg_list(p);
            parser_expect(p, TOKEN_RPAREN, "متوقع ')' بعد قائمة المعاملات");
            node->as.call.callee = parser_create_node(NODE_IDENTIFIER, tok);
            node->as.call.callee->as.identifier.name = strdup("اكتب_ملف");
            node->as.call.args = args;
            parser_match(p, TOKEN_SEMICOLON);
            return node;
        }
        case TOKEN_KEYWORD_CLOSE: {
            ASTNode* node = parser_create_node(NODE_CALL_EXPRESSION, tok);
            parser_advance(p);
            parser_expect(p, TOKEN_LPAREN, "متوقع '(' بعد 'اغلق'");
            ASTNodeList args = parse_arg_list(p);
            parser_expect(p, TOKEN_RPAREN, "متوقع ')' بعد قائمة المعاملات");
            node->as.call.callee = parser_create_node(NODE_IDENTIFIER, tok);
            node->as.call.callee->as.identifier.name = strdup("اغلق");
            node->as.call.args = args;
            parser_match(p, TOKEN_SEMICOLON);
            return node;
        }
        case TOKEN_KEYWORD_ALLOC: {
            ASTNode* node = parser_create_node(NODE_CALL_EXPRESSION, tok);
            parser_advance(p);
            parser_expect(p, TOKEN_LPAREN, "متوقع '(' بعد 'احجز'");
            ASTNodeList args = parse_arg_list(p);
            parser_expect(p, TOKEN_RPAREN, "متوقع ')' بعد قائمة المعاملات");
            node->as.call.callee = parser_create_node(NODE_IDENTIFIER, tok);
            node->as.call.callee->as.identifier.name = strdup("احجز");
            node->as.call.args = args;
            parser_match(p, TOKEN_SEMICOLON);
            return node;
        }
        case TOKEN_KEYWORD_FREE: {
            ASTNode* node = parser_create_node(NODE_CALL_EXPRESSION, tok);
            parser_advance(p);
            parser_expect(p, TOKEN_LPAREN, "متوقع '(' بعد 'حرر'");
            ASTNodeList args = parse_arg_list(p);
            parser_expect(p, TOKEN_RPAREN, "متوقع ')' بعد قائمة المعاملات");
            node->as.call.callee = parser_create_node(NODE_IDENTIFIER, tok);
            node->as.call.callee->as.identifier.name = strdup("حرر");
            node->as.call.args = args;
            parser_match(p, TOKEN_SEMICOLON);
            return node;
        }
        case TOKEN_KEYWORD_RESIZE: {
            ASTNode* node = parser_create_node(NODE_CALL_EXPRESSION, tok);
            parser_advance(p);
            parser_expect(p, TOKEN_LPAREN, "متوقع '(' بعد 'غيّر_حجم'");
            ASTNodeList args = parse_arg_list(p);
            parser_expect(p, TOKEN_RPAREN, "متوقع ')' بعد قائمة المعاملات");
            node->as.call.callee = parser_create_node(NODE_IDENTIFIER, tok);
            node->as.call.callee->as.identifier.name = strdup("غيّر_حجم");
            node->as.call.args = args;
            parser_match(p, TOKEN_SEMICOLON);
            return node;
        }
        default:
            break;
    }

    ASTNode* expr = parse_expression(p);

    if (expr && (expr->type == NODE_IDENTIFIER || expr->type == NODE_INDEX_EXPRESSION || expr->type == NODE_MEMBER_EXPRESSION)) {
        Token* next = parser_current(p);
        switch (next->type) {
            case TOKEN_ASSIGN:
            case TOKEN_PLUS_ASSIGN:
            case TOKEN_MINUS_ASSIGN:
            case TOKEN_STAR_ASSIGN:
            case TOKEN_SLASH_ASSIGN:
            case TOKEN_PERCENT_ASSIGN:
            case TOKEN_SHIFT_LEFT_ASSIGN:
            case TOKEN_SHIFT_RIGHT_ASSIGN:
                return parse_assignment(p, expr);
            default:
                break;
        }
    }

    parser_match(p, TOKEN_SEMICOLON);
    return expr;
}

/* ===== التحليل: بلوك ===== */

static ASTNode* parse_block_inner(Parser* p) {
    ASTNode* block = parser_create_node(NODE_BLOCK, parser_peek(p));
    ast_node_list_init(&block->as.block.statements);

    while (!at_end(p)) {
        Token* tok = parser_current(p);
        if (tok->type == TOKEN_KEYWORD_END) break;
        if (tok->type == TOKEN_KEYWORD_ELSE) break;
        if (tok->type == TOKEN_KEYWORD_CATCH) break;
        if (tok->type == TOKEN_NEWLINE) {
            parser_advance(p);
            continue;
        }

        int before = p->current;
        ASTNode* stmt = parse_statement(p);
        if (stmt) {
            ast_node_list_add(&block->as.block.statements, stmt);
        } else if (p->current == (int)before) {
            parser_advance(p);
        }
    }

    return block;
}

/* ===== التحليل: نوع ===== */

static ASTNode* parse_type(Parser* p) {
    Token* tok = parser_current(p);

    if (tok->type == TOKEN_KEYWORD_POINTER_TYPE) {
        parser_advance(p);
        parser_expect(p, TOKEN_LPAREN, "متوقع '(' بعد 'مؤشر_نوع'");
        ASTNode* inner = parse_type(p);
        parser_expect(p, TOKEN_RPAREN, "متوقع ')' بعد النوع");
        ASTNode* node = parser_create_node(NODE_POINTER_EXPRESSION, tok);
        node->as.pointer.type_node = inner;
        return node;
    }

    if (tok->type == TOKEN_KEYWORD_INT ||
        tok->type == TOKEN_KEYWORD_FLOAT ||
        tok->type == TOKEN_KEYWORD_CHAR ||
        tok->type == TOKEN_KEYWORD_BOOL ||
        tok->type == TOKEN_KEYWORD_VOID ||
        tok->type == TOKEN_KEYWORD_STRING) {
        ASTNode* node = parser_create_node(NODE_IDENTIFIER, tok);
        node->as.identifier.name = strdup(tok->value);
        parser_advance(p);
        if (parser_match(p, TOKEN_LBRACKET)) {
            parser_expect(p, TOKEN_RBRACKET, "متوقع ']' بعد '['");
            ASTNode* arr_type = parser_create_node(NODE_POINTER_EXPRESSION, tok);
            arr_type->as.pointer.type_node = node;
            return arr_type;
        }
        return node;
    }

    if (tok->type ==TOKEN_IDENTIFIER) {
        ASTNode* node = parser_create_node(NODE_IDENTIFIER, tok);
        node->as.identifier.name = strdup(tok->value);
        parser_advance(p);
        if (parser_match(p, TOKEN_LBRACKET)) {
            ASTNode* size = NULL;
            if (!parser_check(p, TOKEN_RBRACKET)) {
                size = parse_expression(p);
            }
            parser_expect(p, TOKEN_RBRACKET, "متوقع ']' بعد '['");
            ASTNode* arr_type = parser_create_node(NODE_ARRAY_TYPE, tok);
            arr_type->as.array_type.element_type = node;
            arr_type->as.array_type.size = size;
            return arr_type;
        }
        return node;
    }

    parser_report_error(p, "P005", "نوع غير معروف");
    return parser_create_node(NODE_IDENTIFIER, tok);
}

/* ===== التحليل: قائمة المعاملات ===== */

static ASTNodeList parse_param_list(Parser* p) {
    ASTNodeList list;
    ast_node_list_init(&list);

    if (parser_check(p, TOKEN_RPAREN)) return list;

    do {
        Token* name_tok = parser_expect(p, TOKEN_IDENTIFIER, "متوقع اسم المعامل");
        parser_expect(p, TOKEN_COLON, "متوقع ':' بعد اسم المعامل");
        ASTNode* type_node = parse_type(p);

        ASTNode* param = parser_create_node(NODE_PARAMETER, name_tok);
        param->as.param.name = strdup(name_tok->value);
        param->as.param.type_node = type_node;
        ast_node_list_add(&list, param);
    } while (parser_match(p, TOKEN_COMMA));

    return list;
}

/* ===== التحليل: قائمة الاستدعاء ===== */

static ASTNodeList parse_arg_list(Parser* p) {
    ASTNodeList list;
    ast_node_list_init(&list);

    if (parser_check(p, TOKEN_RPAREN)) return list;

    do {
        ASTNode* arg = parse_expression(p);
        if (arg) ast_node_list_add(&list, arg);
    } while (parser_match(p, TOKEN_COMMA));

    return list;
}

/* ===== التحليل: متغير ===== */

static ASTNode* parse_variable_decl(Parser* p) {
    Token* var_tok = parser_advance(p);
    parser_expect(p, TOKEN_COLON, "متوقع ':' بعد 'متغير'");

    Token* name_tok = parser_expect(p, TOKEN_IDENTIFIER, "متوقع اسم المتغير");
    char* name = strdup(name_tok->value);

    ASTNode* type_node = NULL;
    if (parser_match(p, TOKEN_COLON)) {
        type_node = parse_type(p);
    }

    ASTNode* init_expr = NULL;
    if (parser_match(p, TOKEN_ASSIGN)) {
        init_expr = parse_expression(p);
        if (!init_expr) {
            parser_report_error(p, "P002", "تعبير مفقود بعد '='");
        }
    }

    parser_match(p, TOKEN_SEMICOLON);

    ASTNode* node = parser_create_node(NODE_VARIABLE_DECL, var_tok);
    node->as.var_decl.name = name;
    node->as.var_decl.type_node = type_node;
    node->as.var_decl.init_expr = init_expr;
    return node;
}

/* ===== التحليل: ثابت ===== */

static ASTNode* parse_constant_decl(Parser* p) {
    Token* const_tok = parser_advance(p);
    parser_expect(p, TOKEN_COLON, "متوقع ':' بعد 'ثابت'");

    Token* name_tok = parser_expect(p, TOKEN_IDENTIFIER, "متوقع اسم الثابت");
    char* name = strdup(name_tok->value);

    parser_expect(p, TOKEN_ASSIGN, "متوقع '=' بعد اسم الثابت");
    ASTNode* value = parse_expression(p);

    parser_match(p, TOKEN_SEMICOLON);

    ASTNode* node = parser_create_node(NODE_CONSTANT_DECL, const_tok);
    node->as.const_decl.name = name;
    node->as.const_decl.value = value;
    return node;
}

/* ===== التحليل: دالة ===== */

static ASTNode* parse_function_decl(Parser* p) {
    Token* func_tok = parser_advance(p);
    parser_expect(p, TOKEN_COLON, "متوقع ':' بعد 'دالة'");

    Token* name_tok = parser_expect(p, TOKEN_IDENTIFIER, "متوقع اسم الدالة");
    char* name = strdup(name_tok->value);

    parser_expect(p, TOKEN_LPAREN, "متوقع '(' بعد اسم الدالة");
    ASTNodeList params = parse_param_list(p);
    parser_expect(p, TOKEN_RPAREN, "متوقع ')' بعد قائمة المعاملات");

    ASTNode* return_type = NULL;
    if (parser_match(p, TOKEN_ARROW)) {
        return_type = parse_type(p);
    }

    parser_expect(p, TOKEN_COLON_ARROW, "متوقع ':-' بعد تعريف الدالة");

    ASTNode* body = parse_block_inner(p);
    parser_expect(p, TOKEN_KEYWORD_END, "متوقع 'نهاية' لإغلاق الدالة");

    ASTNode* node = parser_create_node(NODE_FUNCTION_DECL, func_tok);
    node->as.func_decl.name = name;
    node->as.func_decl.params = params;
    node->as.func_decl.return_type = return_type;
    node->as.func_decl.body = body;
    return node;
}

/* ===== التحليل: أمامية ===== */

static ASTNode* parse_forward_decl(Parser* p) {
    Token* fwd_tok = parser_advance(p);
    parser_expect(p, TOKEN_COLON, "متوقع ':' بعد 'أمامية'");

    Token* name_tok = parser_expect(p, TOKEN_IDENTIFIER, "متوقع اسم الدالة");
    char* name = strdup(name_tok->value);

    parser_expect(p, TOKEN_LPAREN, "متوقع '(' بعد اسم الدالة");
    ASTNodeList params = parse_param_list(p);
    parser_expect(p, TOKEN_RPAREN, "متوقع ')' بعد قائمة المعاملات");

    ASTNode* return_type = NULL;
    if (parser_match(p, TOKEN_ARROW)) {
        return_type = parse_type(p);
    }

    parser_match(p, TOKEN_SEMICOLON);

    ASTNode* node = parser_create_node(NODE_FUNCTION_DECL, fwd_tok);
    node->as.func_decl.name = name;
    node->as.func_decl.params = params;
    node->as.func_decl.return_type = return_type;
    node->as.func_decl.body = NULL;
    return node;
}

/* ===== التحليل: نوع (تعريف هيكل) ===== */

static ASTNode* parse_struct_type_decl(Parser* p) {
    Token* type_tok = parser_advance(p);
    parser_expect(p, TOKEN_COLON, "متوقع ':' بعد 'نوع'");

    Token* name_tok = parser_expect(p, TOKEN_IDENTIFIER, "متوقع اسم النوع");
    char* name = strdup(name_tok->value);

    parser_expect(p, TOKEN_ASSIGN, "متوقع '=' بعد اسم النوع");
    parser_expect(p, TOKEN_LBRACE, "متوقع '{' لبداية تعريف الأعضاء");

    ASTNode* node = parser_create_node(NODE_STRUCT_TYPE, type_tok);
    ast_node_list_init(&node->as.struct_type.fields);

    while (!parser_check(p, TOKEN_RBRACE) && !parser_is_at_end(p)) {
        while (parser_match(p, TOKEN_NEWLINE)) {}
        if (parser_check(p, TOKEN_RBRACE)) break;
        Token* field_tok = parser_current(p);
        Token* field_name = parser_expect(p, TOKEN_IDENTIFIER, "متوقع اسم العضو");
        parser_expect(p, TOKEN_COLON, "متوقع ':' بعد اسم العضو");
        ASTNode* field_type = parse_type(p);

        ASTNode* field = parser_create_node(NODE_STRUCT_FIELD, field_tok);
        field->as.struct_field.name = strdup(field_name->value);
        field->as.struct_field.type_node = field_type;
        ast_node_list_add(&node->as.struct_type.fields, field);

        if (!parser_match(p, TOKEN_COMMA)) break;
    }

    while (parser_match(p, TOKEN_NEWLINE)) {}
    parser_expect(p, TOKEN_RBRACE, "متوقع '}' لإغلاق تعريف النوع");
    parser_match(p, TOKEN_SEMICOLON);

    ASTNode* decl = parser_create_node(NODE_VARIABLE_DECL, type_tok);
    decl->as.var_decl.name = name;
    decl->as.var_decl.type_node = node;
    decl->as.var_decl.init_expr = NULL;
    return decl;
}

/* ===== التحليل: مصفوفة ===== */

static ASTNode* parse_array_decl(Parser* p) {
    Token* arr_tok = parser_advance(p);
    parser_expect(p, TOKEN_COLON, "متوقع ':' بعد 'مصفوفة'");

    Token* name_tok = parser_expect(p, TOKEN_IDENTIFIER, "متوقع اسم المصفوفة");
    char* name = strdup(name_tok->value);

    parser_expect(p, TOKEN_LBRACKET, "متوقع '[' بعد اسم المصفوفة");
    ASTNode* size = NULL;
    if (!parser_check(p, TOKEN_RBRACKET)) {
        size = parse_expression(p);
    }
    parser_expect(p, TOKEN_RBRACKET, "متوقع ']' بعد حجم المصفوفة");

    ASTNode* type_node = parser_create_node(NODE_ARRAY_TYPE, arr_tok);
    type_node->as.array_type.element_type = parser_create_node(NODE_IDENTIFIER, arr_tok);
    type_node->as.array_type.element_type->as.identifier.name = strdup("رقم");
    type_node->as.array_type.size = size;

    ASTNode* init_expr = NULL;
    if (parser_match(p, TOKEN_ASSIGN)) {
        init_expr = parse_expression(p);
    }

    parser_match(p, TOKEN_SEMICOLON);

    ASTNode* node = parser_create_node(NODE_VARIABLE_DECL, arr_tok);
    node->as.var_decl.name = name;
    node->as.var_decl.type_node = type_node;
    node->as.var_decl.init_expr = init_expr;
    return node;
}

/* ===== التحليل: حاول/امسك ===== */

static ASTNode* parse_try_stmt(Parser* p) {
    Token* try_tok = parser_advance(p);
    parser_expect(p, TOKEN_COLON_ARROW, "متوقع ':-' بعد 'حاول'");

    ASTNode* body = parse_block_inner(p);

    ASTNode* catch_body = NULL;
    char* error_var = NULL;
    if (parser_match(p, TOKEN_KEYWORD_CATCH)) {
        parser_expect(p, TOKEN_LPAREN, "متوقع '(' بعد 'امسك'");
        Token* var_tok = parser_expect(p, TOKEN_IDENTIFIER, "متوقع اسم متغير الخطأ");
        error_var = strdup(var_tok->value);
        parser_expect(p, TOKEN_RPAREN, "متوقع ')' بعد اسم متغير الخطأ");
        parser_expect(p, TOKEN_COLON_ARROW, "متوقع ':-' بعد 'امسك'");
        catch_body = parse_block_inner(p);
    }

    parser_expect(p, TOKEN_KEYWORD_END, "متوقع 'نهاية' لإغلاق 'حاول'");

    ASTNode* node = parser_create_node(NODE_TRY_STATEMENT, try_tok);
    node->as.try_stmt.body = body;
    node->as.try_stmt.catch_body = catch_body;
    node->as.try_stmt.error_var = error_var;
    return node;
}

/* ===== التحليل: أطلق ===== */

static ASTNode* parse_throw_stmt(Parser* p) {
    Token* throw_tok = parser_advance(p);
    ASTNode* value = parse_expression(p);
    parser_match(p, TOKEN_SEMICOLON);

    ASTNode* node = parser_create_node(NODE_THROW_STATEMENT, throw_tok);
    node->as.throw_stmt.value = value;
    return node;
}

/* ===== التحليل: إذا (بدون نهاية) ===== */

static ASTNode* parse_if_stmt_no_end(Parser* p) {
    Token* if_tok = parser_advance(p);
    parser_expect(p, TOKEN_LPAREN, "متوقع '(' بعد 'إذا'");
    ASTNode* cond = parse_expression(p);
    parser_expect(p, TOKEN_RPAREN, "متوقع ')' بعد الشرط");
    parser_expect(p, TOKEN_COLON_ARROW, "متوقع ':-' بعد الشرط");

    ASTNode* then_block = parse_block_inner(p);

    ASTNode* else_block = NULL;
    if (parser_match(p, TOKEN_KEYWORD_ELSE)) {
        if (parser_check(p, TOKEN_KEYWORD_IF)) {
            else_block = parse_if_stmt_no_end(p);
        } else {
            parser_expect(p, TOKEN_COLON_ARROW, "متوقع ':-' بعد 'وإلا'");
            else_block = parse_block_inner(p);
        }
    }

    ASTNode* node = parser_create_node(NODE_IF_STATEMENT, if_tok);
    node->as.if_stmt.condition = cond;
    node->as.if_stmt.then_block = then_block;
    node->as.if_stmt.else_block = else_block;
    return node;
}

/* ===== التحليل: إذا (مع نهاية) ===== */

static ASTNode* parse_if_stmt(Parser* p) {
    ASTNode* node = parse_if_stmt_no_end(p);
    parser_expect(p, TOKEN_KEYWORD_END, "متوقع 'نهاية' لإغلاق 'إذا'");
    return node;
}

/* ===== التحليل: طالما ===== */

static ASTNode* parse_while_stmt(Parser* p) {
    Token* while_tok = parser_advance(p);
    parser_expect(p, TOKEN_LPAREN, "متوقع '(' بعد 'طالما'");
    ASTNode* cond = parse_expression(p);
    parser_expect(p, TOKEN_RPAREN, "متوقع ')' بعد الشرط");
    parser_expect(p, TOKEN_COLON_ARROW, "متوقع ':-' بعد الشرط");

    ASTNode* body = parse_block_inner(p);
    parser_expect(p, TOKEN_KEYWORD_END, "متوقع 'نهاية' لإغلاق 'طالما'");

    ASTNode* node = parser_create_node(NODE_WHILE_STATEMENT, while_tok);
    node->as.while_stmt.condition = cond;
    node->as.while_stmt.body = body;
    return node;
}

/* ===== التحليل: لكل ===== */

static ASTNode* parse_for_stmt(Parser* p) {
    Token* for_tok = parser_advance(p);
    parser_expect(p, TOKEN_LPAREN, "متوقع '(' بعد 'لكل'");

    ASTNode* init = NULL;
    Token* tok = parser_current(p);
    if (tok->type == TOKEN_KEYWORD_VARIABLE) {
        init = parse_variable_decl(p);
    } else if (tok->type == TOKEN_IDENTIFIER) {
        ASTNode* expr = parse_expression(p);
        if (expr && expr->type == NODE_IDENTIFIER &&
            parser_check(p, TOKEN_ASSIGN)) {
            init = parse_assignment(p, expr);
        } else {
            init = expr;
        }
    }

    parser_expect(p, TOKEN_SEMICOLON, "متوقع '؛' بعد بداية الحلقة");

    ASTNode* cond = NULL;
    if (!parser_check(p, TOKEN_SEMICOLON)) {
        cond = parse_expression(p);
    }

    parser_expect(p, TOKEN_SEMICOLON, "متوقع '؛' بعد شرط الحلقة");

    ASTNode* update = NULL;
    if (!parser_check(p, TOKEN_RPAREN)) {
        update = parse_expression(p);
    }

    parser_expect(p, TOKEN_RPAREN, "متوقع ')' بعد تعبيرات الحلقة");
    parser_expect(p, TOKEN_COLON_ARROW, "متوقع ':-' بعد 'لكل'");

    ASTNode* body = parse_block_inner(p);
    parser_expect(p, TOKEN_KEYWORD_END, "متوقع 'نهاية' لإغلاق 'لكل'");

    ASTNode* node = parser_create_node(NODE_FOR_STATEMENT, for_tok);
    node->as.for_stmt.init = init;
    node->as.for_stmt.condition = cond;
    node->as.for_stmt.update = update;
    node->as.for_stmt.body = body;
    return node;
}

/* ===== التحليل: ارجع ===== */

static ASTNode* parse_return_stmt(Parser* p) {
    Token* ret_tok = parser_advance(p);
    ASTNode* value = NULL;

    if (!parser_check(p, TOKEN_NEWLINE) &&
        !parser_check(p, TOKEN_KEYWORD_END) &&
        !parser_check(p, TOKEN_EOF) &&
        !parser_check(p, TOKEN_SEMICOLON)) {
        value = parse_expression(p);
    }

    parser_match(p, TOKEN_SEMICOLON);

    ASTNode* node = parser_create_node(NODE_RETURN_STATEMENT, ret_tok);
    node->as.return_stmt.value = value;
    return node;
}

/* ===== التحليل: التعيين ===== */

static ASTNode* parse_assignment(Parser* p, ASTNode* left) {
    if (!left || (left->type != NODE_IDENTIFIER && left->type != NODE_INDEX_EXPRESSION && left->type != NODE_MEMBER_EXPRESSION)) {
        parser_report_error(p, "P001", "جهة يسار التعيين يجب أن تكون معرفاً أو فهرسة أو عضو");
        parser_synchronize(p);
        return left ? left : parser_create_node(NODE_IDENTIFIER, parser_current(p));
    }

    Token* op_tok = parser_advance(p);
    AssignmentOperator op;
    switch (op_tok->type) {
        case TOKEN_ASSIGN:              op = ASSIGN; break;
        case TOKEN_PLUS_ASSIGN:         op = ASSIGN_ADD; break;
        case TOKEN_MINUS_ASSIGN:        op = ASSIGN_SUB; break;
        case TOKEN_STAR_ASSIGN:         op = ASSIGN_MUL; break;
        case TOKEN_SLASH_ASSIGN:        op = ASSIGN_DIV; break;
        case TOKEN_PERCENT_ASSIGN:      op = ASSIGN_MOD; break;
        case TOKEN_SHIFT_LEFT_ASSIGN:   op = ASSIGN_SHIFT_LEFT; break;
        case TOKEN_SHIFT_RIGHT_ASSIGN:  op = ASSIGN_SHIFT_RIGHT; break;
        default:
            parser_report_error(p, "P001", "عملية تعيين غير صالحة");
            return left;
    }

    ASTNode* value = parse_expression(p);
    parser_match(p, TOKEN_SEMICOLON);

    ASTNode* node = parser_create_node(NODE_ASSIGNMENT_EXPRESSION, op_tok);
    node->as.assignment.target = left;
    node->as.assignment.op = op;
    node->as.assignment.value = value;
    return node;
}

/* ===== التحليل: التعبيرات (Pratt Parser) ===== */

static int get_precedence(TokenType type) {
    switch (type) {
        case TOKEN_ASSIGN:
        case TOKEN_PLUS_ASSIGN:
        case TOKEN_MINUS_ASSIGN:
        case TOKEN_STAR_ASSIGN:
        case TOKEN_SLASH_ASSIGN:
        case TOKEN_PERCENT_ASSIGN:
        case TOKEN_SHIFT_LEFT_ASSIGN:
        case TOKEN_SHIFT_RIGHT_ASSIGN:
            return 1;

        case TOKEN_OR:
        case TOKEN_KEYWORD_OR:
            return 2;

        case TOKEN_AND:
        case TOKEN_KEYWORD_AND:
            return 3;

        case TOKEN_BITWISE_OR:
        case TOKEN_KEYWORD_BITWISE_OR:
            return 4;

        case TOKEN_BITWISE_XOR:
        case TOKEN_KEYWORD_BITWISE_XOR:
            return 5;

        case TOKEN_BITWISE_AND:
        case TOKEN_KEYWORD_BITWISE_AND:
            return 6;

        case TOKEN_EQUAL:
        case TOKEN_NOT_EQUAL:
        case TOKEN_KEYWORD_EQUALS:
        case TOKEN_KEYWORD_NOT_EQUALS:
            return 7;

        case TOKEN_GREATER:
        case TOKEN_LESS:
        case TOKEN_GREATER_EQUAL:
        case TOKEN_LESS_EQUAL:
        case TOKEN_KEYWORD_GREATER:
        case TOKEN_KEYWORD_LESS:
        case TOKEN_KEYWORD_GREATER_EQUALS:
        case TOKEN_KEYWORD_LESS_EQUALS:
            return 8;

        case TOKEN_SHIFT_LEFT:
        case TOKEN_SHIFT_RIGHT:
        case TOKEN_KEYWORD_SHIFT_LEFT:
        case TOKEN_KEYWORD_SHIFT_RIGHT:
            return 9;

        case TOKEN_PLUS:
        case TOKEN_MINUS:
        case TOKEN_KEYWORD_ADD:
        case TOKEN_KEYWORD_SUB:
            return 10;

        case TOKEN_STAR:
        case TOKEN_SLASH:
        case TOKEN_PERCENT:
        case TOKEN_KEYWORD_MUL:
        case TOKEN_KEYWORD_DIV:
        case TOKEN_KEYWORD_MOD:
            return 11;

        default:
            return 0;
    }
}

static int is_assignment_op(TokenType type) {
    return get_precedence(type) == 1;
}

static BinaryOperator get_binary_op(TokenType type) {
    switch (type) {
        case TOKEN_PLUS:  case TOKEN_KEYWORD_ADD:   return OP_ADD;
        case TOKEN_MINUS: case TOKEN_KEYWORD_SUB:   return OP_SUB;
        case TOKEN_STAR:  case TOKEN_KEYWORD_MUL:   return OP_MUL;
        case TOKEN_SLASH: case TOKEN_KEYWORD_DIV:   return OP_DIV;
        case TOKEN_PERCENT: case TOKEN_KEYWORD_MOD: return OP_MOD;
        case TOKEN_AND:   case TOKEN_KEYWORD_AND:   return OP_AND;
        case TOKEN_OR:    case TOKEN_KEYWORD_OR:    return OP_OR;
        case TOKEN_BITWISE_AND: case TOKEN_KEYWORD_BITWISE_AND: return OP_BITWISE_AND;
        case TOKEN_BITWISE_OR:  case TOKEN_KEYWORD_BITWISE_OR:  return OP_BITWISE_OR;
        case TOKEN_BITWISE_XOR: case TOKEN_KEYWORD_BITWISE_XOR: return OP_BITWISE_XOR;
        case TOKEN_SHIFT_LEFT:  case TOKEN_KEYWORD_SHIFT_LEFT:  return OP_SHIFT_LEFT;
        case TOKEN_SHIFT_RIGHT: case TOKEN_KEYWORD_SHIFT_RIGHT: return OP_SHIFT_RIGHT;
        case TOKEN_EQUAL:  case TOKEN_KEYWORD_EQUALS:  return OP_EQUAL;
        case TOKEN_NOT_EQUAL: case TOKEN_KEYWORD_NOT_EQUALS: return OP_NOT_EQUAL;
        case TOKEN_GREATER: case TOKEN_KEYWORD_GREATER: return OP_GREATER;
        case TOKEN_LESS:    case TOKEN_KEYWORD_LESS:    return OP_LESS;
        case TOKEN_GREATER_EQUAL: case TOKEN_KEYWORD_GREATER_EQUALS: return OP_GREATER_EQUAL;
        case TOKEN_LESS_EQUAL:    case TOKEN_KEYWORD_LESS_EQUALS:    return OP_LESS_EQUAL;
        default: return -1;
    }
}

static ASTNode* parse_expression(Parser* p) {
    return parse_precedence(p, 1);
}

static ASTNode* parse_precedence(Parser* p, int min_prec) {
    ASTNode* left = parse_unary(p);
    if (!left) return NULL;

    for (;;) {
        Token* op_tok = parser_current(p);
        int prec = get_precedence(op_tok->type);

        if (prec == 0 || prec < min_prec) break;

        if (is_assignment_op(op_tok->type)) {
            if (left->type == NODE_IDENTIFIER || left->type == NODE_INDEX_EXPRESSION || left->type == NODE_MEMBER_EXPRESSION) {
                left = parse_assignment(p, left);
                continue;
            }
            parser_report_error(p, "P001", "جهة يسار التعيين يجب أن تكون معرفاً أو فهرسة أو عضو");
            parser_synchronize(p);
            break;
        }

        parser_advance(p);
        BinaryOperator op = get_binary_op(op_tok->type);
        if (op == (BinaryOperator)-1) {
            parser_report_error(p, "P010", "عملية ثنائية غير معروفة");
            parser_synchronize(p);
            break;
        }

        ASTNode* right = parse_precedence(p, prec + 1);

        ASTNode* node = parser_create_node(NODE_BINARY_EXPRESSION, op_tok);
        node->as.binary.op = op;
        node->as.binary.left = left;
        node->as.binary.right = right;
        left = node;
    }

    return left;
}

/* ===== التحليل: أحادية ===== */

static ASTNode* parse_unary(Parser* p) {
    Token* tok = parser_current(p);

    switch (tok->type) {
        case TOKEN_NOT:
        case TOKEN_KEYWORD_NOT: {
            parser_advance(p);
            ASTNode* operand = parse_unary(p);
            ASTNode* node = parser_create_node(NODE_UNARY_EXPRESSION, tok);
            node->as.unary.op = UNARY_NOT;
            node->as.unary.operand = operand;
            return node;
        }
        case TOKEN_BITWISE_NOT:
        case TOKEN_KEYWORD_BITWISE_NOT: {
            parser_advance(p);
            ASTNode* operand = parse_unary(p);
            ASTNode* node = parser_create_node(NODE_UNARY_EXPRESSION, tok);
            node->as.unary.op = UNARY_BITWISE_NOT;
            node->as.unary.operand = operand;
            return node;
        }
        case TOKEN_MINUS: {
            parser_advance(p);
            ASTNode* operand = parse_unary(p);
            ASTNode* node = parser_create_node(NODE_UNARY_EXPRESSION, tok);
            node->as.unary.op = UNARY_NEGATE;
            node->as.unary.operand = operand;
            return node;
        }
        /* Phase 12.5: &expr — address-of (unary) */
        case TOKEN_BITWISE_AND:
        case TOKEN_KEYWORD_BITWISE_AND: {
            parser_advance(p);
            ASTNode* operand = parse_unary(p);
            ASTNode* node = parser_create_node(NODE_UNARY_EXPRESSION, tok);
            node->as.unary.op = UNARY_ADDRESS_OF;
            node->as.unary.operand = operand;
            return node;
        }
        /* Phase 12.5: *expr — pointer dereference (unary) */
        case TOKEN_STAR: {
            parser_advance(p);
            ASTNode* operand = parse_unary(p);
            ASTNode* node = parser_create_node(NODE_UNARY_EXPRESSION, tok);
            node->as.unary.op = UNARY_DEREF;
            node->as.unary.operand = operand;
            return node;
        }
        default:
            return parse_postfix(p);
    }
}

/* ===== التحليل: لاحقي ===== */

static ASTNode* parse_postfix(Parser* p) {
    ASTNode* node = parse_primary(p);
    if (!node) return NULL;

    for (;;) {
        Token* tok = parser_current(p);

        if (tok->type == TOKEN_LPAREN) {
            parser_advance(p);
            ASTNodeList args = parse_arg_list(p);
            parser_expect(p, TOKEN_RPAREN, "متوقع ')' بعد قائمة المعاملات");

            ASTNode* call = parser_create_node(NODE_CALL_EXPRESSION, tok);
            call->as.call.callee = node;
            call->as.call.args = args;
            node = call;
            continue;
        }

        if (tok->type == TOKEN_LBRACKET) {
            parser_advance(p);
            ASTNode* index = parse_expression(p);
            parser_expect(p, TOKEN_RBRACKET, "متوقع ']' بعد الفهرس");

            ASTNode* idx = parser_create_node(NODE_INDEX_EXPRESSION, tok);
            idx->as.index.array = node;
            idx->as.index.index = index;
            node = idx;
            continue;
        }

        if (tok->type == TOKEN_DOT) {
            parser_advance(p);
            Token* member_tok = parser_expect(p, TOKEN_IDENTIFIER, "متوقع اسم العضو");

            ASTNode* mem = parser_create_node(NODE_MEMBER_EXPRESSION, tok);
            mem->as.member.object = node;
            mem->as.member.member = strdup(member_tok->value);
            node = mem;
            continue;
        }

        break;
    }

    return node;
}

/* ===== التحليل: أساسي ===== */

static ASTNode* parse_primary(Parser* p) {
    Token* tok = parser_current(p);

    if (tok->type == TOKEN_INTEGER) {
        ASTNode* node = parser_create_node(NODE_LITERAL, tok);
        node->as.literal.kind = LITERAL_INT;
        node->as.literal.as.int_value = tok->literal.int_value;
        parser_advance(p);
        return node;
    }

    if (tok->type == TOKEN_FLOAT) {
        ASTNode* node = parser_create_node(NODE_LITERAL, tok);
        node->as.literal.kind = LITERAL_FLOAT;
        node->as.literal.as.float_value = tok->literal.float_value;
        parser_advance(p);
        return node;
    }

    if (tok->type == TOKEN_STRING) {
        ASTNode* node = parser_create_node(NODE_LITERAL, tok);
        node->as.literal.kind = LITERAL_STRING;
        node->as.literal.as.string_value = strdup(tok->value);
        parser_advance(p);
        return node;
    }

    if (tok->type == TOKEN_CHARACTER) {
        ASTNode* node = parser_create_node(NODE_LITERAL, tok);
        node->as.literal.kind = LITERAL_CHAR;
        node->as.literal.as.char_value = tok->value ? tok->value[0] : '\0';
        parser_advance(p);
        return node;
    }

    if (tok->type == TOKEN_KEYWORD_TRUE) {
        ASTNode* node = parser_create_node(NODE_LITERAL, tok);
        node->as.literal.kind = LITERAL_BOOL;
        node->as.literal.as.bool_value = 1;
        parser_advance(p);
        return node;
    }

    if (tok->type == TOKEN_KEYWORD_FALSE) {
        ASTNode* node = parser_create_node(NODE_LITERAL, tok);
        node->as.literal.kind = LITERAL_BOOL;
        node->as.literal.as.bool_value = 0;
        parser_advance(p);
        return node;
    }

    if (tok->type == TOKEN_KEYWORD_NULL) {
        ASTNode* node = parser_create_node(NODE_LITERAL, tok);
        node->as.literal.kind = LITERAL_INT;
        node->as.literal.as.int_value = 0;
        parser_advance(p);
        return node;
    }

    if (tok->type == TOKEN_IDENTIFIER) {
        ASTNode* node = parser_create_node(NODE_IDENTIFIER, tok);
        node->as.identifier.name = strdup(tok->value);
        parser_advance(p);
        return node;
    }

    if (tok->type == TOKEN_LPAREN) {
        parser_advance(p);
        ASTNode* expr = parse_expression(p);
        parser_expect(p, TOKEN_RPAREN, "متوقع ')' لإغلاق التعبير");
        return expr;
    }

    /* حوّل(Type, Expr) */
    if (tok->type == TOKEN_KEYWORD_CAST) {
        parser_advance(p);
        parser_expect(p, TOKEN_LPAREN, "متوقع '(' بعد 'حوّل'");
        ASTNode* type_node = parse_type(p);
        parser_expect(p, TOKEN_COMMA, "متوقع ',' بعد النوع");
        ASTNode* expr = parse_expression(p);
        parser_expect(p, TOKEN_RPAREN, "متوقع ')' بعد التعبير");

        ASTNode* node = parser_create_node(NODE_CAST_EXPRESSION, tok);
        node->as.cast.type_node = type_node;
        node->as.cast.expr = expr;
        return node;
    }

    /* حجم_النوع(Type) */
    if (tok->type == TOKEN_KEYWORD_SIZEOF_TYPE) {
        parser_advance(p);
        parser_expect(p, TOKEN_LPAREN, "متوقع '(' بعد 'حجم_النوع'");
        ASTNode* type_node = parse_type(p);
        parser_expect(p, TOKEN_RPAREN, "متوقع ')' بعد النوع");

        ASTNode* node = parser_create_node(NODE_CALL_EXPRESSION, tok);
        node->as.call.callee = parser_create_node(NODE_IDENTIFIER, tok);
        node->as.call.callee->as.identifier.name = strdup("حجم_النوع");
        ast_node_list_init(&node->as.call.args);
        ast_node_list_add(&node->as.call.args, type_node);
        return node;
    }

    /* حجم_القيمة(Expr) */
    if (tok->type == TOKEN_KEYWORD_SIZEOF_VALUE) {
        parser_advance(p);
        parser_expect(p, TOKEN_LPAREN, "متوقع '(' بعد 'حجم_القيمة'");
        ASTNode* expr = parse_expression(p);
        parser_expect(p, TOKEN_RPAREN, "متوقع ')' بعد التعبير");

        ASTNode* node = parser_create_node(NODE_CALL_EXPRESSION, tok);
        node->as.call.callee = parser_create_node(NODE_IDENTIFIER, tok);
        node->as.call.callee->as.identifier.name = strdup("حجم_القيمة");
        ast_node_list_init(&node->as.call.args);
        ast_node_list_add(&node->as.call.args, expr);
        return node;
    }

    /* Built-in function calls: احجز، حرر، غيّر_حجم، ادخل */
    if (tok->type == TOKEN_KEYWORD_ALLOC || tok->type == TOKEN_KEYWORD_FREE ||
        tok->type == TOKEN_KEYWORD_RESIZE || tok->type == TOKEN_KEYWORD_INPUT) {
        parser_advance(p);
        parser_expect(p, TOKEN_LPAREN, "متوقع '(' بعد دالة مدمجة");
        ASTNodeList args = parse_arg_list(p);
        parser_expect(p, TOKEN_RPAREN, "متوقع ')' بعد قائمة المعاملات");

        ASTNode* node = parser_create_node(NODE_CALL_EXPRESSION, tok);
        node->as.call.callee = parser_create_node(NODE_IDENTIFIER, tok);
        if (tok->type == TOKEN_KEYWORD_ALLOC) node->as.call.callee->as.identifier.name = strdup("احجز");
        else if (tok->type == TOKEN_KEYWORD_FREE) node->as.call.callee->as.identifier.name = strdup("حرر");
        else if (tok->type == TOKEN_KEYWORD_RESIZE) node->as.call.callee->as.identifier.name = strdup("غيّر_حجم");
        else node->as.call.callee->as.identifier.name = strdup("ادخل");
        node->as.call.args = args;
        return node;
    }

    /* {} - مصفوفة أو هيكل */
    if (tok->type == TOKEN_LBRACE) {
        parser_advance(p);

        /* هل مصفوفة فارغة؟ */
        if (parser_check(p, TOKEN_RBRACE)) {
            ASTNode* node = parser_create_node(NODE_ARRAY_EXPRESSION, tok);
            ast_node_list_init(&node->as.array.elements);
            parser_advance(p);
            return node;
        }

        /* نحفظ الموقع ونحاول فحص: هل هو هيكل (name: value)؟ */
        int saved = p->current;
        if (parser_check(p, TOKEN_IDENTIFIER)) {
            parser_advance(p);
            if (parser_check(p, TOKEN_COLON)) {
                /* هيكل */
                p->current = saved;
                ASTNodeList names;
                ASTNodeList values;
                ast_node_list_init(&names);
                ast_node_list_init(&values);

                do {
                    Token* ntok = parser_expect(p, TOKEN_IDENTIFIER, "متوقع اسم العضو");
                    parser_expect(p, TOKEN_COLON, "متوقع ':' بعد اسم العضو");
                    ASTNode* val = parse_expression(p);
                    ast_node_list_add(&names, parser_create_node(NODE_IDENTIFIER, ntok));
                    ast_node_list_add(&values, val);
                } while (parser_match(p, TOKEN_COMMA) &&
                         !parser_check(p, TOKEN_RBRACE));

                ASTNode* snode = parser_create_node(NODE_STRUCT_EXPRESSION, tok);
                snode->as.struct_expr.names = names;
                snode->as.struct_expr.values = values;
                parser_expect(p, TOKEN_RBRACE, "متوقع '}' لإغلاق الهيكل");
                return snode;
            }
        }

        /* مصفوفة */
        p->current = saved;
        ASTNodeList elems;
        ast_node_list_init(&elems);

        if (!parser_check(p, TOKEN_RBRACE)) {
            do {
                ASTNode* elem = parse_expression(p);
                if (elem) ast_node_list_add(&elems, elem);
            } while (parser_match(p, TOKEN_COMMA));
        }

        ASTNode* anode = parser_create_node(NODE_ARRAY_EXPRESSION, tok);
        anode->as.array.elements = elems;
        parser_expect(p, TOKEN_RBRACE, "متوقع '}' لإغلاق المصفوفة");
        return anode;
    }

    if (tok->type == TOKEN_EOF) {
        return NULL;
    }

    parser_report_error(p, "P001", "تعبير غير متوقع");
    parser_synchronize(p);
    return NULL;
}

/* ===== الواجهة العامة ===== */

Parser* parser_create(Token* tokens, int token_count, const char* filename) {
    Parser* p = (Parser*)calloc(1, sizeof(Parser));
    if (!p) return NULL;

    p->tokens = tokens;
    p->token_count = token_count;
    p->current = 0;
    p->filename = filename ? strdup(filename) : strdup("<stdin>");
    p->had_error = 0;
    p->panic_mode = 0;

    p->errors = (ParserErrorList*)malloc(sizeof(ParserErrorList));
    if (p->errors) {
        parser_error_list_init(p->errors);
    }

    return p;
}

void parser_destroy(Parser* parser) {
    if (!parser) return;

    if (parser->errors) {
        parser_error_list_destroy(parser->errors);
        free(parser->errors);
    }

    if (parser->filename) {
        free((void*)parser->filename);
    }

    free(parser);
}

ASTNode* parser_parse(Parser* parser) {
    if (!parser) return NULL;
    return parse_program(parser);
}

void parser_parse_errors(Parser* parser, FILE* file) {
    if (!parser || !parser->errors || !file) return;

    for (int i = 0; i < parser->errors->count; i++) {
        parser_error_print(parser->errors->errors[i], file);
    }
}

int parser_has_errors(Parser* parser) {
    return parser ? parser->had_error : 0;
}
