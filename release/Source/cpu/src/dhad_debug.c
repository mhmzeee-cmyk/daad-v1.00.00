/**
 * ═══════════════════════════════════════════════════════════════
 *  dhad_debug.c — تنفيذ نظام تشخيص أخطاء المعالج ض
 * ═══════════════════════════════════════════════════════════════
 */

#include "dhad_debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ═══════════════════════════════════════
 *  أسماء السجلات والتعليمات
 * ═══════════════════════════════════════ */

static const char *REG_NAMES[] = {
    "س0", "س1", "س2", "س3", "س4", "س5", "س6", "س7", "مح"
};

static const char *REG_NAMES_EN[] = {
    "R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7", "ACC"
};

const char* dbg_reg_name(int reg) {
    if (reg >= 0 && reg <= 8) return REG_NAMES[reg];
    return "؟";
}

const char* dbg_opcode_name(uint8_t opcode, uint8_t ext) {
    switch (opcode) {
    case 0x0: return "سكون";
    case 0x1: return "حمّل";
    case 0x2: return "نقل";
    case 0x3: return "اقرأ";
    case 0x4: return "خزن";
    case 0x5: return "ادفع";
    case 0x6: return "اسحب";
    case 0x7: return "نداء";
    case 0x8: return "إرجاع";
    case 0x9: return "جمع";
    case 0xA: return "طرح";
    case 0xB: return "ضرب";
    case 0xC: return "قسمة";
    case 0xD: return "باقي";
    case 0xE: return "اطبع";
    case 0xF:
        switch (ext) {
        case 0x00: return "قفز";
        case 0x10: return "قفز.صفر";
        case 0x20: return "قفز.غيرصفر";
        case 0x30: return "قفز.حمل";
        case 0x40: return "قفز.سالب";
        case 0x50: return "اطبع.حرف";
        case 0x60: return "ادخل";
        case 0x70: return "أوحصري";
        case 0x80: return "أومنطقي";
        case 0x90: return "ومنطقي";
        case 0xA0: return "ليس";
        case 0xB0: return "تحريك.يسار";
        case 0xC0: return "تحريك.يمين";
        case 0xD0: return "بدل";
        case 0xD1: return "نقص";
        case 0xD2: return "وزد";
        case 0xD3: return "توقف";
        case 0xE0: return "قارن";
        case 0xE1: return "فعّل.مقاطعات";
        case 0xE2: return "عطّل.مقاطعات";
        case 0xE3: return "إرجاع.مقاطعة";
        default:   return "موسعة؟";
        }
    default: return "؟";
    }
}

int dbg_parse_reg(const char *s) {
    if (!s || !*s) return -1;
    for (int i = 0; i < 9; i++) {
        if (strcmp(s, REG_NAMES[i]) == 0 || strcmp(s, REG_NAMES_EN[i]) == 0)
            return i;
    }
    return -1;
}

/* ═══════════════════════════════════════
 *  التهيئة
 * ═══════════════════════════════════════ */

void dbg_init(DhadDebugger *dbg) {
    memset(dbg, 0, sizeof(*dbg));
    for (int i = 0; i < DBG_MAX_BREAKPOINTS; i++)
        dbg->breakpoints[i].slot = -1;
    for (int i = 0; i < DBG_MAX_WATCHPOINTS; i++)
        dbg->watchpoints[i].slot = -1;
    dbg->trace_enabled = true;
    dbg->max_cycles = 100000;
    dbg->single_step = false;
    dbg->running = false;
    dbg->stop_reason = STOP_NONE;
}

void dbg_reset(DhadDebugger *dbg) {
    int verbose = dbg->verbose;
    dbg_init(dbg);
    dbg->verbose = verbose;
}

void dbg_load_labels(DhadDebugger *dbg, const char *names[], uint16_t addrs[], int count) {
    dbg->label_count = count > DBG_MAX_LABELS ? DBG_MAX_LABELS : count;
    for (int i = 0; i < dbg->label_count; i++) {
        snprintf(dbg->labels[i].name, sizeof(dbg->labels[i].name), "%s", names[i]);
        dbg->labels[i].addr = addrs[i];
    }
}

/* ═══════════════════════════════════════
 *  نقاط التوقف
 * ═══════════════════════════════════════ */

static int find_free_bp_slot(DhadDebugger *dbg) {
    for (int i = 0; i < DBG_MAX_BREAKPOINTS; i++) {
        if (dbg->breakpoints[i].slot == -1)
            return i;
    }
    return -1;
}

int dbg_bp_add(DhadDebugger *dbg, uint16_t addr) {
    int slot = find_free_bp_slot(dbg);
    if (slot < 0) return -1;
    Breakpoint *bp = &dbg->breakpoints[slot];
    memset(bp, 0, sizeof(*bp));
    bp->address = addr;
    bp->type = BP_EXECUTE;
    bp->state = BP_ENABLED;
    bp->slot = slot;
    dbg->bp_count++;
    return slot;
}

int dbg_bp_add_label(DhadDebugger *dbg, const char *label) {
    for (int i = 0; i < dbg->label_count; i++) {
        if (strcmp(dbg->labels[i].name, label) == 0) {
            int slot = dbg_bp_add(dbg, dbg->labels[i].addr);
            if (slot >= 0) {
                snprintf(dbg->breakpoints[slot].label, 64, "%s", label);
            }
            return slot;
        }
    }
    return -1;
}

int dbg_bp_add_conditional(DhadDebugger *dbg, uint16_t addr,
                           uint8_t reg, uint8_t op, uint8_t val) {
    int slot = dbg_bp_add(dbg, addr);
    if (slot < 0) return -1;
    Breakpoint *bp = &dbg->breakpoints[slot];
    bp->type = BP_CONDITIONAL;
    bp->cond_reg = reg;
    bp->cond_op = op;
    bp->cond_val = val;
    return slot;
}

void dbg_bp_enable(DhadDebugger *dbg, int slot, bool enable) {
    if (slot < 0 || slot >= DBG_MAX_BREAKPOINTS) return;
    if (dbg->breakpoints[slot].slot == -1) return;
    dbg->breakpoints[slot].state = enable ? BP_ENABLED : BP_DISABLED;
}

void dbg_bp_remove(DhadDebugger *dbg, int slot) {
    if (slot < 0 || slot >= DBG_MAX_BREAKPOINTS) return;
    dbg->breakpoints[slot].slot = -1;
    dbg->bp_count--;
}

bool dbg_bp_check(DhadDebugger *dbg, uint16_t pc, bool *hit) {
    *hit = false;
    for (int i = 0; i < DBG_MAX_BREAKPOINTS; i++) {
        Breakpoint *bp = &dbg->breakpoints[i];
        if (bp->slot == -1) continue;
        if (bp->state == BP_DISABLED) continue;
        if (bp->address != pc) continue;
        if (bp->type == BP_EXECUTE || bp->type == BP_CONDITIONAL) {
            bp->hit_count++;
            dbg->bp_hits++;
            *hit = true;
            if (bp->state == BP_ONE_SHOT)
                bp->state = BP_DISABLED;
            return true;
        }
    }
    return false;
}

void dbg_bp_list(DhadDebugger *dbg) {
    printf("  ╔═══════╤══════════╤═══════════╤════════════════════════╗\n");
    printf("  ║  #    │ العنوان  │ الحالة    │ الوصف                   ║\n");
    printf("  ╠═══════╪══════════╪═══════════╪════════════════════════╣\n");
    int count = 0;
    for (int i = 0; i < DBG_MAX_BREAKPOINTS; i++) {
        Breakpoint *bp = &dbg->breakpoints[i];
        if (bp->slot == -1) continue;
        count++;
        printf("  ║  %-3d  │ 0x%04X   │ %-7s │ %s (اصابات: %u)\n",
               i, bp->address,
               bp->state == BP_ENABLED ? "مفعّل" :
               bp->state == BP_ONE_SHOT ? "مرة واحدة" : "معطّل",
               bp->label[0] ? bp->label : "",
               bp->hit_count);
    }
    if (count == 0) printf("  ║  (لا توجد نقاط توقف)                              ║\n");
    printf("  ╚═══════╧══════════╧═══════════╧════════════════════════╝\n");
}

/* ═══════════════════════════════════════
 *  نقاط المراقبة
 * ═══════════════════════════════════════ */

static int find_free_wp_slot(DhadDebugger *dbg) {
    for (int i = 0; i < DBG_MAX_WATCHPOINTS; i++) {
        if (dbg->watchpoints[i].slot == -1)
            return i;
    }
    return -1;
}

int dbg_wp_add_memory(DhadDebugger *dbg, uint16_t addr, WatchpointMode mode) {
    int slot = find_free_wp_slot(dbg);
    if (slot < 0) return -1;
    Watchpoint *wp = &dbg->watchpoints[slot];
    memset(wp, 0, sizeof(*wp));
    wp->target = WP_MEMORY;
    wp->target_addr = addr;
    wp->mode = mode;
    wp->slot = slot;
    dbg->wp_count++;
    return slot;
}

int dbg_wp_add_register(DhadDebugger *dbg, uint8_t reg, WatchpointMode mode) {
    int slot = find_free_wp_slot(dbg);
    if (slot < 0) return -1;
    Watchpoint *wp = &dbg->watchpoints[slot];
    memset(wp, 0, sizeof(*wp));
    wp->target = WP_REGISTER;
    wp->target_reg = reg;
    wp->mode = mode;
    wp->slot = slot;
    dbg->wp_count++;
    return slot;
}

int dbg_wp_add_acc(DhadDebugger *dbg, WatchpointMode mode) {
    int slot = find_free_wp_slot(dbg);
    if (slot < 0) return -1;
    Watchpoint *wp = &dbg->watchpoints[slot];
    memset(wp, 0, sizeof(*wp));
    wp->target = WP_ACCUMULATOR;
    wp->mode = mode;
    wp->slot = slot;
    dbg->wp_count++;
    return slot;
}

void dbg_wp_remove(DhadDebugger *dbg, int slot) {
    if (slot < 0 || slot >= DBG_MAX_WATCHPOINTS) return;
    dbg->watchpoints[slot].slot = -1;
    dbg->wp_count--;
}

void dbg_wp_check(DhadDebugger *dbg, uint16_t addr,
                  uint8_t reg_id, uint8_t val, bool is_write) {
    for (int i = 0; i < DBG_MAX_WATCHPOINTS; i++) {
        Watchpoint *wp = &dbg->watchpoints[i];
        if (wp->slot == -1) continue;

        bool match = false;
        switch (wp->target) {
        case WP_MEMORY:
            if (wp->target_addr == addr) {
                if (wp->mode == WP_CHANGE && wp->old_value != val)
                    match = true;
                else if (wp->mode == WP_WRITE_ACCESS && is_write)
                    match = true;
                else if (wp->mode == WP_READ_ACCESS && !is_write)
                    match = true;
                wp->old_value = wp->new_value;
                wp->new_value = val;
            }
            break;
        case WP_REGISTER:
            if (wp->target_reg == reg_id) {
                if (wp->mode == WP_CHANGE && wp->old_value != val)
                    match = true;
                wp->old_value = wp->new_value;
                wp->new_value = val;
            }
            break;
        case WP_ACCUMULATOR:
            if (wp->mode == WP_CHANGE && wp->old_value != val)
                match = true;
            wp->old_value = wp->new_value;
            wp->new_value = val;
            break;
        default:
            break;
        }

        if (match) {
            wp->triggered = true;
            dbg->wp_triggers++;
            dbg->stop_reason = STOP_WATCHPOINT;
            dbg->running = false;
            snprintf(dbg->last_msg, sizeof(dbg->last_msg),
                     "مراقِب #%d: تغيير في %s = %d",
                     i,
                     wp->target == WP_MEMORY ? "الذاكرة" :
                     wp->target == WP_REGISTER ? dbg_reg_name(wp->target_reg) : "مح",
                     val);
        }
    }
}

void dbg_wp_list(DhadDebugger *dbg) {
    printf("  ╔═══════╤════════════╤════════════╤══════════════════╗\n");
    printf("  ║  #    │ الهدف      │ الوضع      │ القيمة القديمة  ║\n");
    printf("  ╠═══════╪════════════╪════════════╪══════════════════╣\n");
    int count = 0;
    for (int i = 0; i < DBG_MAX_WATCHPOINTS; i++) {
        Watchpoint *wp = &dbg->watchpoints[i];
        if (wp->slot == -1) continue;
        count++;
        const char *target_name;
        switch (wp->target) {
        case WP_MEMORY:      target_name = "ذاكرة"; break;
        case WP_REGISTER:    target_name = dbg_reg_name(wp->target_reg); break;
        case WP_ACCUMULATOR: target_name = "مح"; break;
        case WP_FLAGS:       target_name = "أعلام"; break;
        case WP_PC:          target_name = "عداد"; break;
        case WP_SP:          target_name = "مكدس"; break;
        default:             target_name = "؟"; break;
        }
        printf("  ║  %-3d  │ %-8s   │ %-8s   │ %3d              ║\n",
               i, target_name,
               wp->mode == WP_CHANGE ? "تغيير" :
               wp->mode == WP_WRITE_ACCESS ? "كتابة" : "قراءة",
               wp->old_value);
    }
    if (count == 0) printf("  ║  (لا توجد نقاط مراقبة)                            ║\n");
    printf("  ╚═══════╧════════════╧════════════╧══════════════════╝\n");
}

/* ═══════════════════════════════════════
 *  سجل التتبع
 * ═══════════════════════════════════════ */

void dbg_trace_enable(DhadDebugger *dbg, bool enable) {
    dbg->trace_enabled = enable;
}

void dbg_trace_record(DhadDebugger *dbg, uint16_t pc, uint8_t opcode,
                      uint8_t operand, uint8_t acc, uint8_t flags,
                      uint8_t sp, uint32_t cycle) {
    if (!dbg->trace_enabled) return;
    TraceEntry *e = &dbg->trace[dbg->trace_head];
    e->pc = pc;
    e->opcode = opcode;
    e->operand = operand;
    e->acc_before = acc;
    e->flags_before = flags;
    e->sp_before = sp;
    e->cycle = cycle;
    dbg->trace_head = (dbg->trace_head + 1) % DBG_TRACE_BUFFER_SIZE;
    if (dbg->trace_count < DBG_TRACE_BUFFER_SIZE)
        dbg->trace_count++;
}

void dbg_trace_dump(DhadDebugger *dbg, int count) {
    if (count <= 0 || count > dbg->trace_count)
        count = dbg->trace_count;

    printf("  ╔═════════╤══════════════╤══════════╤══════╤══════════╗\n");
    printf("  ║  الدورة │ العنوان     │ التعليمة │  مح  │  أعلام   ║\n");
    printf("  ╠═════════╪══════════════╪══════════╪══════╪══════════╣\n");

    int start = (dbg->trace_head - count + DBG_TRACE_BUFFER_SIZE) % DBG_TRACE_BUFFER_SIZE;
    for (int i = 0; i < count; i++) {
        int idx = (start + i) % DBG_TRACE_BUFFER_SIZE;
        TraceEntry *e = &dbg->trace[idx];
        uint8_t ext = e->operand;
        printf("  ║ %7u │ 0x%04X     │ %-8s │ %3d  │ %c%c%c      ║\n",
               e->cycle, e->pc,
               dbg_opcode_name((e->opcode >> 4) & 0xF, ext),
               e->acc_before,
               (e->flags_before & 0x01) ? 'Z' : '-',
               (e->flags_before & 0x02) ? 'N' : '-',
               (e->flags_before & 0x04) ? 'C' : '-');
    }
    printf("  ╚═════════╧══════════════╧══════════╧══════╧══════════╝\n");
}

void dbg_trace_search(DhadDebugger *dbg, uint16_t addr) {
    int found = 0;
    printf("  البحث في السجل عن العنوان 0x%04X:\n", addr);
    for (int i = 0; i < dbg->trace_count; i++) {
        int idx = (dbg->trace_head - dbg->trace_count + i + DBG_TRACE_BUFFER_SIZE) % DBG_TRACE_BUFFER_SIZE;
        TraceEntry *e = &dbg->trace[idx];
        if (e->pc == addr) {
            printf("    الدورة %u: 0x%04X  %s\n",
                   e->cycle, e->pc,
                   dbg_opcode_name((e->opcode >> 4) & 0xF, e->operand));
            found++;
        }
    }
    if (found == 0) printf("    (لم يتم العثور على شيء)\n");
}

/* ═══════════════════════════════════════
 *  فحص الحالة
 * ═══════════════════════════════════════ */

void dbg_print_registers(DhadDebugger *dbg, uint8_t regs[8], uint8_t acc, uint8_t flags) {
    (void)dbg;
    printf("  ┌─────────────────────────────────────────────┐\n");
    printf("  │               السجلات                       │\n");
    printf("  ├─────────────────────────────────────────────┤\n");
    for (int i = 0; i < 8; i += 2) {
        printf("  │  %s (%s): %3d (0x%02X)    %s (%s): %3d (0x%02X)  │\n",
               REG_NAMES[i], REG_NAMES_EN[i], regs[i], regs[i],
               REG_NAMES[i+1], REG_NAMES_EN[i+1], regs[i+1], regs[i+1]);
    }
    printf("  ├─────────────────────────────────────────────┤\n");
    printf("  │  مح (ACC):  %3d (0x%02X)                   │\n", acc, acc);
    printf("  │  أعلام:     %c%c%c  (%d)                     │\n",
           (flags & 0x01) ? 'Z' : '-',
           (flags & 0x02) ? 'N' : '-',
           (flags & 0x04) ? 'C' : '-',
           flags);
    printf("  └─────────────────────────────────────────────┘\n");
}

void dbg_print_memory(DhadDebugger *dbg, uint8_t *mem, uint16_t start, int len) {
    (void)dbg;
    printf("  الذاكرة [0x%04X - 0x%04X]:\n", start, start + len - 1);
    printf("  ");
    for (int i = 0; i < len; i++) {
        printf("%02X ", mem[start + i]);
        if ((i + 1) % 16 == 0) printf("\n  ");
    }
    if (len % 16 != 0) printf("\n");
}

void dbg_print_stack(DhadDebugger *dbg, uint16_t *stack, int sp) {
    (void)dbg;
    printf("  المكدس (SP=%d):\n", sp);
    if (sp == 0) {
        printf("    (فارغ)\n");
        return;
    }
    for (int i = sp - 1; i >= 0; i--) {
        printf("    [%d] 0x%04X%s\n", i, stack[i], i == sp-1 ? " ← أعلى" : "");
    }
}

void dbg_print_instruction(DhadDebugger *dbg, uint8_t *mem, uint16_t pc) {
    (void)dbg;
    uint8_t raw = mem[pc];
    uint8_t op = (raw >> 4) & 0x0F;
    uint8_t reg = raw & 0x0F;
    const char *name = dbg_opcode_name(op, 0);

    printf("  0x%04X: %02X ", pc, raw);

    if (op == 0xF) {
        uint8_t ext = mem[pc + 1];
        printf("%02X ", ext);
        name = dbg_opcode_name(op, ext);
        if (ext <= 0x40) {
            uint8_t lo = mem[pc + 2];
            uint8_t hi = mem[pc + 3];
            uint16_t addr = lo | ((uint16_t)hi << 8);
            printf("%02X %02X  -> %s 0x%04X", lo, hi, name, addr);
        } else {
            printf("        -> %s %s", name, dbg_reg_name(ext & 0x07));
        }
    } else if (op == 0x1 || op == 0x3 || op == 0x4) {
        uint8_t val = mem[pc + 1];
        printf("%02X     -> %s %s, %d", val, name, dbg_reg_name(reg), val);
    } else if (op == 0x7) {
        uint8_t lo = mem[pc + 1];
        uint8_t hi = mem[pc + 2];
        uint16_t addr = lo | ((uint16_t)hi << 8);
        printf("%02X %02X  -> %s 0x%04X", lo, hi, name, addr);
    } else {
        printf("        -> %s", name);
        if (reg < 8) printf(" %s", dbg_reg_name(reg));
    }
    printf("\n");
}

void dbg_print_state(DhadDebugger *dbg, void *cpu_state) {
    /* This function requires casting — done in the integration layer */
    (void)dbg;
    (void)cpu_state;
}

/* ═══════════════════════════════════════
 *  حلقة التشخيص التفاعلية
 * ═══════════════════════════════════════ */

void dbg_print_help(DhadDebugger *dbg) {
    (void)dbg;
    printf("  ╔════════════════════════════════════════════════════════════╗\n");
    printf("  ║               أوامر المصحح (Debugger)                    ║\n");
    printf("  ╠════════════════════════════════════════════════════════════╣\n");
    printf("  ║  n / next        — تنفيذ تعليمة واحدة                   ║\n");
    printf("  ║  c / continue    — الاستمرار حتى نقطة توقف               ║\n");
    printf("  ║  s / step        — الدخول في الدالة (نداء)               ║\n");
    printf("  ║  r / registers   — عرض السجلات                           ║\n");
    printf("  ║  m <adr> <len>   — عرض الذاكرة                          ║\n");
    printf("  ║  st / stack      — عرض المكدس                            ║\n");
    printf("  ║  bp <adr>        — إضافة نقطة توقف                       ║\n");
    printf("  ║  bp- <num>       — حذف نقطة توقف                         ║\n");
    printf("  ║  bpl             — عرض نقاط التوقف                        ║\n");
    printf("  ║  wp <adr> <mode> — إضافة نقطة مراقبة                    ║\n");
    printf("  ║  wpl             — عرض نقاط المراقبة                     ║\n");
    printf("  ║  t [n]           — عرض سجل التتبع                        ║\n");
    printf("  ║  i               — تفاصيل التعليمة الحالية               ║\n");
    printf("  ║  h               — هذه المساعدة                          ║\n");
    printf("  ║  q               — خروج                                   ║\n");
    printf("  ╚════════════════════════════════════════════════════════════╝\n");
}

bool dbg_handle_command(DhadDebugger *dbg, const char *cmd, void *cpu_state) {
    if (!cmd || !*cmd) return false;

    /* تخطي الفراغات */
    while (*cmd == ' ' || *cmd == '\t') cmd++;
    if (*cmd == '\0') return false;

    char buf[DBG_MAX_CMD_LEN];
    snprintf(buf, sizeof(buf), "%s", cmd);
    char *tok = strtok(buf, " \t");
    if (!tok) return false;

    /* n / next — خطوة واحدة */
    if (strcmp(tok, "n") == 0 || strcmp(tok, "next") == 0 ||
        strcmp(tok, "step") == 0 || strcmp(tok, "s") == 0) {
        dbg->single_step = true;
        dbg->running = true;
        dbg->stop_reason = STOP_SINGLE_STEP;
        return true;
    }

    /* c / continue — الاستمرار */
    if (strcmp(tok, "c") == 0 || strcmp(tok, "continue") == 0) {
        dbg->single_step = false;
        dbg->running = true;
        dbg->stop_reason = STOP_NONE;
        return true;
    }

    /* r / registers */
    if (strcmp(tok, "r") == 0 || strcmp(tok, "registers") == 0) {
        /* يُستدعى من الخارج */
        return false;
    }

    /* m <adr> <len> — الذاكرة */
    if (strcmp(tok, "m") == 0 || strcmp(tok, "memory") == 0) {
        return false; /* يُستدعى من الخارج */
    }

    /* st / stack */
    if (strcmp(tok, "st") == 0 || strcmp(tok, "stack") == 0) {
        return false;
    }

    /* bp <adr> */
    if (strcmp(tok, "bp") == 0 || strcmp(tok, "break") == 0) {
        char *arg = strtok(NULL, " \t");
        if (!arg) {
            dbg_bp_list(dbg);
            return true;
        }
        /* محاولة التسمية أولاً */
        int slot = dbg_bp_add_label(dbg, arg);
        if (slot < 0) {
            /* محاولة العنوان */
            uint16_t addr = (uint16_t)strtol(arg, NULL, 0);
            slot = dbg_bp_add(dbg, addr);
        }
        if (slot >= 0)
            printf("  تم إضافة نقطة توقف #%d\n", slot);
        else
            printf("  خطأ: لا يمكن إضافة نقطة التوقف\n");
        return true;
    }

    /* bp- <num> */
    if (strcmp(tok, "bp-") == 0 || strcmp(tok, "bpremove") == 0) {
        char *arg = strtok(NULL, " \t");
        if (arg) {
            int slot = atoi(arg);
            dbg_bp_remove(dbg, slot);
            printf("  تم حذف نقطة التوقف #%d\n", slot);
        }
        return true;
    }

    /* bpl */
    if (strcmp(tok, "bpl") == 0 || strcmp(tok, "bplist") == 0) {
        dbg_bp_list(dbg);
        return true;
    }

    /* wp <adr> <mode> */
    if (strcmp(tok, "wp") == 0 || strcmp(tok, "watch") == 0) {
        char *arg1 = strtok(NULL, " \t");
        char *arg2 = strtok(NULL, " \t");
        if (!arg1) {
            dbg_wp_list(dbg);
            return true;
        }
        WatchpointMode mode = WP_CHANGE;
        if (arg2) {
            if (strcmp(arg2, "r") == 0 || strcmp(arg2, "read") == 0)
                mode = WP_READ_ACCESS;
            else if (strcmp(arg2, "w") == 0 || strcmp(arg2, "write") == 0)
                mode = WP_WRITE_ACCESS;
        }
        uint16_t addr = (uint16_t)strtol(arg1, NULL, 0);
        int slot = dbg_wp_add_memory(dbg, addr, mode);
        if (slot >= 0)
            printf("  تم إضافة نقطة مراقبة #%d على 0x%04X\n", slot, addr);
        else
            printf("  خطأ: لا يمكن إضافة نقطة المراقبة\n");
        return true;
    }

    /* wpl */
    if (strcmp(tok, "wpl") == 0 || strcmp(tok, "wplist") == 0) {
        dbg_wp_list(dbg);
        return true;
    }

    /* t [n] */
    if (strcmp(tok, "t") == 0 || strcmp(tok, "trace") == 0) {
        char *arg = strtok(NULL, " \t");
        int count = arg ? atoi(arg) : 20;
        dbg_trace_dump(dbg, count);
        return true;
    }

    /* i */
    if (strcmp(tok, "i") == 0 || strcmp(tok, "info") == 0) {
        return false; /* يُستدعى من الخارج */
    }

    /* h */
    if (strcmp(tok, "h") == 0 || strcmp(tok, "help") == 0) {
        dbg_print_help(dbg);
        return true;
    }

    /* q */
    if (strcmp(tok, "q") == 0 || strcmp(tok, "quit") == 0) {
        dbg->running = false;
        dbg->stop_reason = STOP_HALT;
        return true;
    }

    printf("  أمر غير معروف: %s (اكتب h للمساعدة)\n", tok);
    return true;
}
