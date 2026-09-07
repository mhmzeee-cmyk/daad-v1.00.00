/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ║                                                                            ║
 * ║   ملف المرجع النهائي — داله (Dhad Processor ISA Reference)                ║
 * ║                                                                            ║
 * ║   هذا الملف هو المرجع الوحيد والنهائي لجميع ثوابت المعمارية.              ║
 * ║   أي تغيير في المعمارية يجب أن يبدأ من هذا الملف أولاً.                  ║
 * ║   جميع ملفات المشروع (المحاكي، المجمّع، المترجم) تعتمد عليه.             ║
 * ║                                                                            ║
 * ║   الإصدار: 2.0.0 (8-bit)                                                   ║
 * ║   التاريخ: 2026-08-26                                                      ║
 * ║   المؤلف: مشروع ض — محمد محمود الحموز                                      ║
 * ║                                                                            ║
 * ╚══════════════════════════════════════════════════════════════════════════════
 */

#ifndef DHAD_ISA_CONSTANTS_H
#define DHAD_ISA_CONSTANTS_H

/* ═══════════════════════════════════════════════════════════════════════════════
 *  الفصل 1: معلومات المشروع
 * ═══════════════════════════════════════════════════════════════════════════════ */

#define DHAD_ISA_NAME            "داله (Dhad)"
#define DHAD_ISA_VERSION         "4.0.0"
#define DHAD_ISA_CODENAME        "رابع (Fourth)"
#define DHAD_ISA_WIDTH_BITS      8
#define DHAD_ISA_DESC            "معالج عربي مخصص للغة ض — الإصدار 4.0 مع مقاطعات"
#define DHAD_ISA_AUTHORS         "مشروع ض — محمد محمود الحموز"
#define DHAD_ISA_LICENSE         "MIT"

/* ═══════════════════════════════════════════════════════════════════════════════
 *  الفصل 2: أبعاد المعالج الأساسية
 *
 *  ┌─────────────────────────────────────────────────────────────────┐
 *  │  هذه القيم تحدد الهيكل الفيزيائي للمعالج.                      │
 *  │  تغييرها يعني تغيير في المعمارية بالكامل.                       │
 *  └─────────────────────────────────────────────────────────────────┘
 * ═══════════════════════════════════════════════════════════════════════════════ */

/** عرض مسار البيانات (Data Path Width) */
#define DHAD_DATA_WIDTH          8

/** عرض العنوان (Address Bus Width) */
#define DHAD_ADDR_WIDTH          12

/** عدد السجلات العامة */
#define DHAD_REG_COUNT           8

/** حجم المكدس الأقصى */
#define DHAD_STACK_SIZE          32

/** حجم الذاكرة الكلي (2^ADDR_WIDTH) */
#define DHAD_MEM_SIZE            (1 << DHAD_ADDR_WIDTH)   /* 4096 */

/** عدد البتات في التعليمة الواحدة */
#define DHAD_INST_WIDTH          8

/** عدد البتات في التعليمة الموسعة */
#define DHAD_INST_EXT_WIDTH      12

/** أقصى حجم للبرنامج (نبدأ من 0x800) */
#define DHAD_PROG_MAX_SIZE       0x700

/** بداية البرنامج في الذاكرة */
#define DHAD_PROG_START          0x800

/** نهاية الذاكرة الفعلية */
#define DHAD_MEM_END             0xFFF

/** الحد الأقصى لعدد الدورات في جلسة واحدة */
#define DHAD_MAX_CYCLES          100000

/* ═══════════════════════════════════════════════════════════════════════════════
 *  الفصل 3: خريطة الذاكرة (Memory Map)
 *
 *  ┌──────────────────────────────────────────────────────────┐
 *  │ العنوان    │ الحجم    │ الوصف                           │
 *  ├────────────┼──────────┼─────────────────────────────────┤
 *  │ 0xFFF      │ 1        │ نهاية الذاكرة                   │
 *  │ ...        │ ...      │ ...                             │
 *  │ 0x800      │ 0x7FF    │ منطقة البرنامج (ROM)            │
 *  │ 0x0FF      │ 1        │ بداية منطقة البيانات (RAM)      │
 *  │ ...        │ ...      │ ...                             │
 *  │ 0x020      │ 0xE0     │ بيانات عامة                     │
 *  │ 0x01F      │ 1        │ أعلى المكدس                     │
 *  │ ...        │ ...      │ ... (ينمو للأسفل)               │
 *  │ 0x010      │ 0x10     │ المكدس (Stack)                  │
 *  │ 0x000      │          │ بداية الذاكرة                   │
 *  └──────────────────────────────────────────────────────────┘
 * ═══════════════════════════════════════════════════════════════════════════════ */

/** بداية منطقة البرنامج */
#define DHAD_MEM_PROG_START     0x800

/** نهاية منطقة البرنامج */
#define DHAD_MEM_PROG_END       0xFFF

/** بداية منطقة البيانات (RAM) */
#define DHAD_MEM_DATA_START     0x020

/** نهاية منطقة البيانات */
#define DHAD_MEM_DATA_END       0x01F

/** بداية المكدس */
#define DHAD_MEM_STACK_START    0x000

/** نهاية المكدس */
#define DHAD_MEM_STACK_END      0x01F

/** حجم المكدس بالبايت */
#define DHAD_MEM_STACK_SIZE     (DHAD_MEM_STACK_END - DHAD_MEM_STACK_START + 1)

/** حجم البيانات المتاح */
#define DHAD_MEM_DATA_SIZE      (DHAD_MEM_DATA_END - DHAD_MEM_DATA_START + 1)

/* ═══════════════════════════════════════════════════════════════════════════════
 *  الفصل 4: منافذ I/O (Memory-Mapped I/O)
 *
 *  ┌──────────────────────────────────────────────────────────┐
 *  │ العنوان    │ الاسم        │ الوصف                       │
 *  ├────────────┼──────────────┼─────────────────────────────┤
 *  │ 0x0F0      │ IO_OUT       │ منفذ الإخراج (licept Byte)  │
 *  │ 0x0F1      │ IO_IN        │ منفذ الإدخال (licept Byte)  │
 *  │ 0x0F2      │ IO_STATUS    │ سجل الحالة                  │
 *  │ 0x0F3      │ IO_CTRL      │ سجل التحكم                  │
 *  └──────────────────────────────────────────────────────────┘
 * ═══════════════════════════════════════════════════════════════════════════════ */

#define DHAD_IO_OUT_ADDR        0x0F0
#define DHAD_IO_IN_ADDR         0x0F1
#define DHAD_IO_STATUS_ADDR     0x0F2
#define DHAD_IO_CTRL_ADDR       0x0F3

/** أوامر I/O */
#define DHAD_IO_CMD_PRINT       0x01    /* طباعة بايت من IO_OUT */
#define DHAD_IO_CMD_INPUT       0x02    /* إدخال بايت إلى IO_IN */
#define DHAD_IO_CMD_RESET       0xFF    /* إعادة تعيين المنافذ */

/* ═══════════════════════════════════════════════════════════════════════════════
 *  الفصل 5: نظام السجلات (Register File)
 *
 *  ┌──────────────────────────────────────────────────────────────────────────┐
 *  │  المعرف     │ الاسم العربي │ الاسم الإنجليزي │ الوصف                    │
 *  ├─────────────┼──────────────┼─────────────────┼──────────────────────────┤
 *  │ DHAD_REG_0  │ س0           │ R0              │ سجل عام 0                │
 *  │ DHAD_REG_1  │ س1           │ R1              │ سجل عام 1                │
 *  │ DHAD_REG_2  │ س2           │ R2              │ سجل عام 2                │
 *  │ DHAD_REG_3  │ س3           │ R3              │ سجل عام 3                │
 *  │ DHAD_REG_4  │ س4           │ R4              │ سجل عام 4                │
 *  │ DHAD_REG_5  │ س5           │ R5              │ سجل عام 5                │
 *  │ DHAD_REG_6  │ س6           │ R6              │ سجل عام 6                │
 *  │ DHAD_REG_7  │ س7           │ R7              │ سجل عام 7                │
 *  │ DHAD_REG_A  │ مح           │ ACC             │ المُراقب (Accumulator)   │
 *  │ DHAD_REG_F  │ أعلام        │ FLAGS           │ سجل الأعلام              │
 *  └──────────────────────────────────────────────────────────────────────────┘
 *
 *  ┌────────────────────────────────────────────────────────────┐
 *  │  ي Shel  │ الوصف                                          │
 *  ├──────────┼────────────────────────────────────────────────┤
 *  │ س0 - س7  │ سجلات عامة متعددة الاستخدام                    │
 *  │ مح       │ السجل الرئيسي للعمليات الحسابية والمنطقية     │
 *  │ أعلام    │ يحتوي على أعلام Z, N, C                         │
 *  │ PC       │ عداد البرنامج (12-bit، غير مرئي للمبرمج)       │
 *  │ SP       │ مؤشر المكدس (4-bit، غير مرئي للمبرمج)          │
 *  └────────────────────────────────────────────────────────────┘
 * ═══════════════════════════════════════════════════════════════════════════════ */

typedef enum {
    DHAD_REG_0 = 0,     /* س0 — سجل عام 0 */
    DHAD_REG_1,         /* س1 — سجل عام 1 */
    DHAD_REG_2,         /* س2 — سجل عام 2 */
    DHAD_REG_3,         /* س3 — سجل عام 3 */
    DHAD_REG_4,         /* س4 — سجل عام 4 */
    DHAD_REG_5,         /* س5 — سجل عام 5 */
    DHAD_REG_6,         /* س6 — سجل عام 6 */
    DHAD_REG_7,         /* س7 — سجل عام 7 */
    DHAD_REG_A,         /* مح — المُراقب (Accumulator) */
    DHAD_REG_F,         /* أعلام — سجل الأعلام */
    DHAD_REG_PC,        /* عداد البرنامج (12-bit) */
    DHAD_REG_SP,        /* مؤشر المكدس (4-bit) */
    DHAD_REG_COUNT_REG  /* العدد الإجمالي */
} DhadRegisterID;

/** أسماء السجلات بالعربي (مرجعية) */
#define DHAD_REG_NAME_0         "س0"
#define DHAD_REG_NAME_1         "س1"
#define DHAD_REG_NAME_2         "س2"
#define DHAD_REG_NAME_3         "س3"
#define DHAD_REG_NAME_4         "س4"
#define DHAD_REG_NAME_5         "س5"
#define DHAD_REG_NAME_6         "س6"
#define DHAD_REG_NAME_7         "س7"
#define DHAD_REG_NAME_A         "مح"
#define DHAD_REG_NAME_F         "أعلام"

/** أسماء السجلات بالإنجليزي (مرجعية) */
#define DHAD_REG_NAME_EN_0      "R0"
#define DHAD_REG_NAME_EN_1      "R1"
#define DHAD_REG_NAME_EN_2      "R2"
#define DHAD_REG_NAME_EN_3      "R3"
#define DHAD_REG_NAME_EN_4      "R4"
#define DHAD_REG_NAME_EN_5      "R5"
#define DHAD_REG_NAME_EN_6      "R6"
#define DHAD_REG_NAME_EN_7      "R7"
#define DHAD_REG_NAME_EN_A      "ACC"
#define DHAD_REG_NAME_EN_F      "FLAGS"

/** حجم كل سجل (بالبتات) */
#define DHAD_REG_SIZE_BITS      DHAD_DATA_WIDTH

/** القيمة القصوى لسجل */
#define DHAD_REG_MAX_VALUE      ((1 << DHAD_REG_SIZE_BITS) - 1)

/** مask لتسجيلات البيانات */
#define DHAD_REG_MASK           DHAD_REG_MAX_VALUE

/* ═══════════════════════════════════════════════════════════════════════════════
 *  الفصل 6: سجل الأعلام (Flags Register)
 *
 *  ┌──────────────────────────────────────────────────────────────┐
 *  │ البت  │ الرمز │ الاسم العربي  │ الشرح                       │
 *  ├───────┼───────┼──────────────┼─────────────────────────────┤
 *  │   0   │   Z   │ صفر          │ النتيجة تساوي صفر           │
 *  │   1   │   N   │ سالب         │ النتيجة سالبة (bit 3 = 1)   │
 *  │   2   │   C   │ حمل          │ وصل حمل من أعلى بت          │
 *  │   3   │   V   │ فيض          │ فيض/تلوث في العملية          │
 *  └──────────────────────────────────────────────────────────────┘
 *
 *  ملاحظة: البتات 4-7 محجوزة للمستقبل
 * ═══════════════════════════════════════════════════════════════════════════════ */

#define DHAD_FLAG_BIT_ZERO      0   /* Z: صفر */
#define DHAD_FLAG_BIT_NEG       1   /* N: سالب */
#define DHAD_FLAG_BIT_CARRY     2   /* C: حمل */
#define DHAD_FLAG_BIT_OVERFLOW  3   /* V: فيض */

#define DHAD_FLAG_ZERO          (1 << DHAD_FLAG_BIT_ZERO)      /* 0x01 */
#define DHAD_FLAG_NEG           (1 << DHAD_FLAG_BIT_NEG)       /* 0x02 */
#define DHAD_FLAG_CARRY         (1 << DHAD_FLAG_BIT_CARRY)     /* 0x04 */
#define DHAD_FLAG_OVERFLOW      (1 << DHAD_FLAG_BIT_OVERFLOW)  /* 0x08 */

/** جميع الأعلام */
#define DHAD_FLAG_ALL           (DHAD_FLAG_ZERO | DHAD_FLAG_NEG | DHAD_FLAG_CARRY | DHAD_FLAG_OVERFLOW)

/** لا أعلام */
#define DHAD_FLAG_NONE          0x00

/** أعلام محجوزة (للمستقبل) */
#define DHAD_FLAG_RESERVED      0xF0

/* ═══════════════════════════════════════════════════════════════════════════════
 *  الفصل 7: تنسيق التعليمة (Instruction Encoding)
 *
 *  ╔═══════════════════════════════════════════════════════════════════╗
 *  ║  التنسيق الأساسي: 8 بت                                          ║
 *  ║                                                                   ║
 *  ║  ┌──────────┬──────────┐                                         ║
 *  ║  │ Opcode   │ Operand  │                                         ║
 *  ║  │ [7:4]    │ [3:0]    │                                         ║
 *  ║  │ (4 bit)  │ (4 bit)  │                                         ║
 *  ║  └──────────┴──────────┘                                         ║
 *  ║                                                                   ║
 *  ║  التنسيق الموسع: 12 بت (تعليمات القفز والمنطق الموسعة)          ║
 *  ║                                                                   ║
 *  ║  ┌──────────┬──────────────────────────────────┐                  ║
 *  ║  │ Opcode   │ 0xF0                             │                  ║
 *  ║  ├──────────┼──────────┬───────────────────────┤                  ║
 *  ║  │ ExtCode  │ Reg/Type │ [Address/Unused]      │                  ║
 *  ║  │ [11:8]   │ [7:4]    │ [3:0] (للقفز فقط)    │                  ║
 *  ║  │ (4 bit)  │ (4 bit)  │ (4 bit)               │                  ║
 *  ║  └──────────┴──────────┴───────────────────────┘                  ║
 *  ║                                                                   ║
 *  ║  ملاحظة: التعليمات非-قفز لا تستخدم البتات [3:0] من Byte الثاني  ║
 *  ╚═══════════════════════════════════════════════════════════════════╝
 * ═══════════════════════════════════════════════════════════════════════════════ */

/** موضع Opcode في Byte الأول */
#define DHAD_OPCODE_SHIFT       4
#define DHAD_OPCODE_MASK        0xF0

/** موضع Operand في Byte الأول */
#define DHAD_OPERAND_SHIFT      0
#define DHAD_OPERAND_MASK       0x0F

/** Byte التعليمة الموسعة */
#define DHAD_EXT_PREFIX         0xF0

/** موضع Extension Code في Byte الثاني */
#define DHAD_EXT_CODE_SHIFT     4
#define DHAD_EXT_CODE_MASK      0xF0

/** موضع Reg/Type في Byte الثاني */
#define DHAD_EXT_REG_SHIFT      0
#define DHAD_EXT_REG_MASK       0x0F

/** عدد البايتات لكل نوع تعليمة */
#define DHAD_INST_SIZE_BASE     1   /* تعليمة أساسية: 1 بايت */
#define DHAD_INST_SIZE_IMM      2   /* حمّل + قيمة: 2 بايت */
#define DHAD_INST_SIZE_MEM      2   /* اقرأ/خزن + عنوان: 2 بايت */
#define DHAD_INST_SIZE_CALL     2   /* نداء + عنوان: 2 بايت */
#define DHAD_INST_SIZE_JUMP     3   /* قفز + Extension + عنوان: 3 بايت */
#define DHAD_INST_SIZE_EXT      2   /* تعليمة موسعة بدون قفز: 2 بايت */

/* ═══════════════════════════════════════════════════════════════════════════════
 *  الفصل 8: مجموعة التعليمات الأساسية (Base Opcodes)
 *
 *  ╔═══════════════════════════════════════════════════════════════════╗
 *  ║  Opcode[7:4] │ العدد  │ الوصف                                    ║
 *  ║══════════════╪════════╪══════════════════════════════════════════║
 *  ║  0x0         │ 1      │ توقف / سكون                              ║
 *  ║  0x1         │ 1      │ حمّل (load immediate)                     ║
 *  ║  0x2         │ 1      │ نقل (mov reg, reg)                        ║
 *  ║  0x3         │ 1      │ اقرأ من الذاكرة                           ║
 *  ║  0x4         │ 1      │ خزن في الذاكرة                            ║
 *  ║  0x5         │ 1      │ ادفع للمكدس (push)                        ║
 *  ║  0x6         │ 1      │ اسحب من المكدس (pop)                      ║
 *  ║  0x7         │ 1      │ نداء دالة (call)                          ║
 *  ║  0x8         │ 1      │ إرجاع / تبريد                             ║
 *  ║  0x9         │ 1      │ جمع                                       ║
 *  ║  0xA         │ 1      │ طرح                                       ║
 *  ║  0xB         │ 1      │ ضرب                                       ║
 *  ║  0xC         │ 1      │ قسمة                                      ║
 *  ║  0xD         │ 1      │ باقي القسمة                               ║
 *  ║  0xE         │ 1      │ طباعة (print)                             ║
 *  ║  0xF         │ 1      │ تعليمة موسعة (extended)                   ║
 *  ╚═══════════════════════════════════════════════════════════════════╝
 * ═══════════════════════════════════════════════════════════════════════════════ */

/* ─── الفئة 0: النظام ─── */
#define DHAD_OP_NOP             0x00    /* سكون: لا تفعل شيئاً */

/* ─── الفئة 1: النقل الفوري ─── */
#define DHAD_OP_LOAD_IMM        0x10    /* حمّل سج, قيمة: سج = قيمة (8-bit) */

/* ─── الفئة 2: نقل بين السجلات ─── */
#define DHAD_OP_MOV_REG         0x20    /* نقل dst, src: dst = src */

/* ─── الفئة 3: قراءة من الذاكرة ─── */
#define DHAD_OP_LOAD_MEM        0x30    /* اقرأ سج, عنوان: سج = الذاكرة[عنوان] */

/* ─── الفئة 4: كتابة في الذاكرة ─── */
#define DHAD_OP_STORE_MEM       0x40    /* خزن سج, عنوان: الذاكرة[عنوان] = سج */

/* ─── الفئة 5: المكدس ─── */
#define DHAD_OP_PUSH            0x50    /* ادفع: push مح للمكدس */

/* ─── الفئة 6: المكدس ─── */
#define DHAD_OP_POP             0x60    /* اسحب: pop من المكدس للمح */

/* ─── الفئة 7: الدوال ─── */
#define DHAD_OP_CALL            0x70    /* نداء عنوان: push PC, jump */

/* ─── الفئة 8: الإرجاع / التبريد ─── */
#define DHAD_OP_RET             0x80    /* إرجاع: pop PC (operand = 0) */
#define DHAD_OP_NEG             0x8F    /* تبريد: مح = -مح (operand ≠ 0) */

/* ─── الفئة 9: الحساب ─── */
#define DHAD_OP_ADD             0x90    /* جمع: مح = مح + سج */

/* ─── الفئة 10: الحساب ─── */
#define DHAD_OP_SUB             0xA0    /* طرح: مح = مح - سج */

/* ─── الفئة 11: الحساب ─── */
#define DHAD_OP_MUL             0xB0    /* ضرب: مح = مح × سج */

/* ─── الفئة 12: الحساب ─── */
#define DHAD_OP_DIV             0xC0    /* قسمة: مح = مح ÷ سج */

/* ─── الفئة 13: الحساب ─── */
#define DHAD_OP_MOD             0xD0    /* باقي: مح = مح % سج */

/* ─── الفئة 14: الإخراج ─── */
#define DHAD_OP_PRINT           0xE0    /* طباعة: print مح كرقم */

/* ─── الفئة 15: التعليمات الموسعة ─── */
#define DHAD_OP_EXTENDED        0xF0    /* بادئة التعليمة الموسعة */

/* ═══════════════════════════════════════════════════════════════════════════════
 *  الفصل 9: تعليمات موسعة (Extended Opcodes)
 *
 *  جميع هذه التعليمات تبدأ بـ 0xF0 في البايت الأول.
 *  البايت الثاني يحدد نوع التعليمة الموسعة.
 *
 *  ╔═══════════════════════════════════════════════════════════════════╗
 *  ║  ExtCode[7:4] │ العدد │ الوصف                                   ║
 *  ║═══════════════╪═══════╪═════════════════════════════════════════║
 *  ║  0x00         │ 1     │ قفز غير شرطي (JMP)                     ║
 *  ║  0x10         │ 1     │ قفز إذا صفر (JZ)                       ║
 *  ║  0x20         │ 1     │ قفز إذا غير صفر (JNZ)                  ║
 *  ║  0x30         │ 1     │ قفز إذا حمل (JC)                       ║
 *  ║  0x40         │ 1     │ قفز إذا سالب (JN)                      ║
 *  ║  0x50         │ 1     │ طباعة حرف (PRINT_CH)                   ║
 *  ║  0x60         │ 1     │ إدخال (INPUT)                           ║
 *  ║  0x70         │ 1     │ أو حصري (XOR)                           ║
 *  ║  0x80         │ 1     │ أو منطقي (OR)                           ║
 *  ║  0x90         │ 1     │ و منطقي (AND)                           ║
 *  ║  0xA0         │ 1     │ نفي (NOT)                               ║
 *  ║  0xB0         │ 1     │ تحريك لليسار (SHL)                      ║
 *  ║  0xC0         │ 1     │ تحريك لليمين (SHR)                      ║
 *  ║  0xD0-0xFF    │ --    │ محجوز للمستقبل                          ║
 *  ╚═══════════════════════════════════════════════════════════════════╝
 * ═══════════════════════════════════════════════════════════════════════════════ */

/* ─── قفز ─── */
#define DHAD_EXT_JMP            0x00    /* قفز غير شرطي: PC = عنوان */
#define DHAD_EXT_JZ             0x10    /* قفز إذا صفر: إذا Z=1 */
#define DHAD_EXT_JNZ            0x20    /* قفز إذا غير صفر: إذا Z=0 */
#define DHAD_EXT_JC             0x30    /* قفز إذا حمل: إذا C=1 */
#define DHAD_EXT_JN             0x40    /* قفز إذا سالب: إذا N=1 */

/* ─── I/O ─── */
#define DHAD_EXT_PRINT_CH       0x50    /* طباعة حرف ASCII من المح */
#define DHAD_EXT_INPUT          0x60    /* إدخال رقم من المستخدم */

/* ─── المنطق ─── */
#define DHAD_EXT_XOR            0x70    /* أو حصري: مح = مح XOR سج */
#define DHAD_EXT_OR             0x80    /* أو منطقي: مح = مح OR سج */
#define DHAD_EXT_AND            0x90    /* و منطقي: مح = مح AND سج */
#define DHAD_EXT_NOT            0xA0    /* نفي: مح = NOT مح */

/* ─── التحريك ─── */
#define DHAD_EXT_SHL            0xB0    /* تحريك لليسار: مح = مح << 1 */
#define DHAD_EXT_SHR            0xC0    /* تحريك لليمين: مح = مح >> 1 */

/* ─── محجوز (تعليمات جديدة) ─── */
#define DHAD_EXT_SWAP          0xD0    /* بدل س1, س2: تبديل بين سجلين */
#define DHAD_EXT_DEC           0xD1    /* نقص سج: سج-- */
#define DHAD_EXT_INC           0xD2    /* وزد سج: سج++ */
#define DHAD_EXT_HALT          0xD3    /* توقف: إيقاف المعالج */
#define DHAD_EXT_CMP           0xE0    /* قارن سج: مح - سج (الأعلام فقط) */
#define DHAD_EXT_EI            0xE1    /* فعّل المقاطعات */
#define DHAD_EXT_DI            0xE2    /* عطّل المقاطعات */
#define DHAD_EXT_RETI          0xE3    /* العودة من المقاطعة */

/* ═══════════════════════════════════════════════════════════════════════════════
 *  الفصل 10: جدول التعليمات الكامل (Full Instruction Table)
 *
 *  هذا الجدول هو المرجع النهائي لكل تعليمة و吃饱 encodings.
 *
 *  ╔═══════╦══════════╦═════════╦═══════════════════════════════════════╗
 *  ║ Opcode║ الاسم    ║ الحجم  ║ الوصف                                ║
 *  ╠═══════╬══════════╬═════════╬═══════════════════════════════════════╣
 *  ║ 0x00  ║ سكون     ║ 1 بايت  ║ لا يفعل شيئاً (NOP)                  ║
 *  ║ 0x1r  ║ حمّل     ║ 2 بايت  ║ سج = byte2 (قيمة فورية 8-bit)       ║
 *  ║ 0x2d  ║ نقل      ║ 2 بايت  ║ dst = src                            ║
 *  ║ 0x3r  ║ اقرأ     ║ 2 بايت  ║ سج = الذاكرة[byte2]                  ║
 *  ║ 0x4r  ║ خزن      ║ 2 بايت  ║ الذاكرة[byte2] = سج                  ║
 *  ║ 0x50  ║ ادفع     ║ 1 بايت  ║ push مح للمكدس                       ║
 *  ║ 0x60  ║ اسحب     ║ 1 بايت  ║ pop من المكدس للمح                    ║
 *  ║ 0x70  ║ نداء     ║ 3 بايت  ║ push PC, jump addr16                  ║
 *  ║ 0x80  ║ إرجاع    ║ 1 بايت  ║ pop PC                                ║
 *  ║ 0x8F  ║ تبريد    ║ 1 بايت  ║ مح = -مح                              ║
 *  ║ 0x9r  ║ جمع      ║ 1 بايت  ║ مح = مح + سج                         ║
 *  ║ 0xAr  ║ طرح      ║ 1 بايت  ║ مح = مح - سج                         ║
 *  ║ 0xBr  ║ ضرب      ║ 1 بايت  ║ مح = مح × سج                         ║
 *  ║ 0xCr  ║ قسمة     ║ 1 بايت  ║ مح = مح ÷ سج                         ║
 *  ║ 0xDr  ║ باقي     ║ 1 بايت  ║ مح = مح % سج                         ║
 *  ║ 0xE0  ║ اطبع     ║ 1 بايت  ║ print مح كرقم                         ║
 *  ║ 0xF0  ║ (موسعة)  ║ 2-3     ║ انظر الفصل 9                         ║
 *  ╚═══════╩══════════╩═════════╩═══════════════════════════════════════╝
 *
 *  r = رقم السجل (0-7)
 *  d = dst<<4 | src (لتعليمات نقل السجلات و SWAP)
 * ═══════════════════════════════════════════════════════════════════════════════ */

/* ═══════════════════════════════════════════════════════════════════════════════
 *  الفصل 11: الأسماء العربية للتعليمات (Arabic Mnemonics)
 *
 *  ╔══════════════════════╦═══════════════════════════════════════╗
 *  ║ الاسم العربي        ║ المعادل الإنجليزي / الوصف             ║
 *  ╠══════════════════════╬═══════════════════════════════════════╣
 *  ║ توقف                ║ HALT (0xF0 0x00)                      ║
 *  ║ سكون                ║ NOP (0x00)                             ║
 *  ║ حمّل                ║ LOAD_IMM (r, imm8)                     ║
 *  ║ نقل                 ║ MOV (dst, src)                         ║
 *  ║ اقرأ                ║ LOAD_MEM (r, addr8)                    ║
 *  ║ خزن                 ║ STORE_MEM (r, addr8)                   ║
 *  ║ ادفع                ║ PUSH                                   ║
 *  ║ اسحب                ║ POP                                    ║
 *  ║ نداء                ║ CALL (addr16)                          ║
 *  ║ إرجاع / ارجع       ║ RET                                    ║
 *  ║ تبريد               ║ NEG                                    ║
 *  ║ جمع                 ║ ADD (r)                                ║
 *  ║ طرح                 ║ SUB (r)                                ║
 *  ║ ضرب                 ║ MUL (r)                                ║
 *  ║ قسمة                ║ DIV (r)                                ║
 *  ║ باقي                ║ MOD (r)                                ║
 *  ║ اطبع                ║ PRINT                                  ║
 *  ║ اطبع_حرف            ║ PRINT_CH                               ║
 *  ║ ادخل                ║ INPUT                                  ║
 *  ║ قفز                 ║ JMP (addr16)                           ║
 *  ║ قفز_إذا_صفر        ║ JZ (addr16)                            ║
 *  ║ قفز_إذا_غيرصفر     ║ JNZ (addr16)                           ║
 *  ║ قفز_إذا_حمل        ║ JC (addr16)                            ║
 *  ║ قفز_إذا_سالب       ║ JN (addr16)                            ║
 *  ║ ومنطقي              ║ AND (r)                                ║
 *  ║ أومنطقي             ║ OR (r)                                 ║
 *  ║ أوحصري              ║ XOR (r)                                ║
 *  ║ ليس                 ║ NOT                                    ║
 *  ║ تحريك_يسار          ║ SHL                                    ║
 *  ║ تحريك_يمين          ║ SHR                                    ║
 *  ║ قارن                ║ CMP (r) — مح - سج (الأعلام فقط)       ║
 *  ║ بدل                 ║ SWAP (dst, src) — تبديل سجلين         ║
 *  ║ نقص                 ║ DEC (r) — سج--                         ║
 *  ║ وزد                 ║ INC (r) — سج++                         ║
 *  ╚══════════════════════╩═══════════════════════════════════════╝
 * ═══════════════════════════════════════════════════════════════════════════════ */

#define DHAD_MNEMONIC_HALT      "توقف"
#define DHAD_MNEMONIC_NOP       "سكون"
#define DHAD_MNEMONIC_LOAD_IMM  "حمّل"
#define DHAD_MNEMONIC_MOV       "نقل"
#define DHAD_MNEMONIC_LOAD      "اقرأ"
#define DHAD_MNEMONIC_STORE     "خزن"
#define DHAD_MNEMONIC_PUSH      "ادفع"
#define DHAD_MNEMONIC_POP       "اسحب"
#define DHAD_MNEMONIC_CALL      "نداء"
#define DHAD_MNEMONIC_RET       "إرجاع"
#define DHAD_MNEMONIC_NEG       "تبريد"
#define DHAD_MNEMONIC_ADD       "جمع"
#define DHAD_MNEMONIC_SUB       "طرح"
#define DHAD_MNEMONIC_MUL       "ضرب"
#define DHAD_MNEMONIC_DIV       "قسمة"
#define DHAD_MNEMONIC_MOD       "باقي"
#define DHAD_MNEMONIC_PRINT     "اطبع"
#define DHAD_MNEMONIC_PRINT_CH  "اطبع_حرف"
#define DHAD_MNEMONIC_INPUT     "ادخل"
#define DHAD_MNEMONIC_JMP       "قفز"
#define DHAD_MNEMONIC_JZ        "قفز_إذا_صفر"
#define DHAD_MNEMONIC_JNZ       "قفز_إذا_غيرصفر"
#define DHAD_MNEMONIC_JC        "قفز_إذا_حمل"
#define DHAD_MNEMONIC_JN        "قفز_إذا_سالب"
#define DHAD_MNEMONIC_AND       "ومنطقي"
#define DHAD_MNEMONIC_OR        "أومنطقي"
#define DHAD_MNEMONIC_XOR       "أوحصري"
#define DHAD_MNEMONIC_NOT       "ليس"
#define DHAD_MNEMONIC_SHL       "تحريك_يسار"
#define DHAD_MNEMONIC_SHR       "تحريك_يمين"

/* ═══════════════════════════════════════════════════════════════════════════════
 *  الفصل 12: القيم الثابتة (Constants)
 * ═══════════════════════════════════════════════════════════════════════════════ */

/** القيم الصفرية */
#define DHAD_ZERO_4BIT          0x00
#define DHAD_ZERO_8BIT          0x00
#define DHAD_ZERO_12BIT         0x000
#define DHAD_ZERO_16BIT         0x0000

/** القيم القصوى */
#define DHAD_MAX_4BIT           0xFF    /* 255 — 8-bit */
#define DHAD_MAX_8BIT           0xFF    /* 255 */
#define DHAD_MAX_12BIT          0xFFF   /* 4095 */
#define DHAD_MAX_16BIT          0xFFFF  /* 65535 */

/** قيم خاصة */
#define DHAD_VALUE_TRUE         0x01
#define DHAD_VALUE_FALSE        0x00
#define DHAD_VALUE_NULL         0x00
#define DHAD_VALUE_NEWLINE      0x0A    /* سطر جديد ASCII */
#define DHAD_VALUE_SPACE        0x20    /* مسافة ASCII */

/* ═══════════════════════════════════════════════════════════════════════════════
 *  الفصل 13: تنسيق القيم الفورية (Immediate Value Encoding)
 *
 *  ╔═══════════════════════════════════════════════════════════════════╗
 *  ║  النوع        │ المدى            │ التنسيق                       ║
 *  ║═══════════════╪══════════════════╪═══════════════════════════════║
 *  ║  فوري 4-bit   │ 0-15 (أو -8→7)  │ بايت واحد [3:0]              ║
 *  ║  فوري 8-bit   │ 0-255           │ بايتان [7:0]                  ║
 *  ║  عنوان 8-bit  │ 0x800-0x8FF     │ بايت واحد (بالإضافة لـ0x800) ║
 *  ╚═══════════════════════════════════════════════════════════════════╝
 *
 *  ملاحظة: القيم الفورية تُخزّن كأرقام موجبة فقط (unsigned).
 *  للتعامل مع الأرقام السالبة، نستخدم تعليمة تبريد (NEG).
 * ═══════════════════════════════════════════════════════════════════════════════ */

#define DHAD_IMM4_MIN           0
#define DHAD_IMM4_MAX           15
#define DHAD_IMM8_MIN           0
#define DHAD_IMM8_MAX           255

/** تحويل القيمة السالبة إلى تمثيل 4-bit */
#define DHAD_TO_4BIT_SIGNED(x)  ((uint8_t)((x) & 0x0F))

/** استخراج القيمة السالبة من 4-bit */
#define DHAD_FROM_4BIT_SIGNED(x) ((int8_t)(((x) & 0x08) ? ((x) | 0xF0) : (x)))

/* ═══════════════════════════════════════════════════════════════════════════════
 *  الفصل 14: تعريفات الأنواع (Type Definitions)
 * ═══════════════════════════════════════════════════════════════════════════════ */

#include <stdint.h>
#include <stdbool.h>

/** أنواع القيم في المعالج */
typedef uint8_t     dhad_u4;     /* 4-bit unsigned */
typedef uint8_t     dhad_u8;     /* 8-bit unsigned */
typedef uint16_t    dhad_u12;    /* 12-bit unsigned (يُخزّن في 16-bit) */
typedef uint16_t    dhad_u16;    /* 16-bit unsigned */
typedef int8_t      dhad_s4;     /* 4-bit signed */
typedef int8_t      dhad_s8;     /* 8-bit signed */

/** أنواع التعليمات */
typedef dhad_u8     DhadOpcode;
typedef dhad_u8     DhadOperand;
typedef dhad_u8     DhadExtCode;
typedef dhad_u12    DhadAddress;

/** هيكل التعليمة المشفرة */
typedef struct {
    dhad_u8     byte1;          /* البايت الأول: opcode + operand */
    dhad_u8     byte2;          /* البايت الثاني: قيمة/عنوان/extension */
    dhad_u8     byte3;          /* البايت الثالث: عنوان (للقفز فقط) */
    int         size;           /* عدد البايتات الفعلية (1, 2, أو 3) */
} DhadEncodedInst;

/** حالة المعالج */
typedef struct {
    /* السجلات */
    dhad_u8     regs[DHAD_REG_COUNT];  /* س0 - س7 */
    dhad_u8     acc;                    /* المح (Accumulator) */
    dhad_u8     flags;                  /* سجل الأعلام */

    /* عداد البرنامج */
    dhad_u12    pc;

    /* مؤشر المكدس */
    dhad_u8     sp;

    /* الذاكرة */
    dhad_u8     memory[DHAD_MEM_SIZE];

    /* المكدس (في الذاكرة) */
    dhad_u16    stack[DHAD_STACK_SIZE];

    /* الحالة */
    bool        halted;
    bool        running;
    uint32_t    cycle_count;

    /* I/O */
    dhad_u8     io_out;
    dhad_u8     io_in;
    bool        io_ready;
} DhadCPUState;

/* ═══════════════════════════════════════════════════════════════════════════════
 *  الفصل 15: دوال مساعدة (Utility Macros)
 * ═══════════════════════════════════════════════════════════════════════════════ */

/** استخراج Opcode من البايت الأول */
#define DHAD_GET_OPCODE(byte1)      (((byte1) >> DHAD_OPCODE_SHIFT) & 0x0F)

/** استخراج Operand من البايت الأول */
#define DHAD_GET_OPERAND(byte1)     ((byte1) & DHAD_OPERAND_MASK)

/** بناء البايت الأول */
#define DHAD_MAKE_BYTE1(op, oper)   ((((op) & 0x0F) << DHAD_OPCODE_SHIFT) | ((oper) & DHAD_OPERAND_MASK))

/** استخراج Extension Code من البايت الثاني */
#define DHAD_GET_EXT_CODE(byte2)    ((byte2) & DHAD_EXT_CODE_MASK)

/** استخراج Reg من البايت الثاني */
#define DHAD_GET_EXT_REG(byte2)     ((byte2) & DHAD_EXT_REG_MASK)

/** بناء البايت الثاني للموسعة */
#define DHAD_MAKE_EXT_BYTE(ext, reg) (((ext) & DHAD_EXT_CODE_MASK) | ((reg) & DHAD_EXT_REG_MASK))

/** هل التعليمة موسعة؟ */
#define DHAD_IS_EXTENDED(op)        ((op) == DHAD_OP_EXTENDED)

/** هل التعليمة تتطلب بايت ثاني؟ */
#define DHAD_HAS_BYTE2(op)          ((op) == DHAD_OP_LOAD_IMM || (op) == DHAD_OP_LOAD_MEM || \
                                     (op) == DHAD_OP_STORE_MEM || (op) == DHAD_OP_CALL || \
                                     (op) == DHAD_OP_EXTENDED)

/** هل التعليمة الموسعة تتطلب بايت ثالث (عنوان قفز)؟ */
#define DHAD_EXT_HAS_ADDR(ext)      ((ext) <= DHAD_EXT_JN)

/** تحويل العنوان إلى موضع في الذاكرة */
#define DHAD_ADDR_TO_MEM(addr)      ((addr) + DHAD_PROG_START)

/** تحويل الموضع في الذاكرة إلى عنوان */
#define DHAD_MEM_TO_ADDR(mem)       ((mem) - DHAD_PROG_START)

/* ═══════════════════════════════════════════════════════════════════════════════
 *  الفصل 16: أخطاء المعالج (Error Codes)
 * ═══════════════════════════════════════════════════════════════════════════════ */

#define DHAD_OK                     0    /* نجاح */
#define DHAD_ERR_UNKNOWN_OPCODE    -1    /* opcode غير معروف */
#define DHAD_ERR_UNKNOWN_REG       -2    /* سجل غير معروف */
#define DHAD_ERR_DIV_ZERO          -3    /* قسمة على صفر */
#define DHAD_ERR_STACK_OVERFLOW    -4    /* تدفق المكدس للأعلى */
#define DHAD_ERR_STACK_UNDERFLOW   -5    /* تدفق المكدس للأسفل */
#define DHAD_ERR_MEM_OUT_OF_RANGE  -6    /* عنوان خارج الذاكرة */
#define DHAD_ERR_ILLEGAL_INST      -7    /* تعليمة غير قانونية */
#define DHAD_ERR_IO_TIMEOUT        -8    /* انتهاء مهلة I/O */

/* ═══════════════════════════════════════════════════════════════════════════════
 *  الفصل 17: أسماء الملفات والامتدادات (File Conventions)
 * ═══════════════════════════════════════════════════════════════════════════════ */

#define DHAD_SOURCE_EXT_ASM        ".ضasm"
#define DHAD_SOURCE_EXT_ASM_EN     ".dasm"
#define DHAD_BINARY_EXT            ".dbin"
#define DHAD_OBJECT_EXT            ".dobj"
#define DHAD_EXECUTABLE_EXT        ".dexe"

#define DHAD_DEFAULT_INPUT_EXT     DHAD_SOURCE_EXT_ASM
#define DHAD_DEFAULT_OUTPUT_EXT    DHAD_BINARY_EXT

/* ═══════════════════════════════════════════════════════════════════════════════
 *  الفصل 18: ثوابت العرض والتنسيق (Display Constants)
 * ═══════════════════════════════════════════════════════════════════════════════ */

/** ألوان ANSI */
#define DHAD_COLOR_RESET           "\033[0m"
#define DHAD_COLOR_RED             "\033[31m"
#define DHAD_COLOR_GREEN           "\033[32m"
#define DHAD_COLOR_YELLOW          "\033[33m"
#define DHAD_COLOR_BLUE            "\033[34m"
#define DHAD_COLOR_MAGENTA         "\033[35m"
#define DHAD_COLOR_CYAN            "\033[36m"
#define DHAD_COLOR_WHITE           "\033[37m"
#define DHAD_COLOR_BOLD            "\033[1m"
#define DHAD_COLOR_DIM             "\033[2m"
#define DHAD_COLOR_BG_BLUE         "\033[44m"
#define DHAD_COLOR_BG_GREEN        "\033[42m"
#define DHAD_COLOR_BG_RED          "\033[41m"

/** أبعاد العرض الافتراضية */
#define DHAD_DISPLAY_WIDTH         60
#define DHAD_DISPLAY_HEIGHT        30
#define DHAD_REGISTER_BOX_WIDTH    20
#define DHAD_MEMORY_BOX_WIDTH      30
#define DHAD_LOG_BOX_WIDTH         40

/* ═══════════════════════════════════════════════════════════════════════════════
 *  الفصل 19: إعدادات المحاكي (Simulator Configuration)
 * ═══════════════════════════════════════════════════════════════════════════════ */

/** سرعة المحاكي (밀리 ثانية لكل دورة) */
#define DHAD_SIM_DELAY_MS          0

/** هل نطبع كل تعليمة؟ */
#define DHAD_SIM_VERBOSE_DEFAULT   false

/** هل نحلل الأداء؟ */
#define DHAD_SIM_PERF_DEFAULT      false

/** هل نستخدم ألوان ANSI؟ */
#define DHAD_SIM_COLOR_DEFAULT     true

/** هل نطبع إحصائيات النهاية؟ */
#define DHAD_SIM_STATS_DEFAULT     true

/* ═══════════════════════════════════════════════════════════════════════════════
 *  الفصل 20: ألوان أسماء السجلات (Register Name Colors for Display)
 * ═══════════════════════════════════════════════════════════════════════════════ */

#define DHAD_COLOR_REG_0          DHAD_COLOR_CYAN
#define DHAD_COLOR_REG_1          DHAD_COLOR_GREEN
#define DHAD_COLOR_REG_2          DHAD_COLOR_YELLOW
#define DHAD_COLOR_REG_3          DHAD_COLOR_MAGENTA
#define DHAD_COLOR_REG_4          DHAD_COLOR_CYAN
#define DHAD_COLOR_REG_5          DHAD_COLOR_GREEN
#define DHAD_COLOR_REG_6          DHAD_COLOR_YELLOW
#define DHAD_COLOR_REG_7          DHAD_COLOR_MAGENTA
#define DHAD_COLOR_REG_ACC        DHAD_COLOR_RED
#define DHAD_COLOR_REG_FLAGS      DHAD_COLOR_BLUE

/* ═══════════════════════════════════════════════════════════════════════════════
 *  الفصل 21: خطة التوسع (Expansion Roadmap)
 *
 *  ╔═══════════════════════════════════════════════════════════════════╗
 *  ║ المرحلة  │ العرض  │ العنوان │ التعليمات │ الذاكرة │ الحالة      ║
 *  ╠══════════╪════════╪════════╪══════════╪════════╪══════════════╣
 *  ║ v1 (4b)  │ 4-bit  │ 12-bit │ 20+      │ 4KB    │ ✅ مكتمل    ║
 *  ║ v2 (8b)  │ 8-bit  │ 16-bit │ 128+     │ 64KB   │ 🔜 مستقبل  ║
 *  ║ v3 (16b) │ 16-bit │ 20-bit │ 512+     │ 1MB    │ 🔜 مستقبل  ║
 *  ║ v4 (32b) │ 32-bit │ 32-bit │ 1024+    │ 4GB    │ 🔜 مستقبل  ║
 *  ╚═══════════════════════════════════════════════════════════════════╝
 * ═══════════════════════════════════════════════════════════════════════════════ */

/** إصدارات المعمارية المستقبلية */
#define DHAD_VERSION_1_0_0        "1.0.0"     /* 4-bit — قديم */
#define DHAD_VERSION_2_0_0        "2.0.0"     /* 8-bit — الحالي */
#define DHAD_VERSION_3_0_0        "3.0.0"     /* 16-bit — مستقبل */
#define DHAD_VERSION_4_0_0        "4.0.0"     /* 32-bit — مستقبل */

/* ═══════════════════════════════════════════════════════════════════════════════
 *  الفصل 22: جدول التوافق (Compatibility Table)
 *
 *  يوضح أي الميزات متاحة في أي إصدار:
 *
 *  ╔══════════════════════════╦═══════╦═══════╦═══════╦═══════╗
 *  ║ الميزة                  ║ v1    ║ v2    ║ v3    ║ v4    ║
 *  ╠══════════════════════════╬═══════╬═══════╬═══════╬═══════╣
 *  ║ 4-bit arithmetic        ║  ✅  ║  ✅  ║  ✅  ║  ✅  ║
 *  ║ 8-bit arithmetic        ║  ❌  ║  ✅  ║  ✅  ║  ✅  ║
 *  ║ 16-bit arithmetic       ║  ❌  ║  ❌  ║  ✅  ║  ✅  ║
 *  ║ 32-bit arithmetic       ║  ❌  ║  ❌  ║  ❌  ║  ✅  ║
 *  ║ 8 registers             ║  ✅  ║  ✅  ║  ✅  ║  ✅  ║
 *  ║ 16 registers            ║  ❌  ║  ✅  ║  ✅  ║  ✅  ║
 *  ║ 32 registers            ║  ❌  ║  ❌  ║  ❌  ║  ✅  ║
 *  ║ Memory-mapped I/O       ║  ✅  ║  ✅  ║  ✅  ║  ✅  ║
 *  ║ Interrupts              ║  ❌  ║  ✅  ║  ✅  ║  ✅  ║
 *  ║ Protected mode          ║  ❌  ║  ❌  ║  ❌  ║  ✅  ║
 *  ║ Floating point          ║  ❌  ║  ❌  ║  ✅  ║  ✅  ║
 *  ║ Virtual memory          ║  ❌  ║  ❌  ║  ❌  ║  ✅  ║
 *  ╚══════════════════════════╩═══════╩═══════╩═══════╩═══════╝
 * ═══════════════════════════════════════════════════════════════════════════════ */

/* ═══════════════════════════════════════════════════════════════════════════════
 *  الفصل 23: ملاحظات للمطورين (Developer Notes)
 *
 *  1. هذا الملف هو source of truth الوحيد للمعمارية.
 *  2. أي تغيير في أي قيمة هنا يجب أن يُعكس في:
 *     - المحاكي (src/main.c أو ملفات منفصلة)
 *     - المجمّع (Assembler)
 *     - المترجم (Compiler backend)
 *     - التوثيق (docs/)
 *  3. لا تُضاف تعليمات جديدة دون تحديث هذا الملف أولاً.
 *  4. القيم المحجوزة (Reserved) لا يمكن استخدامها حتى تُعاد تسميتها.
 *  5. اختبر جميع التغييرات على Programs اختبارية قبل الدمج.
 * ═══════════════════════════════════════════════════════════════════════════════ */

#endif /* DHAD_ISA_CONSTANTS_H */

/*
 * ═══════════════════════════════════════════════════════════════════════════════
 *  نهاية ملف المرجع — dhad_isa_constants.h
 *  الإصدار: 1.0.0
 *  آخر تحديث: 2026-08-26
 * ═══════════════════════════════════════════════════════════════════════════════
 */
