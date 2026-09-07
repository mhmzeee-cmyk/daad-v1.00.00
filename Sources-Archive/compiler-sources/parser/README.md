# Parser - المحلل النحوي لغة ض Core

**الإصدار:** 0.1.0
**التاريخ:** 2026-08-03

---

## نظرة عامة

المحلل النحوي (Parser) مسؤول عن تحويل سلسلة الـ Tokens إلى شجرة AST (Abstract Syntax Tree). يستخدم نمط **Pratt Parser** (أولوية تسلسلية) لمعالجة التعبيرات.

## الهيكل

```
parser/
├── parser.h            # الواجهة العامة
├── parser.c            # التنفيذ الرئيسي
├── parser_internal.h   # التعريفات الداخلية
├── parser_error.h      # تعريفات الأخطاء
├── parser_error.c      # تنفيذ الأخطاء
├── parser_recovery.c   # استرداد الأخطاء (Panic Mode)
└── README.md           # هذا الملف
```

## الواجهة العامة

```c
// إنشاء محلل
Parser* parser_create(Token* tokens, int token_count, const char* filename);

// تحليل
ASTNode* parser_parse(Parser* parser);

// التحقق من أخطاء
int parser_has_errors(Parser* parser);

// طباعة الأخطاء
void parser_parse_errors(Parser* parser, FILE* file);

// تحرير الذاكرة
void parser_destroy(Parser* parser);
```

## الأولويات (12 مستوى)

| الأولوية | العمليات |
|----------|----------|
| 1 (أدنى) | التعيين: `= += -= *= /= %= <<= >>=` |
| 2 | `أو` |
| 3 | `و` |
| 4 | `أو_بتية` / `\|` |
| 5 | `تبادل_بتية` / `^` |
| 6 | `و_بتية` / `&` |
| 7 | المساواة: `== != يساوي لا_يساوي` |
| 8 | المقارنة: `> < >= <= أكبر أصغر` |
| 9 | التحريك: `<< >> تحريك_يسار تحريك_يمين` |
| 10 | الجمع والطرح: `+ - اجمع اطرح` |
| 11 | الضرب والقسمة: `* / % اضرب اقسم باقي` |
| 12 (أعلى) | الأحادية: `! ~ ليس ليس_بتية -` |

## أكواد الأخطاء

| الكود | الوصف |
|-------|-------|
| P001 | علامة ناقصة |
| P002 | جملة غير مكتملة |
| P003 | دالة غير مكتملة |
| P004 | متغير غير معروف |
| P005 | نوع غير معروف |

## استخدامه

```c
#include "lexer/lexer.h"
#include "parser/parser.h"

// تحليل المصدري إلى Tokens
Lexer* lexer = lexer_create(source, "program.dad");
size_t count;
Token* tokens = lexer_tokenize(lexer, &count);

// تحليل Tokens إلى AST
Parser* parser = parser_create(tokens, (int)count, "program.dad");
ASTNode* ast = parser_parse(parser);

if (parser_has_errors(parser)) {
    parser_parse_errors(parser, stderr);
} else {
    // استخدام AST...
}

parser_destroy(parser);
lexer_destroy(lexer);
```
