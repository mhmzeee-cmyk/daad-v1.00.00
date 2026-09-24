/**
 * @file parser.h
 * @brief واجهة المحلل النحوي (Parser) لغة ض Core
 *
 * @version 0.1.0
 * @date 2026-08-03
 */

#ifndef DAAD_PARSER_H
#define DAAD_PARSER_H

#include "../ast/ast_node.h"
#include "../token/token.h"
#include "parser_error.h"
#include <stdio.h>

/* Parser is an opaque type - defined in parser_internal.h */
typedef struct Parser Parser;

/**
 * @brief إنشاء محلل نحوي جديد
 * @param tokens مصفوفة الـ Tokens
 * @param token_count عدد الـ Tokens
 * @param filename اسم الملف
 * @return مؤشر إلى المحلل الجديد
 */
Parser* parser_create(Token* tokens, int token_count, const char* filename);

/**
 * @brief تحرير ذاكرة المحلل
 * @param parser مؤشر إلى المحلل
 */
void parser_destroy(Parser* parser);

/**
 * @brief تحليل جميع الـ Tokens وإرجاع شجرة AST
 * @param parser مؤشر إلى المحلل
 * @return عقدة البرنامج (NODE_PROGRAM)
 */
ASTNode* parser_parse(Parser* parser);

/**
 * @brief طباعة جميع أخطاء التحليل
 * @param parser مؤشر إلى المحلل
 * @param file ملف الإخراج
 */
void parser_parse_errors(Parser* parser, FILE* file);

/**
 * @brief التحقق من وجود أخطاء
 * @param parser مؤشر إلى المحلل
 * @return 1 إذا وُجدت أخطاء، 0 خلاف ذلك
 */
int parser_has_errors(Parser* parser);

#endif /* DAAD_PARSER_H */
