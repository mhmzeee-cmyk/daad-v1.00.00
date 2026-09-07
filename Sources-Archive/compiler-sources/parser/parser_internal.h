/**
 * @file parser_internal.h
 * @brief تعريفات داخلية للمحلل النحوي (Parser)
 *
 * @version 0.1.0
 * @date 2026-08-03
 */

#ifndef DAAD_PARSER_INTERNAL_H
#define DAAD_PARSER_INTERNAL_H

#include "../token/token.h"
#include "parser_error.h"
#include "../ast/ast_node.h"
#include "../ast/ast.h"

/**
 * @brief بنية المحلل النحوي
 */
struct Parser {
    Token* tokens;          /* مصفوفة الـ Tokens */
    int token_count;        /* عدد الـ Tokens */
    int current;            /* الموقع الحالي */
    const char* filename;   /* اسم الملف */
    ParserErrorList* errors;/* قائمة الأخطاء */
    int had_error;          /* هل حدث خطأ؟ */
    int panic_mode;         /* وضع الذعر (Panic Mode) */
};

typedef struct Parser Parser;

/* ===== دوال مساعدة ===== */

/**
 * @brief الحصول على الـ Token الحالية
 */
Token* parser_current(Parser* p);

/**
 * @brief الاطلاع على الـ Token التالية بدون أخذها
 */
Token* parser_peek(Parser* p);

/**
 * @brief التقدم للـ Token التالية وإرجاعها
 */
Token* parser_advance(Parser* p);

/**
 * @brief التحقق من نوع الـ Token الحالية
 */
int parser_check(Parser* p, TokenType type);

/**
 * @brief مطابقة الـ Token الحالية مع نوع محدد
 *        إذا تطابقت تقدم، وإلا لا
 */
int parser_match(Parser* p, TokenType type);

/**
 * @brief مطابقة الـ Token مع نوع وقيمة محددين
 */
int parser_match_value(Parser* p, TokenType type, const char* value);

/**
 * @brief التأكد من وجود Token من نوع محدد
 *        إذا لم توجد أبلغ عن خطأ
 */
Token* parser_expect(Parser* p, TokenType type, const char* error_msg);

/**
 * @brief التقدم وتجاوز الـ Tokens المختلقة
 */
void parser_synchronize(Parser* p);

/**
 * @brief التحقق من نهاية الملف
 */
int parser_is_at_end(Parser* p);

/**
 * @brief إنشاء عقدة AST من Token (يضبط الموقع تلقائياً)
 */
ASTNode* parser_create_node(NodeType type, const Token* tok);

#endif /* DAAD_PARSER_INTERNAL_H */
