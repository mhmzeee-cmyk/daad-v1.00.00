# Lexer - المحلل البصري لغة ض Core

## نظرة عامة

الLexer هو المسؤول عن تحويل النص المصدري إلى سلسلة من الـ Tokens. وهو الخطوة الأولى في عملية الترجمة.

## المكونات

```
compiler/
├── token/
│   ├── token_types.h    # تعريف أنواع الـ Tokens
│   ├── token.h          # بنية الـ Token
│   └── token.c          # تنفيذ دوال Token
│
├── lexer/
│   ├── lexer.h          # واجهة الـ Lexer
│   ├── lexer.c          # تنفيذ الـ Lexer
│   ├── unicode.h        # أدوات Unicode
│   ├── unicode.c        # تنفيذ Unicode
│   ├── keywords.h       # الكلمات المفتاحية
│   └── keywords.c       # تنفيذ البحث
│
└── tests/
    └── test_lexer.c     # اختبارات الـ Lexer
```

## تدفق البيانات

```
النص المصدري (.dad)
        │
        ▼
┌─────────────────────────────────────────┐
│               Lexer                     │
│                                         │
│  1. قراءة الحروف                        │
│  2. تحليل Unicode                       │
│  3. تجميع الـ Tokens                    │
│  4. التحقق من الكلمات المفتاحية        │
│  5. اكتشاف الأخطاء                     │
│                                         │
└─────────────────────────────────────────┘
        │
        ▼
قائمة من الـ Tokens
```

## أنواع الـ Tokens

### كلمات مفتاحية
- `TOKEN_KEYWORD_VARIABLE` → متغير
- `TOKEN_KEYWORD_FUNCTION` → دالة
- `TOKEN_KEYWORD_IF` → إذا
- `TOKEN_KEYWORD_INT` → رقم
- والمزيد...

### قيم
- `TOKEN_INTEGER` → أعداد صحيحة (123, 0xFF, 0b1010)
- `TOKEN_FLOAT` → أعداد عشرية (3.14, 1.0e10)
- `TOKEN_STRING` → نصوص ("مرحباً")
- `TOKEN_CHARACTER` → حروف ('أ')

### معرفات
- `TOKEN_IDENTIFIER` → أسماء المتغيرات والدوال

### معاملات
- `TOKEN_PLUS` → +
- `TOKEN_MINUS` → -
- `TOKEN_ASSIGN` → =
- `TOKEN_EQUAL` → ==
- والمزيد...

### فواصل
- `TOKEN_COLON` → :
- `TOKEN_LPAREN` → (
- `TOKEN_RPAREN` → )
- `TOKEN_LBRACE` → {
- `TOKEN_RBRACE` → }
- والمزيد...

### خاص
- `TOKEN_NEWLINE` → نهاية سطر
- `TOKEN_EOF` → نهاية ملف
- `TOKEN_ERROR` → خطأ

## استخدام الـ Lexer

### إنشاء Lexer

```c
#include "lexer/lexer.h"

// من نص
Lexer* lexer = lexer_create("متغير: عدد = 10", "test.dad");

// من ملف
FILE* file = fopen("test.dad", "r");
Lexer* lexer = lexer_create_from_file(file, "test.dad");
```

### قراءة الـ Tokens

```c
// قراءة token واحدة
Token token = lexer_next_token(lexer);

// الاطلاع على التالية بدون أخذها
Token next = lexer_peek_token(lexer);

// تحليل الكل
size_t count;
Token* tokens = lexer_tokenize(lexer, &count);
```

### التحقق من الأخطاء

```c
if (lexer_has_errors(lexer)) {
    lexer_print_errors(lexer, stderr);
}
```

### تحرير الذاكرة

```c
lexer_destroy(lexer);
token_destroy(&token);
```

## دعم Unicode

يدعم الـ Lexer:
- العربية بالكامل
- الإنجليزية
- الأرقام العربية-الهندية (٠-٩)
- الترميز UTF-8

## إدارة الأخطاء

لا ينهار الـ Lexer عند أول خطأ. يُكمل التحليل ويجمع الأخطاء.

| الكود | الوصف |
|-------|-------|
| L001 | معرف غير معروف |
| L002 | خطأ في الترميز |
| L003 | نص غير مكتمل |
| L004 | رقم غير صالح |
| L005 | تعليق غير مكتمل |
| L006 | رمز غير معروف |

## تشغيل الاختبارات

```bash
# التجميع
gcc -o test_lexer tests/test_lexer.c lexer/lexer.c lexer/unicode.c lexer/keywords.c token/token.c -I.

# التشغيل
./test_lexer
```

## الملفات المُستخدمة

- `docs/keywords.md` - الكلمات المفتاحية
- `docs/grammar.md` - قواعد اللغة
- `docs/error-codes.md` - أكواد الأخطاء

---

**الإصدار:** 0.1.0
**التاريخ:** 2026-08-03
