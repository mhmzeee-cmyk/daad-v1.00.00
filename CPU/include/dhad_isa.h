#ifndef DHAD_ISA_H
#define DHAD_ISA_H

#include <stdint.h>
#include <stdbool.h>

/* ─────────────────────────────────────────────
   معالج ض — تعريفات المعمارية (ISA)
   ───────────────────────────────────────────── */

/* عرض البيانات والعنوان */
#define DHAD_DATA_WIDTH     4
#define DHAD_ADDR_WIDTH     12
#define DHAD_MEM_SIZE       (1 << DHAD_ADDR_WIDTH)  /* 4096 */
#define DHAD_REG_COUNT      8
#define DHAD_STACK_SIZE     16
#define DHAD_IO_BASE        0x0F0

/* ── السجلات ── */
typedef enum {
    DHAD_REG_S0 = 0,
    DHAD_REG_S1,
    DHAD_REG_S2,
    DHAD_REG_S3,
    DHAD_REG_S4,
    DHAD_REG_S5,
    DHAD_REG_S6,
    DHAD_REG_S7,
    DHAD_REG_A,     /* المح (Accumulator) */
    DHAD_REG_F,     /* سجل الأعلام (Flags) */
    DHAD_REG_COUNT_REG
} DhadRegId;

/* ── الأعلام ── */
#define DHAD_FLAG_Z  (1 << 0)   /* صفر */
#define DHAD_FLAG_N  (1 << 1)   /* سالب */
#define DHAD_FLAG_C  (1 << 2)   /* حمل */
#define DHAD_FLAG_V  (1 << 3)   /* فيض */

/* ── تعريفات التعليمات (Opcodes) ── */

/* الفئة 0: تعليمات النظام والنقل */
#define DHAD_OP_NOP       0x0
/* DHAD_OP_HALT is an extended opcode (0xF0 0xD3), NOT a base opcode */
#define DHAD_OP_MOV_IMM   0x1  /* حمّل سج, قيمة (فوري) */
#define DHAD_OP_MOV_REG   0x2  /* نقل سج2, سج1 */
#define DHAD_OP_LOAD      0x3  /* اقرأ من الذاكرة: سج = الذاكرة[عنوان] */
#define DHAD_OP_STORE     0x4  /* اكتب للذاكرة: الذاكرة[عنوان] = سج */
#define DHAD_OP_PUSH      0x5  /* ادفع المح للمكدس */
#define DHAD_OP_POP       0x6  /* اسحب من المكدس للمح */
#define DHAD_OP_CALL      0x7  /* نداء: push PC, jump للعنوان */
#define DHAD_OP_RET       0x8  /* ارجاع: pop PC */

/* الفئة 1: التعليمات الحسابية (opcode بين سجلين) */
#define DHAD_OP_ADD       0x9   /* جمع */
#define DHAD_OP_SUB       0xA   /* طرح */
#define DHAD_OP_MUL       0xB   /* ضرب */
#define DHAD_OP_DIV       0xC   /* قسمة */
#define DHAD_OP_MOD       0xD   /* باقي القسمة */
#define DHAD_OP_AND       0xE   /* و منطقي */
#define DHAD_OP_OR        0xF   /* أو منطقي */

/* الفئة 2: تعليمات التحكم (قفز شرطي) */
#define DHAD_OP_JMP       0x10  /* قفز غير شرطي */
#define DHAD_OP_JZ        0x11  /* قفز إذا صفر */
#define DHAD_OP_JNZ       0x12  /* قفز إذا غير صفر */
#define DHAD_OP_JC        0x13  /* قفز إذا حمل */
#define DHAD_OP_JN        0x14  /* قفز إذا سالب */

/* الفئة 3: تعليمات منطقية إضافية */
#define DHAD_OP_XOR       0x15  /* أو حصري */
#define DHAD_OP_NOT       0x16  /* نفي */
#define DHAD_OP_SHL       0x17  /* تحريك لليسار */
#define DHAD_OP_SHR       0x18  /* تحريك لليمين */
#define DHAD_OP_NEG       0x19  /* تبريد */

/* الفئة 4: تعليمات المقارنة */
#define DHAD_OP_CMP       0x1A  /* مقارنة (مح - سج) فقط تحديث الأعلام */
#define DHAD_OP_CMP_IMM   0x1B  /* مقارنة مع قيمة فورية */

/* الفئة 5: تعليمات I/O */
#define DHAD_OP_PRINT     0x1C  /* طباعة المح */
#define DHAD_OP_INPUT     0x1D  /* إدخال للمح */
#define DHAD_OP_PRINT_CH  0x1E  /* طباعة حرف من المح */

/* 0x1F محجوز للمستقبل */

/* ── تنسيق التعليمة ── */
/*
 * التعليمة الأساسية: 8 بت
 * ┌──────────┬──────────┐
 * │ opcode   │ operand  │
 * │ (4 bit)  │ (4 bit)  │
 * └──────────┴──────────┘
 *
 * التعليمة الموسعة: 12 بت
 * ┌──────────┬──────────┬──────────┐
 * │ opcode   │ reg      │ imm/addr │
 * │ (4 bit)  │ (4 bit)  │ (4 bit)  │
 * └──────────┴──────────┴──────────┘
 *
 * التعليمة الكاملة (لل Addresses الكبيرة): 16 بت
 * ┌──────────┬──────────┬──────────┐
 * │ opcode   │ reg      │ addr_hi  │
 * │ (4 bit)  │ (4 bit)  │ (4 bit)  │
 * ├──────────┴──────────┼──────────┤
 * │      addr_lo        │ (fut)   │
 * │      (8 bit)        │ (4 bit) │
 * └─────────────────────┴──────────┘
 */

/* ── الهيكل الداخلي للتعليمة ── */
typedef struct {
    uint8_t opcode;
    uint8_t reg;
    uint8_t reg2;
    uint8_t imm;
    uint16_t addr;
    bool is_extended;
    bool is_16bit;
} DhadInstruction;

/* ── هيكل المعالج ── */
typedef struct {
    /* السجلات */
    uint8_t regs[DHAD_REG_COUNT];     /* س0-س7 */
    uint8_t acc;                        /* المح (Accumulator) */
    uint8_t flags;                      /* سجل الأعلام */

    /* عداد البرنامج */
    uint16_t pc;

    /* مؤشر المكدس */
    uint8_t sp;

    /* الذاكرة */
    uint8_t memory[DHAD_MEM_SIZE];

    /* المكدس (في الذاكرة) */
    uint16_t stack[DHAD_STACK_SIZE];

    /* الحالة */
    bool halted;
    bool running;
    uint32_t cycle_count;

    /* I/O */
    uint8_t io_out;
    uint8_t io_in;
    bool io_ready;

    /* سجلات الحالة */
    uint16_t last_fetch;
    uint8_t last_opcode;
} DhadCPU;

/* ── الدوال ── */

/* تهيئة المعالج */
void dhad_cpu_init(DhadCPU *cpu);

/* تنفيذ تعليمة واحدة */
void dhad_cpu_step(DhadCPU *cpu);

/* تشغيل حتى التوقف */
void dhad_cpu_run(DhadCPU *cpu, uint32_t max_cycles);

/* تحميل برنامج في الذاكرة */
int dhad_cpu_load_program(DhadCPU *cpu, const uint8_t *program, uint16_t size);

/* إعادة تعيين المعالج */
void dhad_cpu_reset(DhadCPU *cpu);

/* طباعة حالة المعالج */
void dhad_cpu_print_state(const DhadCPU *cpu);

/* ── دوال مساعدة ── */
void dhad_update_flags(DhadCPU *cpu, uint8_t result);
uint8_t dhad_fetch_byte(DhadCPU *cpu);
uint16_t dhad_fetch_word(DhadCPU *cpu);
void dhad_push_stack(DhadCPU *cpu, uint16_t value);
uint16_t dhad_pop_stack(DhadCPU *cpu);

/* ── أسماء السجلات بالعربي ── */
const char* dhad_reg_name(uint8_t reg_id);
const char* dhad_op_name(uint8_t opcode);

#endif /* DHAD_ISA_H */
