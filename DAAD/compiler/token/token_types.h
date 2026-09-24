/**
 * @file token_types.h
 * @brief تعريف أنواع الـ Tokens في لغة ض Core
 * 
 * هذا الملف يحتوي على جميع أنواع الـ Tokens التي يتعرف عليها Lexer.
 * جميع الأنواع مأخوذة من المواصفات الرسمية في docs/keywords.md.
 * 
 * @version 0.1.0
 * @date 2026-08-03
 */

#ifndef DAAD_TOKEN_TYPES_H
#define DAAD_TOKEN_TYPES_H

/**
 * @brief أنواع الـ Tokens
 */
typedef enum {
    /* ===== كلمات مفتاحية ===== */
    
    /* تعريف البيانات */
    TOKEN_KEYWORD_VARIABLE,         /* متغير */
    TOKEN_KEYWORD_CONSTANT,         /* ثابت */
    TOKEN_KEYWORD_TYPE_DEF,         /* نوع */
    TOKEN_KEYWORD_TYPEOF,           /* جلب_نوع */
    TOKEN_KEYWORD_STRUCT,           /* هيكل */
    TOKEN_KEYWORD_ARRAY,            /* مصفوفة */
    TOKEN_KEYWORD_POINTER_TYPE,     /* مؤشر_نوع */
    
    /* الأنواع الأساسية */
    TOKEN_KEYWORD_INT,              /* رقم */
    TOKEN_KEYWORD_FLOAT,            /* رقم_عشري */
    TOKEN_KEYWORD_CHAR,             /* حرف */
    TOKEN_KEYWORD_BOOL,             /* منطق */
    TOKEN_KEYWORD_VOID,             /* فراغ */
    TOKEN_KEYWORD_STRING,           /* نص */
    
    /* العمليات الحسابية */
    TOKEN_KEYWORD_ADD,              /* اجمع */
    TOKEN_KEYWORD_SUB,              /* اطرح */
    TOKEN_KEYWORD_MUL,              /* اضرب */
    TOKEN_KEYWORD_DIV,              /* اقسم */
    TOKEN_KEYWORD_MOD,              /* باقي */
    
    /* العمليات المنطقية */
    TOKEN_KEYWORD_AND,              /* و */
    TOKEN_KEYWORD_OR,               /* أو */
    TOKEN_KEYWORD_NOT,              /* ليس */
    
    /* العمليات على البتات */
    TOKEN_KEYWORD_BITWISE_AND,      /* و_بتية */
    TOKEN_KEYWORD_BITWISE_OR,       /* أو_بتية */
    TOKEN_KEYWORD_BITWISE_XOR,      /* تبادل_بتية */
    TOKEN_KEYWORD_BITWISE_NOT,      /* ليس_بتية */
    TOKEN_KEYWORD_SHIFT_LEFT,       /* تحريك_يسار */
    TOKEN_KEYWORD_SHIFT_RIGHT,      /* تحريك_يمين */
    
    /* المقارنات */
    TOKEN_KEYWORD_EQUALS,           /* يساوي */
    TOKEN_KEYWORD_NOT_EQUALS,       /* لا_يساوي */
    TOKEN_KEYWORD_GREATER,          /* أكبر */
    TOKEN_KEYWORD_LESS,             /* أصغر */
    TOKEN_KEYWORD_GREATER_EQUALS,   /* أكبر_أو_يساوي */
    TOKEN_KEYWORD_LESS_EQUALS,      /* أصغر_أو_يساوي */
    
    /* عمليات الذاكرة */
    TOKEN_KEYWORD_LOAD,             /* حمل */
    TOKEN_KEYWORD_STORE,            /* خزن */
    TOKEN_KEYWORD_READ_MEM,         /* اقرأ_ذاكرة */
    TOKEN_KEYWORD_READ_STACK,       /* اقرأ_مكدس */
    TOKEN_KEYWORD_PUSH,             /* ادفع */
    TOKEN_KEYWORD_POP,              /* اسحب */
    TOKEN_KEYWORD_MOVE,             /* انقل */
    TOKEN_KEYWORD_COPY_MEM,         /* انسخ_ذاكرة */
    TOKEN_KEYWORD_CLEAR_MEM,        /* امسح_ذاكرة */
    
    /* إدارة الكومة */
    TOKEN_KEYWORD_ALLOC,            /* احجز */
    TOKEN_KEYWORD_FREE,             /* حرر */
    TOKEN_KEYWORD_RESIZE,           /* غيّر_حجم */
    
    /* التحكم في التدفق */
    TOKEN_KEYWORD_IF,               /* إذا */
    TOKEN_KEYWORD_ELSE,             /* وإلا */
    TOKEN_KEYWORD_WHILE,            /* طالما */
    TOKEN_KEYWORD_FOR,              /* لكل */
    TOKEN_KEYWORD_RETURN,           /* ارجع */
    TOKEN_KEYWORD_GOTO,             /* اقفز */
    TOKEN_KEYWORD_EXIT,             /* توقف */
    TOKEN_KEYWORD_LABEL,            /* علامة */
    
    /* الدوال */
    TOKEN_KEYWORD_FUNCTION,         /* دالة */
    TOKEN_KEYWORD_SYSCALL,          /* استدعاء */
    TOKEN_KEYWORD_END,              /* نهاية */
    TOKEN_KEYWORD_LOCAL,            /* محلي */
    TOKEN_KEYWORD_FORWARD,          /* أمامية */
    
    /* التعامل مع الأخطاء */
    TOKEN_KEYWORD_TRY,              /* حاول */
    TOKEN_KEYWORD_CATCH,            /* امسك */
    TOKEN_KEYWORD_THROW,            /* أطلق */
    
    /* الإدخال/الإخراج */
    TOKEN_KEYWORD_PRINT,            /* اطبع */
    TOKEN_KEYWORD_INPUT,            /* أدخل */
    TOKEN_KEYWORD_APPEND,           /* ادرج */
    TOKEN_KEYWORD_OPEN,             /* افتح */
    TOKEN_KEYWORD_READ_FILE,        /* اقرأ_ملف */
    TOKEN_KEYWORD_WRITE_FILE,       /* اكتب_ملف */
    TOKEN_KEYWORD_CLOSE,            /* اغلق */
    
    /* التعامل مع الأنواع */
    TOKEN_KEYWORD_CAST,             /* حوّل */
    TOKEN_KEYWORD_SIZEOF_TYPE,      /* حجم_النوع */
    TOKEN_KEYWORD_SIZEOF_VALUE,     /* حجم_القيمة */
    
    /* القيم المميزة */
    TOKEN_KEYWORD_TRUE,             /* صحيح */
    TOKEN_KEYWORD_FALSE,            /* خطأ */
    TOKEN_KEYWORD_NULL,             /* فراغ (كمية) */
    
    /* ===== أنواع البيانات ===== */
    
    TOKEN_INTEGER,                  /* عدد صحيح: 123, 0xFF, 0b1010, 0o77 */
    TOKEN_FLOAT,                    /* عدد عشري: 3.14, 1.0e10 */
    TOKEN_STRING,                   /* سلسلة نصية: "مرحباً" */
    TOKEN_CHARACTER,                /* حرف واحد: 'أ' */
    
    /* ===== المعرفات ===== */
    
    TOKEN_IDENTIFIER,               /* معرف: اسم متغير أو دالة */
    
    /* ===== المعاملات ===== */
    
    TOKEN_PLUS,                     /* + */
    TOKEN_MINUS,                    /* - */
    TOKEN_STAR,                     /* * */
    TOKEN_SLASH,                    /* / */
    TOKEN_PERCENT,                  /* % */
    
    TOKEN_ASSIGN,                   /* = */
    TOKEN_PLUS_ASSIGN,              /* += */
    TOKEN_MINUS_ASSIGN,             /* -= */
    TOKEN_STAR_ASSIGN,              /* *= */
    TOKEN_SLASH_ASSIGN,             /* /= */
    TOKEN_PERCENT_ASSIGN,           /* %= */
    TOKEN_SHIFT_LEFT_ASSIGN,        /* <<= */
    TOKEN_SHIFT_RIGHT_ASSIGN,       /* >>= */
    
    TOKEN_EQUAL,                    /* == */
    TOKEN_NOT_EQUAL,                /* != */
    TOKEN_GREATER,                  /* > */
    TOKEN_LESS,                     /* < */
    TOKEN_GREATER_EQUAL,            /* >= */
    TOKEN_LESS_EQUAL,               /* <= */
    
    TOKEN_AND,                      /* و (منطقي) - في الكود: && */
    TOKEN_OR,                       /* أو (منطقي) - في الكود: || */
    TOKEN_NOT,                      /* ليس (منطقي) - في الكود: ! */
    
    TOKEN_BITWISE_AND,              /* & */
    TOKEN_BITWISE_OR,               /* | */
    TOKEN_BITWISE_XOR,              /* ^ */
    TOKEN_BITWISE_NOT,              /* ~ */
    TOKEN_SHIFT_LEFT,               /* << */
    TOKEN_SHIFT_RIGHT,              /* >> */
    
    /* ===== الفواصل (Delimiters) ===== */
    
    TOKEN_COLON,                    /* : */
    TOKEN_SEMICOLON,                /* ؛ (عربي فقط) */
    TOKEN_ARROW,                    /* -> */
    TOKEN_COLON_ARROW,              /* :- */
    TOKEN_COMMA,                    /* , */
    TOKEN_DOT,                      /* . */
    TOKEN_AT,                       /* @ */
    
    TOKEN_LPAREN,                   /* ( */
    TOKEN_RPAREN,                   /* ) */
    TOKEN_LBRACE,                   /* { */
    TOKEN_RBRACE,                   /* } */
    TOKEN_LBRACKET,                 /* [ */
    TOKEN_RBRACKET,                 /* ] */
    
    /* ===== التعليقات ===== */
    
    TOKEN_COMMENT_SINGLE,           /* # تعليق سطر واحد */
    TOKEN_COMMENT_MULTI_START,      /* #[ بداية تعليق متعدد */
    TOKEN_COMMENT_MULTI_END,        /* ]# نهاية تعليق متعدد */
    
    /* ===== خاص ===== */
    
    TOKEN_NEWLINE,                  /* نهاية سطر */
    TOKEN_EOF,                      /* نهاية ملف */
    TOKEN_ERROR,                    /* خطأ */
    
    /* ===== غير مدعوم حالياً (للتوسع المستقبلي) ===== */
    
    TOKEN_KEYWORD_ASYNC,            /* متزامن (غير مدعوم) */
    TOKEN_KEYWORD_PARALLEL,         /* متواز (غير مدعوم) */
    TOKEN_KEYWORD_AWAIT,            /* انتظر (غير مدعوم) */
    TOKEN_KEYWORD_SEND,             /* أرسل (غير مدعوم) */
    TOKEN_KEYWORD_RECEIVE,          /* استقبل (غير مدعوم) */
    
    TOKEN_COUNT                     /* عدد الأنواع الإجمالي */
} TokenType;

/**
 * @brief الحصول على اسم النوع كنص
 * @param type نوع الـ Token
 * @return اسم النوع
 */
const char* token_type_name(TokenType type);

#endif /* DAAD_TOKEN_TYPES_H */
