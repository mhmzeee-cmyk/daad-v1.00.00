/**
 * @file test_lexer.c
 * @brief اختبارات Lexer
 * 
 * @version 0.1.0
 * @date 2026-08-03
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer/lexer.h"
#include "token/token.h"

/* عدد الاختبارات */
static int tests_passed = 0;
static int tests_failed = 0;

/**
 * @brief طباعة نتيجة الاختبار
 */
static void assert_test(const char* test_name, int condition) {
    if (condition) {
        printf("   %s\n", test_name);
        tests_passed++;
    } else {
        printf("   %s\n", test_name);
        tests_failed++;
    }
}

/**
 * @brief اختبار الكلمات المفتاحية
 */
static void test_keywords(void) {
    printf("\n=== اختبار الكلمات المفتاحية ===\n");
    
    const char* keywords[] = {
        "متغير", "ثابت", "نوع", "دالة", "إذا", "وإلا",
        "طالما", "لكل", "ارجع", "نهاية", "رقم", "رقم_عشري",
        "حرف", "منطق", "فراغ", "نص", "صحيح", "خطأ",
        NULL
    };
    
    for (int i = 0; keywords[i] != NULL; i++) {
        Lexer* lexer = lexer_create(keywords[i], "test");
        Token token = lexer_next_token(lexer);
        
        char test_name[256];
        snprintf(test_name, sizeof(test_name), "الكلمة المفتاحية: %s", keywords[i]);
        assert_test(test_name, token.type != TOKEN_IDENTIFIER && token.type != TOKEN_ERROR);
        
        token_destroy(&token);
        lexer_destroy(lexer);
    }
}

/**
 * @brief اختبار المعرفات
 */
static void test_identifiers(void) {
    printf("\n=== اختبار المعرفات ===\n");
    
    const char* identifiers[] = {
        "أحمد", "محمد", "المتغير", "عدد1", "myVar", "_test",
        NULL
    };
    
    for (int i = 0; identifiers[i] != NULL; i++) {
        Lexer* lexer = lexer_create(identifiers[i], "test");
        Token token = lexer_next_token(lexer);
        
        char test_name[256];
        snprintf(test_name, sizeof(test_name), "المعرف: %s", identifiers[i]);
        assert_test(test_name, token.type == TOKEN_IDENTIFIER);
        
        token_destroy(&token);
        lexer_destroy(lexer);
    }
}

/**
 * @brief اختبار الأرقام
 */
static void test_numbers(void) {
    printf("\n=== اختبار الأرقام ===\n");
    
    /* أرقام عشرية */
    {
        Lexer* lexer = lexer_create("123", "test");
        Token token = lexer_next_token(lexer);
        assert_test("عدد صحيح: 123", token.type == TOKEN_INTEGER);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* أرقام hex */
    {
        Lexer* lexer = lexer_create("0xFF", "test");
        Token token = lexer_next_token(lexer);
        assert_test("عدد hex: 0xFF", token.type == TOKEN_INTEGER);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* أرقام binary */
    {
        Lexer* lexer = lexer_create("0b1010", "test");
        Token token = lexer_next_token(lexer);
        assert_test("عدد binary: 0b1010", token.type == TOKEN_INTEGER);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* أرقام octal */
    {
        Lexer* lexer = lexer_create("0o77", "test");
        Token token = lexer_next_token(lexer);
        assert_test("عدد octal: 0o77", token.type == TOKEN_INTEGER);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* أرقام عشرية */
    {
        Lexer* lexer = lexer_create("3.14", "test");
        Token token = lexer_next_token(lexer);
        assert_test("عدد عشري: 3.14", token.type == TOKEN_FLOAT);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
}

/**
 * @brief اختبار النصوص
 */
static void test_strings(void) {
    printf("\n=== اختبار النصوص ===\n");
    
    /* نص بسيط */
    {
        Lexer* lexer = lexer_create("\"مرحباً\"", "test");
        Token token = lexer_next_token(lexer);
        assert_test("نص بسيط: مرحباً", token.type == TOKEN_STRING);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* نص مع escape */
    {
        Lexer* lexer = lexer_create("\"سطر\\nجديد\"", "test");
        Token token = lexer_next_token(lexer);
        assert_test("نص مع escape", token.type == TOKEN_STRING);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
}

/**
 * @brief اختبار الأحرف
 */
static void test_characters(void) {
    printf("\n=== اختبار الأحرف ===\n");
    
    /* حرف عربي */
    {
        Lexer* lexer = lexer_create("'أ'", "test");
        Token token = lexer_next_token(lexer);
        assert_test("حرف عربي: أ", token.type == TOKEN_CHARACTER);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* حرف إنجليزي */
    {
        Lexer* lexer = lexer_create("'A'", "test");
        Token token = lexer_next_token(lexer);
        assert_test("حرف إنجليزي: A", token.type == TOKEN_CHARACTER);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
}

/**
 * @brief اختبار التعليقات
 */
static void test_comments(void) {
    printf("\n=== اختبار التعليقات ===\n");
    
    /* تعليق سطر واحد */
    {
        Lexer* lexer = lexer_create("# هذا تعليق", "test");
        lexer->skip_comments = 0;
        Token token = lexer_next_token(lexer);
        assert_test("تعليق سطر واحد", token.type == TOKEN_COMMENT_SINGLE);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* تعليق متعدد - يُتجاهل */
    {
        Lexer* lexer = lexer_create("#[ تعليق ]#", "test");
        Token token = lexer_next_token(lexer);
        assert_test("تعليق متعدد (يُتجاهل)", token.type != TOKEN_COMMENT_MULTI_START);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
}

/**
 * @brief اختبار المعاملات
 */
static void test_operators(void) {
    printf("\n=== اختبار المعاملات ===\n");
    
    const char* operators[] = {
        "+", "-", "*", "/", "%",
        "=", "==", "!=", ">", "<", ">=", "<=",
        "+=", "-=", "*=", "/=", "%=",
        "<<", ">>", "<<=", ">>=",
        "&", "|", "^", "~", "&&", "||", "!",
        NULL
    };
    
    TokenType expected[] = {
        TOKEN_PLUS, TOKEN_MINUS, TOKEN_STAR, TOKEN_SLASH, TOKEN_PERCENT,
        TOKEN_ASSIGN, TOKEN_EQUAL, TOKEN_NOT_EQUAL, TOKEN_GREATER, TOKEN_LESS, TOKEN_GREATER_EQUAL, TOKEN_LESS_EQUAL,
        TOKEN_PLUS_ASSIGN, TOKEN_MINUS_ASSIGN, TOKEN_STAR_ASSIGN, TOKEN_SLASH_ASSIGN, TOKEN_PERCENT_ASSIGN,
        TOKEN_SHIFT_LEFT, TOKEN_SHIFT_RIGHT, TOKEN_SHIFT_LEFT_ASSIGN, TOKEN_SHIFT_RIGHT_ASSIGN,
        TOKEN_BITWISE_AND, TOKEN_BITWISE_OR, TOKEN_BITWISE_XOR, TOKEN_BITWISE_NOT, TOKEN_AND, TOKEN_OR, TOKEN_NOT,
    };
    
    for (int i = 0; operators[i] != NULL; i++) {
        Lexer* lexer = lexer_create(operators[i], "test");
        Token token = lexer_next_token(lexer);
        
        char test_name[256];
        snprintf(test_name, sizeof(test_name), "معامل: %s", operators[i]);
        assert_test(test_name, token.type == expected[i]);
        
        token_destroy(&token);
        lexer_destroy(lexer);
    }
}

/**
 * @brief اختبار الفواصل
 */
static void test_delimiters(void) {
    printf("\n=== اختبار الفواصل ===\n");
    
    const char* delimiters[] = {
        "(", ")", "{", "}", "[", "]", ":", ",", ".", "@",
        "->", ":-",
        NULL
    };
    
    TokenType expected[] = {
        TOKEN_LPAREN, TOKEN_RPAREN, TOKEN_LBRACE, TOKEN_RBRACE, TOKEN_LBRACKET, TOKEN_RBRACKET,
        TOKEN_COLON, TOKEN_COMMA, TOKEN_DOT, TOKEN_AT,
        TOKEN_ARROW, TOKEN_COLON_ARROW,
    };
    
    for (int i = 0; delimiters[i] != NULL; i++) {
        Lexer* lexer = lexer_create(delimiters[i], "test");
        Token token = lexer_next_token(lexer);
        
        char test_name[256];
        snprintf(test_name, sizeof(test_name), "فاصلة: %s", delimiters[i]);
        assert_test(test_name, token.type == expected[i]);
        
        token_destroy(&token);
        lexer_destroy(lexer);
    }
}

/**
 * @brief اختبار المسافات والنهايات
 */
static void test_whitespace_newlines(void) {
    printf("\n=== اختبار المسافات والنهايات ===\n");
    
    /* مسافات */
    {
        Lexer* lexer = lexer_create("   ", "test");
        Token token = lexer_next_token(lexer);
        assert_test("مسافات (يُتجاهل)", token.type == TOKEN_EOF);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* نهاية سطر */
    {
        Lexer* lexer = lexer_create("\n", "test");
        lexer->skip_whitespace = 0;
        Token token = lexer_next_token(lexer);
        assert_test("نهاية سطر", token.type == TOKEN_NEWLINE);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
}

/**
 * @brief اختبار EOF
 */
static void test_eof(void) {
    printf("\n=== اختبار EOF ===\n");
    
    Lexer* lexer = lexer_create("", "test");
    Token token = lexer_next_token(lexer);
    assert_test("ملف فارغ = EOF", token.type == TOKEN_EOF);
    token_destroy(&token);
    lexer_destroy(lexer);
}

/**
 * @brief اختبار الأخطاء
 */
static void test_errors(void) {
    printf("\n=== اختبار الأخطاء ===\n");
    
    /* نص غير مكتمل */
    {
        Lexer* lexer = lexer_create("\"نص بدون إغلاق", "test");
        Token token = lexer_next_token(lexer);
        assert_test("خطأ: نص غير مكتمل", token.type == TOKEN_ERROR);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
}

/**
 * @brief اختبار برنامج متكامل
 */
static void test_full_program(void) {
    printf("\n=== اختبار برنامج متكامل ===\n");
    
    const char* source = 
        "ثابت: النهاية = 10\n"
        "\n"
        "دالة: اجمع(أ: رقم، ب: رقم) -> رقم :-\n"
        "    ارجع(أ + ب)\n"
        "نهاية\n"
        "\n"
        "دالة: الرئيسية() :-\n"
        "    متغير: عدد: رقم = 5\n"
        "    متغير: النتيجة = اجمع(العدد، 10)\n"
        "    اطبع(النتيجة)\n"
        "نهاية\n";
    
    Lexer* lexer = lexer_create(source, "test.dad");
    size_t count = 0;
    Token* tokens = lexer_tokenize(lexer, &count);
    
    assert_test("برنامج متكامل: يوجد tokens", tokens != NULL && count > 0);
    assert_test("برنامج متكامل: لا يوجد أخطاء", !lexer_has_errors(lexer));
    
    if (tokens) {
        /* التحقق من وجود EOF في النهاية */
        assert_test("برنامج متكامل: ينتهي بـ EOF", tokens[count - 1].type == TOKEN_EOF);
        
        /* تحرير الـ Tokens */
        for (size_t i = 0; i < count; i++) {
            token_destroy(&tokens[i]);
        }
        free(tokens);
    }
    
    lexer_destroy(lexer);
}

/**
 * @brief اختبار عدد علمي مع إشارة
 */
static void test_edge_cases(void) {
    printf("\n=== اختبار الحالات الصعبة ===\n");
    
    /* سلسلة نصية غير مغلقة */
    {
        Lexer* lexer = lexer_create("\"مرحبا", "test");
        Token token = lexer_next_token(lexer);
        assert_test("سلسلة غير مغلقة: خطأ", token.type == TOKEN_ERROR);
        assert_test("سلسلة غير مغلقة: كود L003", strcmp(token.value, "L003") == 0);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* حرف غير مكتمل */
    {
        Lexer* lexer = lexer_create("'أ", "test");
        Token token = lexer_next_token(lexer);
        assert_test("حرف غير مكتمل: خطأ", token.type == TOKEN_ERROR);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* hex بدون أرقام */
    {
        Lexer* lexer = lexer_create("0x", "test");
        Token token = lexer_next_token(lexer);
        assert_test("hex فارغ: خطأ", token.type == TOKEN_ERROR);
        assert_test("hex فارغ: كود L008", strcmp(token.value, "L008") == 0);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* binary بدون أرقام */
    {
        Lexer* lexer = lexer_create("0b", "test");
        Token token = lexer_next_token(lexer);
        assert_test("binary فارغ: خطأ", token.type == TOKEN_ERROR);
        assert_test("binary فارغ: كود L009", strcmp(token.value, "L009") == 0);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* octal بدون أرقام */
    {
        Lexer* lexer = lexer_create("0o", "test");
        Token token = lexer_next_token(lexer);
        assert_test("octal فارغ: خطأ", token.type == TOKEN_ERROR);
        assert_test("octal فارغ: كود L010", strcmp(token.value, "L010") == 0);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* رقم غير صالح */
    {
        Lexer* lexer = lexer_create("0xZZ", "test");
        Token token = lexer_next_token(lexer);
        assert_test("0xZZ: خطأ", token.type == TOKEN_ERROR);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* عدد عشري متعدد النقاط */
    {
        Lexer* lexer = lexer_create("12.3.4", "test");
        Token token = lexer_next_token(lexer);
        assert_test("12.3.4: عدد عشري فقط", token.type == TOKEN_FLOAT);
        assert_test("12.3.4: القيمة 12.3", strncmp(token.value, "12.3", 4) == 0);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* حرف عربي-هندي */
    {
        Lexer* lexer = lexer_create("١٢٣", "test");
        Token token = lexer_next_token(lexer);
        assert_test("أرقام عربية-هندية", token.type == TOKEN_INTEGER);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* معرف عربي */
    {
        Lexer* lexer = lexer_create("اسم_الطالب", "test");
        Token token = lexer_next_token(lexer);
        assert_test("معرف عربي: اسم_الطالب", token.type == TOKEN_IDENTIFIER);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* معرف مع رقم عربي-هندي */
    {
        Lexer* lexer = lexer_create("رقم٢", "test");
        Token token = lexer_next_token(lexer);
        assert_test("معرف مع رقم عربي: رقم٢", token.type == TOKEN_IDENTIFIER);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* معرف يبدأ بشرطة سفلية */
    {
        Lexer* lexer = lexer_create("_متغير", "test");
        Token token = lexer_next_token(lexer);
        assert_test("معرف يبدأ بـ _", token.type == TOKEN_IDENTIFIER);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* تعليق غير مغلق */
    {
        Lexer* lexer = lexer_create("#[ تعليق مفتوح", "test");
        lexer->skip_comments = 0;
        Token token = lexer_next_token(lexer);
        assert_test("تعليق غير مغلق", token.type == TOKEN_COMMENT_MULTI_START);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* تعليق متداخل */
    {
        Lexer* lexer = lexer_create("#[ خارجي #[ داخلي ]# ]#", "test");
        lexer->skip_comments = 0;
        Token token = lexer_next_token(lexer);
        assert_test("تعليق متداخل", token.type == TOKEN_COMMENT_MULTI_START);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* حرف escape */
    {
        Lexer* lexer = lexer_create("'\\n'", "test");
        Token token = lexer_next_token(lexer);
        assert_test("حرف escape: \\n", token.type == TOKEN_CHARACTER);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* نص مع escape مزدوج */
    {
        Lexer* lexer = lexer_create("\"نص مع \\\\ backslash\"", "test");
        Token token = lexer_next_token(lexer);
        assert_test("نص مع backslash", token.type == TOKEN_STRING);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* فاصلة منقوطة عربية */
    {
        Lexer* lexer = lexer_create("أ = 10؛ ب = 20", "test");
        Token t1 = lexer_next_token(lexer);
        Token t2 = lexer_next_token(lexer);
        Token t3 = lexer_next_token(lexer);
        Token t4 = lexer_next_token(lexer);
        assert_test("فاصلة منقوطة عربية", t4.type == TOKEN_SEMICOLON);
        token_destroy(&t1);
        token_destroy(&t2);
        token_destroy(&t3);
        token_destroy(&t4);
        lexer_destroy(lexer);
    }
    
    /* سهم مزدوج */
    {
        Lexer* lexer = lexer_create("->", "test");
        Token token = lexer_next_token(lexer);
        assert_test("سهم: ->", token.type == TOKEN_ARROW);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* سهم تعيين */
    {
        Lexer* lexer = lexer_create(":-", "test");
        Token token = lexer_next_token(lexer);
        assert_test("سهم تعيين: :-", token.type == TOKEN_COLON_ARROW);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* تحريك يسار مع تعيين */
    {
        Lexer* lexer = lexer_create("<<=", "test");
        Token token = lexer_next_token(lexer);
        assert_test(" <<= ", token.type == TOKEN_SHIFT_LEFT_ASSIGN);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* تحريك يمين مع تعيين */
    {
        Lexer* lexer = lexer_create(">>=", "test");
        Token token = lexer_next_token(lexer);
        assert_test(" >>= ", token.type == TOKEN_SHIFT_RIGHT_ASSIGN);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* رمز غير معروف */
    {
        Lexer* lexer = lexer_create("$", "test");
        Token token = lexer_next_token(lexer);
        assert_test("رمز غير معروف: $", token.type == TOKEN_ERROR);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* UTF-8 صحيح */
    {
        const char* utf8 = "مرحباً";
        Lexer* lexer = lexer_create(utf8, "test");
        Token token = lexer_next_token(lexer);
        assert_test("UTF-8: مرحباً", token.type == TOKEN_IDENTIFIER);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* حرف غير صالح في UTF-8 */
    {
        /* بايت غير صالح وحيد */
        Lexer* lexer = lexer_create("\xFF", "test");
        Token token = lexer_next_token(lexer);
        assert_test("بايت غير صالح UTF-8", token.type == TOKEN_ERROR);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* مصفوفة فارغة */
    {
        Lexer* lexer = lexer_create("{}", "test");
        Token t1 = lexer_next_token(lexer);
        Token t2 = lexer_next_token(lexer);
        assert_test("مصفوفة فارغة: {", t1.type == TOKEN_LBRACE);
        assert_test("مصفوفة فارغة: }", t2.type == TOKEN_RBRACE);
        token_destroy(&t1);
        token_destroy(&t2);
        lexer_destroy(lexer);
    }
    
    /* كلمة محجوزة في بداية معرف */
    {
        Lexer* lexer = lexer_create("متغير", "test");
        Token token = lexer_next_token(lexer);
        assert_test("كلمة محجوزة: متغير", token.type == TOKEN_KEYWORD_VARIABLE);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* معرف يبدأ بكلمة محجوزة */
    {
        Lexer* lexer = lexer_create("متغيرات", "test");
        Token token = lexer_next_token(lexer);
        assert_test("معرف يبدأ بكلمة محجوزة", token.type == TOKEN_IDENTIFIER);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* أرقام مع فاصل سفلي */
    {
        Lexer* lexer = lexer_create("1_000_000", "test");
        Token token = lexer_next_token(lexer);
        assert_test("أرقام مع فاصل سفلي", token.type == TOKEN_INTEGER);
        assert_test("قيمة الرقم", token.literal.int_value == 1000000);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* عدد عشري科学ي */
    {
        Lexer* lexer = lexer_create("1.5e10", "test");
        Token token = lexer_next_token(lexer);
        assert_test("عدد علمي: 1.5e10", token.type == TOKEN_FLOAT);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* عدد عشري科学ي مع إشارة */
    {
        Lexer* lexer = lexer_create("1.5e-3", "test");
        Token token = lexer_next_token(lexer);
        assert_test("عدد علمي: 1.5e-3", token.type == TOKEN_FLOAT);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
}

/**
 * @brief اختبارات إدارة الذاكرة
 */
static void test_memory(void) {
    printf("\n=== اختبارات الذاكرة ===\n");
    
    /* lexer_create_from_file */
    {
        FILE* f = tmpfile();
        if (f) {
            fprintf(f, "متغير: ع = 10");
            rewind(f);
            Lexer* lexer = lexer_create_from_file(f, "test.dad");
            assert_test("create_from_file: ليس NULL", lexer != NULL);
            if (lexer) {
                assert_test("create_from_file: source_owned", lexer->source_owned == 1);
                Token token = lexer_next_token(lexer);
                assert_test("create_from_file: يقرأ Tokens", token.type == TOKEN_KEYWORD_VARIABLE);
                token_destroy(&token);
                lexer_destroy(lexer);
            }
            fclose(f);
        }
    }
    
    /* lexer_create_from_file مع ملف فارغ */
    {
        FILE* f = tmpfile();
        if (f) {
            Lexer* lexer = lexer_create_from_file(f, "empty.dad");
            assert_test("create_from_file فارغ: NULL", lexer == NULL);
            fclose(f);
        }
    }
    
    /* lexer_create_from_file مع NULL */
    {
        Lexer* lexer = lexer_create_from_file(NULL, "test");
        assert_test("create_from_file NULL: NULL", lexer == NULL);
    }
    
    /* token_destroy على Token فارغ */
    {
        Token empty = {0};
        token_destroy(&empty);
        assert_test("token_destroy فارغ: لا ي_crash", 1);
    }
    
    /* token_copy */
    {
        Lexer* lexer = lexer_create("متغير", "test");
        Token original = lexer_next_token(lexer);
        Token copy = token_copy(&original);
        assert_test("token_copy: النوع مطابق", copy.type == original.type);
        assert_test("token_copy: القيمة مختلفة المؤشر", copy.value != original.value);
        assert_test("token_copy: المحتوى مطابق", strcmp(copy.value, original.value) == 0);
        token_destroy(&original);
        token_destroy(&copy);
        lexer_destroy(lexer);
    }
}

/**
 * @brief اختبارات Unicode و UTF-8
 */
static void test_unicode(void) {
    printf("\n=== اختبارات Unicode ===\n");
    
    /* معرف عربي طويل */
    {
        Lexer* lexer = lexer_create("اسم_الطالب_في_المدرسة", "test");
        Token token = lexer_next_token(lexer);
        assert_test("معرف عربي طويل", token.type == TOKEN_IDENTIFIER);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* حرف عربي في نص */
    {
        Lexer* lexer = lexer_create("\"مرحباً بالعالم\"", "test");
        Token token = lexer_next_token(lexer);
        assert_test("نص عربي كامل", token.type == TOKEN_STRING);
        assert_test("نص عربي: الطول", token.length > 0);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* حرف عربي-هندي في حرف */
    {
        Lexer* lexer = lexer_create("'٥'", "test");
        Token token = lexer_next_token(lexer);
        assert_test("حرف عربي-هندي", token.type == TOKEN_CHARACTER);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* UTF-8 غير صالح */
    {
        /* بايت وحيد غير صالح: 0xFF */
        Lexer* lexer = lexer_create("\xFF", "test");
        Token token = lexer_next_token(lexer);
        assert_test("UTF-8 غير صالح: 0xFF", token.type == TOKEN_ERROR);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* UTF-8 غير صالح: بايت متابعة مفقودة */
    {
        /* 0xC0 هو بداية 2 بايت، لكن لا يليه بايت متابعة */
        Lexer* lexer = lexer_create("\xC0", "test");
        Token token = lexer_next_token(lexer);
        assert_test("UTF-8 غير صالح: بايت متابعة مفقودة", token.type == TOKEN_ERROR);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
}

/**
 * @brief اختبارات Scientific Notation
 */
static void test_scientific(void) {
    printf("\n=== اختبارات Scientific Notation ===\n");
    
    /* 1e - خطأ: لا توجد أرقام بعد e */
    {
        Lexer* lexer = lexer_create("1e", "test");
        Token token = lexer_next_token(lexer);
        assert_test("1e: خطأ", token.type == TOKEN_ERROR);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* 1e+ - خطأ */
    {
        Lexer* lexer = lexer_create("1e+", "test");
        Token token = lexer_next_token(lexer);
        assert_test("1e+: خطأ", token.type == TOKEN_ERROR);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* 1e- - خطأ */
    {
        Lexer* lexer = lexer_create("1e-", "test");
        Token token = lexer_next_token(lexer);
        assert_test("1e-: خطأ", token.type == TOKEN_ERROR);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* 1e10 - صحيح */
    {
        Lexer* lexer = lexer_create("1e10", "test");
        Token token = lexer_next_token(lexer);
        assert_test("1e10: صحيح", token.type == TOKEN_FLOAT);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* 1e+10 - صحيح */
    {
        Lexer* lexer = lexer_create("1e+10", "test");
        Token token = lexer_next_token(lexer);
        assert_test("1e+10: صحيح", token.type == TOKEN_FLOAT);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
    
    /* .5e10 - يُعامل كنقطة ثم عدد علمي */
    {
        Lexer* lexer = lexer_create(".5e10", "test");
        Token t1 = lexer_next_token(lexer);
        assert_test(".5e10: النقطة أولاً", t1.type == TOKEN_DOT);
        token_destroy(&t1);
        Token t2 = lexer_next_token(lexer);
        assert_test(".5e10: الرقم 5e10", t2.type == TOKEN_FLOAT);
        token_destroy(&t2);
        lexer_destroy(lexer);
    }
}

/**
 * @brief اختبارات Peek
 */
static void test_peek(void) {
    printf("\n=== اختبارات Peek ===\n");
    
    /* peek لا يأكل التوكن */
    {
        Lexer* lexer = lexer_create("10 20", "test");
        Token peeked = lexer_peek_token(lexer);
        assert_test("peek: النوع صحيح", peeked.type == TOKEN_INTEGER);
        Token actual = lexer_next_token(lexer);
        assert_test("peek: التوكن التالي مطابق", actual.type == TOKEN_INTEGER);
        assert_test("peek: نفس القيمة", strcmp(peeked.value, actual.value) == 0);
        token_destroy(&peeked);
        token_destroy(&actual);
        lexer_destroy(lexer);
    }
    
    /* peek لا يغير error_count */
    {
        Lexer* lexer = lexer_create("10 $ 20", "test");
        /* تخطي الخطأ الأول */
        lexer_next_token(lexer); /* 10 */
        lexer_next_token(lexer); /* $ → خطأ */
        size_t errors_before = lexer_error_count(lexer);
        Token peeked = lexer_peek_token(lexer);
        size_t errors_after = lexer_error_count(lexer);
        assert_test("peek: لا يغير error_count", errors_before == errors_after);
        token_destroy(&peeked);
        lexer_destroy(lexer);
    }
    
    /* peek متعدد */
    {
        Lexer* lexer = lexer_create("10 20 30", "test");
        Token p1 = lexer_peek_token(lexer);
        Token p2 = lexer_peek_token(lexer);
        Token p3 = lexer_peek_token(lexer);
        assert_test("peek متعدد: p1 = 10", p1.type == TOKEN_INTEGER);
        assert_test("peek متعدد: p2 = 10", p2.type == TOKEN_INTEGER);
        assert_test("peek متعدد: p3 = 10", p3.type == TOKEN_INTEGER);
        token_destroy(&p1);
        token_destroy(&p2);
        token_destroy(&p3);
        Token actual = lexer_next_token(lexer);
        assert_test("peek متعدد: التالي仍是 10", actual.type == TOKEN_INTEGER);
        token_destroy(&actual);
        lexer_destroy(lexer);
    }
}

/**
 * @brief اختبارات Error Recovery
 */
static void test_error_recovery(void) {
    printf("\n=== اختبارات Error Recovery ===\n");
    
    /* استمرار بعد الخطأ */
    {
        Lexer* lexer = lexer_create("\"نص\" $ متغير", "test");
        Token t1 = lexer_next_token(lexer);
        assert_test("بعد الخطأ: نص صحيح", t1.type == TOKEN_STRING);
        token_destroy(&t1);
        Token t2 = lexer_next_token(lexer);
        assert_test("بعد الخطأ: خطأ", t2.type == TOKEN_ERROR);
        token_destroy(&t2);
        Token t3 = lexer_next_token(lexer);
        assert_test("بعد الخطأ: متغير", t3.type == TOKEN_KEYWORD_VARIABLE);
        token_destroy(&t3);
        lexer_destroy(lexer);
    }
    
    /* حساب الأخطاء */
    {
        Lexer* lexer = lexer_create("\"$a$b$", "test");
        while (1) {
            Token t = lexer_next_token(lexer);
            if (t.type == TOKEN_EOF || t.type == TOKEN_ERROR) {
                if (t.type == TOKEN_ERROR) {
                    token_destroy(&t);
                }
                break;
            }
            token_destroy(&t);
        }
        assert_test("حساب الأخطاء: هناك أخطاء", lexer_has_errors(lexer));
        assert_test("حساب الأخطاء: عدد > 0", lexer_error_count(lexer) > 0);
        lexer_destroy(lexer);
    }
    
    /* lexer_get_error */
    {
        Lexer* lexer = lexer_create("\"$", "test");
        Token t = lexer_next_token(lexer);
        token_destroy(&t);
        const LexerError* err = lexer_get_error(lexer, 0);
        assert_test("get_error: ليس NULL", err != NULL);
        assert_test("get_error: كود L003", strcmp(err->code, "L003") == 0);
        assert_test("get_error: فهرس غير صالح", lexer_get_error(lexer, 999) == NULL);
        lexer_destroy(lexer);
    }
}

/**
 * @brief اختبار Character Literals مع \0
 */
static void test_character_null(void) {
    printf("\n=== اختبارات Character \\0 ===\n");
    
    /* حرف \0 */
    {
        Lexer* lexer = lexer_create("'\\0'", "test");
        Token token = lexer_next_token(lexer);
        assert_test("حرف \\0: النوع صحيح", token.type == TOKEN_CHARACTER);
        assert_test("حرف \\0: has_literal", token.has_literal == 1);
        assert_test("حرف \\0: القيمة 0", token.literal.int_value == 0);
        token_destroy(&token);
        lexer_destroy(lexer);
    }
}

/**
 * @brief الدالة الرئيسية للاختبارات
 */
int main(void) {
    printf("=== اختبارات Lexer - ض Core ===\n");
    printf("الإصدار: 0.1.0\n");
    printf("التاريخ: 2026-08-03\n");
    
    test_keywords();
    test_identifiers();
    test_numbers();
    test_strings();
    test_characters();
    test_comments();
    test_operators();
    test_delimiters();
    test_whitespace_newlines();
    test_eof();
    test_errors();
    test_full_program();
    test_edge_cases();
    test_memory();
    test_unicode();
    test_scientific();
    test_peek();
    test_error_recovery();
    test_character_null();
    
    printf("\n=== النتائج ===\n");
    printf("الاختبارات الناجحة: %d\n", tests_passed);
    printf("الاختبارات الفاشلة: %d\n", tests_failed);
    printf("المجموع: %d\n", tests_passed + tests_failed);
    
    return tests_failed > 0 ? 1 : 0;
}
