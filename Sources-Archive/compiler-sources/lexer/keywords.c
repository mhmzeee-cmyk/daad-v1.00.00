/**
 * @file keywords.c
 * @brief تنفيذ جدول البحث عن الكلمات المفتاحية
 * 
 * @version 0.1.0
 * @date 2026-08-03
 */

#include "lexer/keywords.h"
#include <string.h>

/**
 * @brief جدول الكلمات المفتاحية
 */
static KeywordEntry KEYWORDS[] = {
    /* إ */
    {"إذا",              TOKEN_KEYWORD_IF,               0},
    
    /* أ */
    {"أدخل",             TOKEN_KEYWORD_INPUT,             0},
    {"أمامية",           TOKEN_KEYWORD_FORWARD,           0},
    {"أو",               TOKEN_KEYWORD_OR,                0},
    {"أو_بتية",          TOKEN_KEYWORD_BITWISE_OR,        0},
    {"أطلق",             TOKEN_KEYWORD_THROW,              0},
    {"أكبر",             TOKEN_KEYWORD_GREATER,            0},
    {"أكبر_أو_يساوي",    TOKEN_KEYWORD_GREATER_EQUALS,    0},
    {"أغلق",             TOKEN_KEYWORD_CLOSE,              0},
    
    /* ا */
    {"ادفع",             TOKEN_KEYWORD_PUSH,              0},
    {"ادرج",             TOKEN_KEYWORD_APPEND,             0},
    {"ارجع",             TOKEN_KEYWORD_RETURN,             0},
    {"اسحب",             TOKEN_KEYWORD_POP,               0},
    {"اقرأ_ذاكرة",       TOKEN_KEYWORD_READ_MEM,          0},
    {"اقرأ_مكدس",        TOKEN_KEYWORD_READ_STACK,        0},
    {"اقرأ_ملف",         TOKEN_KEYWORD_READ_FILE,         0},
    {"اقفز",             TOKEN_KEYWORD_GOTO,              0},
    {"استدعاء",          TOKEN_KEYWORD_SYSCALL,           0},
    {"احجز",             TOKEN_KEYWORD_ALLOC,             0},
    {"اطبع",             TOKEN_KEYWORD_PRINT,             0},
    {"اطرح",             TOKEN_KEYWORD_SUB,               0},
    {"اضرب",             TOKEN_KEYWORD_MUL,               0},
    {"اقسم",             TOKEN_KEYWORD_DIV,               0},
    {"امسح_ذاكرة",       TOKEN_KEYWORD_CLEAR_MEM,         0},
    {"امسك",             TOKEN_KEYWORD_CATCH,              0},
    {"انسخ_ذاكرة",       TOKEN_KEYWORD_COPY_MEM,          0},
    {"انقل",             TOKEN_KEYWORD_MOVE,               0},
    
    /* ب */
    {"باقي",             TOKEN_KEYWORD_MOD,                0},
    
    /* ت */
    {"تحريك_يمين",       TOKEN_KEYWORD_SHIFT_RIGHT,       0},
    {"تحريك_يسار",       TOKEN_KEYWORD_SHIFT_LEFT,        0},
    {"توقف",             TOKEN_KEYWORD_EXIT,               0},
    {"تبادل_بتية",       TOKEN_KEYWORD_BITWISE_XOR,       0},
    
    /* ث */
    {"ثابت",             TOKEN_KEYWORD_CONSTANT,           0},
    
    /* ح */
    {"حاول",             TOKEN_KEYWORD_TRY,                0},
    {"حجم_القيمة",       TOKEN_KEYWORD_SIZEOF_VALUE,      0},
    {"حجم_النوع",        TOKEN_KEYWORD_SIZEOF_TYPE,       0},
    {"حمل",              TOKEN_KEYWORD_LOAD,               0},
    {"حوّل",             TOKEN_KEYWORD_CAST,               0},
    {"حرر",              TOKEN_KEYWORD_FREE,               0},
    {"حرف",              TOKEN_KEYWORD_CHAR,               0},
    {"خزن",              TOKEN_KEYWORD_STORE,              0},
    {"خطأ",              TOKEN_KEYWORD_FALSE,              0},
    
    /* د */
    {"دالة",             TOKEN_KEYWORD_FUNCTION,           0},
    
    /* ر */
    {"رقم",              TOKEN_KEYWORD_INT,                0},
    {"رقم_عشري",         TOKEN_KEYWORD_FLOAT,             0},
    
    /* س */
    {"سجل",              TOKEN_KEYWORD_LOCAL,              0},
    
    /* ص */
    {"صحيح",             TOKEN_KEYWORD_TRUE,               0},
    
    /* ط */
    {"طالما",            TOKEN_KEYWORD_WHILE,              0},
    {"طابع",             TOKEN_KEYWORD_PRINT,              0},
    
    /* ض */
    {"ضمن",              TOKEN_KEYWORD_LOCAL,              0},
    
    /* غ */
    {"غيّر_حجم",         TOKEN_KEYWORD_RESIZE,            0},
    
    /* ف */
    {"فراغ",             TOKEN_KEYWORD_VOID,               0},
    
    /* ق */
    {"قارن",             TOKEN_KEYWORD_EQUALS,             0},
    
    /* ك */
    {"كل",               TOKEN_KEYWORD_FOR,                0},
    
    /* ل */
    {"لكل",              TOKEN_KEYWORD_FOR,                0},
    {"لا_يساوي",         TOKEN_KEYWORD_NOT_EQUALS,        0},
    {"ليس",              TOKEN_KEYWORD_NOT,                0},
    {"ليس_بتية",         TOKEN_KEYWORD_BITWISE_NOT,       0},
    
    /* م */
    {"محلية",            TOKEN_KEYWORD_LOCAL,              0},
    {"محلي",             TOKEN_KEYWORD_LOCAL,              0},
    {"متغير",            TOKEN_KEYWORD_VARIABLE,           0},
    {"مثل",              TOKEN_KEYWORD_LOCAL,              0},
    {"منطق",             TOKEN_KEYWORD_BOOL,               0},
    {"مصفوفة",           TOKEN_KEYWORD_ARRAY,             0},
    {"مؤشر_نوع",         TOKEN_KEYWORD_POINTER_TYPE,      0},
    
    /* ن */
    {"نوع",              TOKEN_KEYWORD_TYPE_DEF,           0},
    {"نوع_عددي",         TOKEN_KEYWORD_INT,               0},
    {"نهاية",            TOKEN_KEYWORD_END,               0},
    {"نص",               TOKEN_KEYWORD_STRING,             0},
    
    /* هـ */
    {"هيكل",             TOKEN_KEYWORD_STRUCT,             0},
    
    /* و */
    {"و",                TOKEN_KEYWORD_AND,                0},
    {"وإلا",             TOKEN_KEYWORD_ELSE,               0},
    {"و_بتية",           TOKEN_KEYWORD_BITWISE_AND,        0},
    
    /* ي */
    {"يساوي",            TOKEN_KEYWORD_EQUALS,             0},
};

/**
 * @brief عدد الكلمات المفتاحية في الجدول
 */
static const size_t KEYWORD_COUNT = sizeof(KEYWORDS) / sizeof(KeywordEntry);

/**
 * @brief تهيئة أطوال الكلمات المفتاحية
 */
static int keywords_initialized = 0;

static void keywords_init_lengths(void) {
    if (keywords_initialized) return;
    for (size_t i = 0; i < KEYWORD_COUNT; i++) {
        KEYWORDS[i].length = strlen(KEYWORDS[i].text);
    }
    keywords_initialized = 1;
}

/**
 * @brief إعادة تهيئة الكلمات المفتاحية (للاختبارات)
 */
void keywords_reset_init(void) {
    keywords_initialized = 0;
}

/**
 * @brief البحث عن كلمة مفتاحية (بحث خطي)
 */
TokenType keywords_lookup(const char* text, size_t length) {
    if (!text || length == 0) return TOKEN_IDENTIFIER;
    
    keywords_init_lengths();
    
    for (size_t i = 0; i < KEYWORD_COUNT; i++) {
        if (KEYWORDS[i].length == length) {
            if (memcmp(KEYWORDS[i].text, text, length) == 0) {
                return KEYWORDS[i].type;
            }
        }
    }
    
    return TOKEN_IDENTIFIER;
}

/**
 * @brief التحقق من أن النص كلمة مفتاحية
 */
int keywords_is_keyword(const char* text, size_t length) {
    return keywords_lookup(text, length) != TOKEN_IDENTIFIER;
}
