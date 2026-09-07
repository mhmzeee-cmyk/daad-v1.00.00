/**
 * ═══════════════════════════════════════════════════════════════
 *  dhad_debug.h — نظام تشخيص أخطاء المعالج ض (Debugger)
 *
 *  تصميم معياري وقابل للتوسع:
 *  - نقاط توقف (Breakpoints)
 *  - نقاط مراقبة (Watchpoints)
 *  - تتبع تنفيذ (Trace Log)
 *  - فحص الحالة (State Inspection)
 *  - وضع خطوة بخطوة (Single Step)
 * ═══════════════════════════════════════════════════════════════
 */

#ifndef DHAD_DEBUG_H
#define DHAD_DEBUG_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* ═══════════════════════════════════════
 *  الأنواع الأساسية
 * ═══════════════════════════════════════ */

#define DBG_MAX_BREAKPOINTS    32
#define DBG_MAX_WATCHPOINTS    16
#define DBG_TRACE_BUFFER_SIZE  256
#define DBG_MAX_LABELS         128
#define DBG_MAX_CMD_LEN        256

/* أنواع نقاط التوقف */
typedef enum {
    BP_EXECUTE,         /* توقف عند التنفيذ على عنوان */
    BP_READ,            /* توقف عند قراءة عنوان */
    BP_WRITE,           /* توقف عند كتابة عنوان */
    BP_CONDITIONAL      /* توقف عند تنفيذ + الشرط محقق */
} BreakpointType;

/* حالة نقطة التوقف */
typedef enum {
    BP_DISABLED,
    BP_ENABLED,
    BP_ONE_SHOT          /* توقف مرة واحدة ثم تعطّل */
} BreakpointState;

/* نقطة توقف */
typedef struct {
    uint16_t        address;
    BreakpointType  type;
    BreakpointState state;
    char            label[64];      /* اسم التسمية (اختياري) */
    uint32_t        hit_count;      /* عدد مرات الإصابة */
    /* شرط اختياري (BP_CONDITIONAL) */
    uint8_t         cond_reg;       /* رقم السجل (0-7, 8=مح, 255=أي) */
    uint8_t         cond_op;        /* عملية المقارنة: 0===, 1=!=, 2=>, 3=< */
    uint8_t         cond_val;       /* القيمة المقارنة */
    int             slot;           /* رقم الس_lot في المصفوفة (-1=غير مستخدم) */
} Breakpoint;

/* نقطة مراقبة */
typedef enum {
    WP_MEMORY,          /* مراقبة عنوان في الذاكرة */
    WP_REGISTER,        /* مراقبة سجل */
    WP_ACCUMULATOR,     /* مراقبة المح */
    WP_FLAGS,           /* مراقبة الأعلام */
    WP_PC,              /* مراقبة عداد البرنامج */
    WP_SP               /* مراقبة مؤشر المكدس */
} WatchpointTarget;

typedef enum {
    WP_NONE,
    WP_READ_ACCESS,
    WP_WRITE_ACCESS,
    WP_CHANGE          /* أي تغيير */
} WatchpointMode;

typedef struct {
    uint16_t            target_addr;   /* العنوان (لWP_MEMORY) */
    uint8_t             target_reg;    /* رقم السجل (لWP_REGISTER) */
    WatchpointTarget    target;
    WatchpointMode      mode;
    uint32_t            old_value;     /* القيمة السابقة */
    uint32_t            new_value;     /* القيمة الحالية */
    bool                triggered;
    char                label[64];
    int                 slot;
} Watchpoint;

/* سجل تتبع تنفيذ واحد */
typedef struct {
    uint16_t    pc;            /* عنوان التعليمة */
    uint8_t     opcode;        /* بايت التعليمة الأول */
    uint8_t     operand;       /* بايت التعليمة الثاني (إن وجد) */
    uint8_t     acc_before;    /* قيمة المح قبل التنفيذ */
    uint8_t     flags_before;  /* الأعلام قبل التنفيذ */
    uint8_t     sp_before;     /* المكدس قبل التنفيذ */
    uint32_t    cycle;         /* رقم الدورة */
    uint16_t    addr_accessed; /* العنوان الم accessed (إن وجد) */
} TraceEntry;

/* أسباب التوقف */
typedef enum {
    STOP_NONE,
    STOP_HALT,              /* تعليمة HALT */
    STOP_BREAKPOINT,        /* نقطة توقف */
    STOP_WATCHPOINT,        /* نقطة مراقبة */
    STOP_SINGLE_STEP,       /* خطوة بخطوة */
    STOP_MAX_CYCLES,        /* الحد الأقصى للدورات */
    STOP_ERROR              /* خطأ */
} StopReason;

/* ═══════════════════════════════════════
 *  هيكل Debugger الرئيسي
 * ═══════════════════════════════════════ */

typedef struct {
    /* ── نقاط التوقف ── */
    Breakpoint breakpoints[DBG_MAX_BREAKPOINTS];
    int        bp_count;

    /* ── نقاط المراقبة ── */
    Watchpoint watchpoints[DBG_MAX_WATCHPOINTS];
    int        wp_count;

    /* ── سجل التتبع ── */
    TraceEntry trace[DBG_TRACE_BUFFER_SIZE];
    int        trace_head;     /* المؤشر الدائري */
    int        trace_count;    /* عدد الإدخالات الفعلية */
    bool       trace_enabled;

    /* ── الجدولة ── */
    bool       single_step;    /* وضع الخطوة بخطوة */
    bool       running;        /* هل المعالج يعمل؟ */
    StopReason stop_reason;    /* سبب التوقف الحالي */
    uint32_t   max_cycles;     /* الحد الأقصى للدورات */
    uint32_t   step_count;     /* عدد خطوات التنفيذ */

    /* ── جدول التسميات (من المجمّع) ── */
    struct {
        char     name[64];
        uint16_t addr;
    } labels[DBG_MAX_LABELS];
    int label_count;

    /* ── إحصائيات ── */
    uint32_t total_instructions;
    uint32_t total_cycles;
    uint32_t bp_hits;
    uint32_t wp_triggers;

    /* ── مخرجات ── */
    char     last_msg[512];    /* آخر رسالة */
    bool     verbose;          /* وضع تفصيلي */
} DhadDebugger;

/* ═══════════════════════════════════════
 *  واجهة Debugger — دوال التهيئة
 * ═══════════════════════════════════════ */

/** تهيئة Debugger */
void dbg_init(DhadDebugger *dbg);

/** تفريغ جميع النقاط */
void dbg_reset(DhadDebugger *dbg);

/** تحميل تسميات من المجمّع */
void dbg_load_labels(DhadDebugger *dbg, const char *names[], uint16_t addrs[], int count);

/* ═══════════════════════════════════════
 *  نقاط التوقف (Breakpoints)
 * ═══════════════════════════════════════ */

/** إضافة نقطة توقف التنفيذ */
int  dbg_bp_add(DhadDebugger *dbg, uint16_t addr);

/** إضافة نقطة توقف بالتسمية */
int  dbg_bp_add_label(DhadDebugger *dbg, const char *label);

/** إضافة نقطة توقف شرطية */
int  dbg_bp_add_conditional(DhadDebugger *dbg, uint16_t addr,
                            uint8_t reg, uint8_t op, uint8_t val);

/** تفعيل/تعطيل نقطة توقف */
void dbg_bp_enable(DhadDebugger *dbg, int slot, bool enable);

/** حذف نقطة توقف */
void dbg_bp_remove(DhadDebugger *dbg, int slot);

/** فحص الوصول لنتيجة توقف */
bool dbg_bp_check(DhadDebugger *dbg, uint16_t pc, bool *hit);

/** عرض جميع نقاط التوقف */
void dbg_bp_list(DhadDebugger *dbg);

/* ═══════════════════════════════════════
 *  نقاط المراقبة (Watchpoints)
 * ═══════════════════════════════════════ */

/** إضافة مراقبة عنوان ذاكرة */
int  dbg_wp_add_memory(DhadDebugger *dbg, uint16_t addr, WatchpointMode mode);

/** إضافة مراقبة سجل */
int  dbg_wp_add_register(DhadDebugger *dbg, uint8_t reg, WatchpointMode mode);

/** إضافة مراقبة المح */
int  dbg_wp_add_acc(DhadDebugger *dbg, WatchpointMode mode);

/** حذف نقطة مراقبة */
void dbg_wp_remove(DhadDebugger *dbg, int slot);

/** فحص المراقبة (يُستدعى قبل/بعد كل تعليمة) */
void dbg_wp_check(DhadDebugger *dbg, uint16_t addr,
                  uint8_t reg_id, uint8_t val, bool is_write);

/** عرض جميع نقاط المراقبة */
void dbg_wp_list(DhadDebugger *dbg);

/* ═══════════════════════════════════════
 *  سجل التتبع (Trace Log)
 * ═══════════════════════════════════════ */

/** تفعيل/تعطيل التتبع */
void dbg_trace_enable(DhadDebugger *dbg, bool enable);

/** تسجيل تعليمة في السجل */
void dbg_trace_record(DhadDebugger *dbg, uint16_t pc, uint8_t opcode,
                      uint8_t operand, uint8_t acc, uint8_t flags,
                      uint8_t sp, uint32_t cycle);

/** عرض آخر N إدخالات */
void dbg_trace_dump(DhadDebugger *dbg, int count);

/** بحث في السجل */
void dbg_trace_search(DhadDebugger *dbg, uint16_t addr);

/* ═══════════════════════════════════════
 *  فحص الحالة (State Inspection)
 * ═══════════════════════════════════════ */

/** طباعة حالة المعالج كاملة */
void dbg_print_state(DhadDebugger *dbg, void *cpu_state);

/** طباعة السجلات */
void dbg_print_registers(DhadDebugger *dbg, uint8_t regs[8], uint8_t acc, uint8_t flags);

/** طباعة جزء من الذاكرة */
void dbg_print_memory(DhadDebugger *dbg, uint8_t *mem, uint16_t start, int len);

/** طباعة المكدس */
void dbg_print_stack(DhadDebugger *dbg, uint16_t *stack, int sp);

/** تفاصيل التعليمة الحالية */
void dbg_print_instruction(DhadDebugger *dbg, uint8_t *mem, uint16_t pc);

/* ═══════════════════════════════════════
 *  حلقة التشخيص التفاعلية
 * ═══════════════════════════════════════ */

/** معالج أمر نصي */
bool dbg_handle_command(DhadDebugger *dbg, const char *cmd, void *cpu_state);

/** عرض المساعدة */
void dbg_print_help(DhadDebugger *dbg);

/* ═══════════════════════════════════════
 *  دوال مساعدة
 * ═══════════════════════════════════════ */

/** اسم السجل بالعربي */
const char* dbg_reg_name(int reg);

/** اسم التعليمة بالعربي */
const char* dbg_opcode_name(uint8_t opcode, uint8_t ext);

/** تحليل رقم السجل من نص */
int  dbg_parse_reg(const char *s);

#endif /* DHAD_DEBUG_H */
