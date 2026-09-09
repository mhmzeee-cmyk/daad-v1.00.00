/* ═══════════════════════════════════════════════════════════════════════════
   معالج ض — المحاكي والمجمّع الرئيسي (8-bit)
   Dhad Processor v4.0 — Simulator & Assembler & Debugger
   ═══════════════════════════════════════════════════════════════════════════ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include "dhad_debug.h"

/* ═══════════════════════════════════════
   تعريفات المعمارية (ISA v2.0 — 8-bit)
   ═══════════════════════════════════════ */

#define MEM_SIZE    65536
#define STACK_SIZE  32
#define REG_COUNT   8
#define PROG_START  0x0000
#define PROG_MAX    0xF000

#define FLAG_Z  (1 << 0)
#define FLAG_N  (1 << 1)
#define FLAG_C  (1 << 2)

/* ═══════════════════════════════════════
   هيكل المعالج
   ═══════════════════════════════════════ */

typedef struct {
    uint8_t  regs[REG_COUNT];
    uint8_t  acc;
    uint8_t  flags;
    uint16_t pc;
    uint8_t  sp;
    uint8_t  memory[MEM_SIZE];
    uint16_t stack[STACK_SIZE];
    bool     halted;
    uint32_t cycles;
    bool     int_enabled;
    uint8_t  int_mask;
    uint8_t  int_pending;
    uint16_t ivt_addr;
} CPU;

/* ═══════════════════════════════════════
   هيكل المجمّع
   ═══════════════════════════════════════ */

typedef struct {
    char    name[64];
    uint16_t addr;
} Label;

typedef struct {
    char    name[64];
    int     value;
} Equate;

#define MAX_MACRO_LINES 32
typedef struct {
    char    name[64];
    char    lines[MAX_MACRO_LINES][256];
    int     line_count;
} Macro;

#define MAX_IF_NESTING 32
typedef struct {
    Label    labels[256];
    int      label_count;
    Equate   equates[256];
    int      equate_count;
    Macro    macros[64];
    int      macro_count;
    bool     in_macro_def;
    char     current_macro[64];
    int      if_nesting;
    bool     if_skip_stack[MAX_IF_NESTING];
    int      error_count;
    uint8_t  program[PROG_MAX];
    int      prog_size;
    int      pass;
    int      line;
} Asm;

/* ═══════════════════════════════════════
   دوال المعالج
   ═══════════════════════════════════════ */

static void cpu_init(CPU *c) {
    memset(c, 0, sizeof(CPU));
    c->pc = PROG_START;
}

static void update_flags(CPU *c, uint16_t result) {
    c->flags = 0;
    if ((result & 0xFF) == 0) c->flags |= FLAG_Z;
    if (result & 0x80) c->flags |= FLAG_N;
}

static void cpu_reset(CPU *c) {
    memset(c, 0, sizeof(*c));
    c->ivt_addr = 0x0040;
    c->int_mask = 0xFF;
}

static void cpu_trigger_interrupt(CPU *c, int irq) {
    if (irq >= 0 && irq < 8)
        c->int_pending |= (1 << irq);
}

static uint8_t fetch(CPU *c) {
    uint8_t b = c->memory[c->pc];
    c->pc++;
    return b;
}

static uint16_t fetch16(CPU *c) {
    uint8_t lo = fetch(c);
    uint8_t hi = fetch(c);
    return (uint16_t)((hi << 8) | lo);
}

static uint8_t cpu_get_reg(CPU *c, uint8_t r) {
    return (r == 8) ? c->acc : c->regs[r & 0x07];
}

static void cpu_set_reg(CPU *c, uint8_t r, uint8_t v) {
    if (r == 8) c->acc = v; else c->regs[r & 0x07] = v;
}

static void push(CPU *c, uint16_t v) {
    if (c->sp < STACK_SIZE - 1) c->stack[c->sp++] = v;
}

static uint16_t pop(CPU *c) {
    if (c->sp > 0) return c->stack[--c->sp];
    return 0;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  مجموعة التعليمات — 8-bit
 *
 *  Byte 1: [opcode:4][reg:4]     (reg: 0-7=سجل, 8=مح, 9=مح)
 *  Byte 2: قيمة فورية / عنوان منخفض (لتعليمات الحمّل/اقرأ/خزن/نداء)
 *  Byte 3: عنوان مرتفع (للقفز فقط)
 *
 *  0x0: توقف          0x8: إرجاع/تبريد
 *  0x1: حمّل سج,imm8  0x9: جمع مح,سج
 *  0x2: نقل dst,src   0xA: طرح مح,سج
 *  0x3: اقرأ سج,addr  0xB: ضرب مح,سج
 *  0x4: خزن سج,addr   0xC: قسمة مح,سج
 *  0x5: ادفع          0xD: باقي مح,سج
 *  0x6: اسحب          0xE: اطبع
 *  0x7: نداء addr16   0xF: موسعة (قفز/منطق/I-O)
 * ═══════════════════════════════════════════════════════════════════════════ */

static void cpu_step(CPU *c) {
    if (c->halted) return;

    /* ── فحص المقاطعات ── */
    if (c->int_enabled) {
        uint8_t pending = c->int_pending & c->int_mask;
        if (pending) {
            for (int i = 0; i < 8; i++) {
                if (pending & (1 << i)) {
                    c->int_pending &= ~(1 << i);
                    push(c, c->flags);
                    push(c, c->pc);
                    c->int_enabled = false;
                    uint16_t handler = c->ivt_addr + (i * 4);
                    c->pc = handler;
                    c->cycles += 3;
                    return;
                }
            }
        }
    }

    uint8_t raw = fetch(c);
    uint8_t op = (raw >> 4) & 0x0F;
    uint8_t reg = raw & 0x0F;
    c->cycles++;

    switch (op) {

    /* ── سكون (NOP) ── */
    case 0x0:
        break;

    /* ── حمّل سج, قيمة8 ── */
    case 0x1: {
        uint8_t val = fetch(c);
        if (reg < 8) c->regs[reg] = val;
        else         c->acc = val;
        break;
    }

    /* ── نقل dst, src — بايت1=المصدر، بايت2=الوجه ── */
    case 0x2: {
        uint8_t dst = fetch(c);
        cpu_set_reg(c, dst, cpu_get_reg(c, reg));
        break;
    }

    /* ── اقرأ سج, عنوان8 ── */
    case 0x3: {
        uint8_t addr = fetch(c);
        uint8_t v = c->memory[addr];
        if (reg < 8) c->regs[reg] = v;
        else         c->acc = v;
        break;
    }

    /* ── خزن سج, عنوان8 ── */
    case 0x4: {
        uint8_t addr = fetch(c);
        uint8_t v = (reg < 8) ? c->regs[reg] : c->acc;
        c->memory[addr] = v;
        break;
    }

    /* ── ادفع (push acc) ── */
    case 0x5:
        push(c, c->acc);
        break;

    /* ── اسحب (pop → acc) ── */
    case 0x6:
        c->acc = (uint8_t)(pop(c) & 0xFF);
        break;

    /* ── نداء عنوان16 ── */
    case 0x7: {
        uint16_t addr = fetch16(c);
        push(c, c->pc);
        c->pc = addr;
        break;
    }

    /* ── إرجاع / تبريد ── */
    case 0x8:
        if (reg == 0) {
            c->pc = pop(c);
        } else {
            c->acc = (uint8_t)(-(int8_t)c->acc);
            update_flags(c, c->acc);
        }
        break;

    /* ── جمع ── */
    case 0x9: {
        uint8_t v = cpu_get_reg(c, reg);
        uint16_t r = (uint16_t)c->acc + (uint16_t)v;
        c->acc = (uint8_t)r;
        c->flags = 0;
        if (c->acc == 0) c->flags |= FLAG_Z;
        if (c->acc & 0x80) c->flags |= FLAG_N;
        if (r > 0xFF) c->flags |= FLAG_C;
        break;
    }

    /* ── طرح ── */
    case 0xA: {
        uint8_t v = cpu_get_reg(c, reg);
        uint16_t r = (uint16_t)c->acc - (uint16_t)v;
        c->acc = (uint8_t)r;
        c->flags = 0;
        if (c->acc == 0) c->flags |= FLAG_Z;
        if (c->acc & 0x80) c->flags |= FLAG_N;
        if (r > 0xFF) c->flags |= FLAG_C;
        break;
    }

    /* ── ضرب ── */
    case 0xB: {
        uint8_t v = cpu_get_reg(c, reg);
        uint16_t r = (uint16_t)c->acc * (uint16_t)v;
        c->acc = (uint8_t)(r & 0xFF);
        update_flags(c, c->acc);
        break;
    }

    /* ── قسمة ── */
    case 0xC: {
        uint8_t v = cpu_get_reg(c, reg);
        if (v == 0) { c->halted = true; break; }
        c->acc = c->acc / v;
        update_flags(c, c->acc);
        break;
    }

    /* ── باقي ── */
    case 0xD: {
        uint8_t v = cpu_get_reg(c, reg);
        if (v == 0) { c->halted = true; break; }
        c->acc = c->acc % v;
        update_flags(c, c->acc);
        break;
    }

    /* ── اطبع (رقم) ── */
    case 0xE:
        printf("%d", c->acc);
        break;

    /* ── تعليمات موسعة ── */
    case 0xF: {
        uint8_t ext = fetch(c);
        switch (ext) {

        /* ── قفزات: [ext][addr_lo][addr_hi] ── */
        case 0x00: { /* JMP */
            uint16_t addr = fetch16(c);
            c->pc = addr;
            break;
        }
        case 0x10: { /* JZ */
            uint16_t addr = fetch16(c);
            if (c->flags & FLAG_Z) c->pc = addr;
            break;
        }
        case 0x20: { /* JNZ */
            uint16_t addr = fetch16(c);
            if (!(c->flags & FLAG_Z)) c->pc = addr;
            break;
        }
        case 0x30: { /* JC */
            uint16_t addr = fetch16(c);
            if (c->flags & FLAG_C) c->pc = addr;
            break;
        }
        case 0x40: { /* JN */
            uint16_t addr = fetch16(c);
            if (c->flags & FLAG_N) c->pc = addr;
            break;
        }

        /* ── I/O ── */
        case 0x50: { /* طباعة حرف */
            printf("%c", c->acc);
            break;
        }
        case 0x60: { /* إدخال */
            int v = 0;
            if (scanf("%d", &v) == 1) {
                c->acc = (uint8_t)(v & 0xFF);
            }
            break;
        }

        /* ── NOT (لا يحتاج سجل) ── */
        case 0xA0: { /* NOT */
            c->acc = ~c->acc;
            update_flags(c, c->acc);
            break;
        }
        /* ── SHL/SHR (لا تحتاج سجل) ── */
        case 0xB0: { /* SHL */
            c->flags = 0;
            if (c->acc & 0x80) c->flags |= FLAG_C;
            c->acc = (uint8_t)(c->acc << 1);
            if (c->acc == 0) c->flags |= FLAG_Z;
            if (c->acc & 0x80) c->flags |= FLAG_N;
            break;
        }
        case 0xC0: { /* SHR */
            c->flags = 0;
            if (c->acc & 0x01) c->flags |= FLAG_C;
            c->acc = (uint8_t)(c->acc >> 1);
            if (c->acc == 0) c->flags |= FLAG_Z;
            if (c->acc & 0x80) c->flags |= FLAG_N;
            break;
        }

        /* ── منطق ب регистр: يستخدم (ext & 0xF0) ── */
        default: {
            uint8_t ext_base = ext & 0xF0;
            if (ext_base == 0x70) { /* XOR */
                uint8_t v = cpu_get_reg(c, ext & 0x0F);
                c->acc ^= v;
                update_flags(c, c->acc);
            } else if (ext_base == 0x80) { /* OR */
                uint8_t v = cpu_get_reg(c, ext & 0x0F);
                c->acc |= v;
                update_flags(c, c->acc);
            } else if (ext_base == 0x90) { /* AND */
                uint8_t v = cpu_get_reg(c, ext & 0x0F);
                c->acc &= v;
                update_flags(c, c->acc);
            }
            break;
        }

        /* ── بدل (SWAP) — بايت2 = [dst:4][src:4] ── */
        case 0xD0: {
            uint8_t operands = fetch(c);
            uint8_t dst = (operands >> 4) & 0x0F;
            uint8_t src = operands & 0x0F;
            uint8_t *pd = (dst < 8) ? &c->regs[dst] : &c->acc;
            uint8_t *ps = (src < 8) ? &c->regs[src] : &c->acc;
            uint8_t tmp = *pd;
            *pd = *ps;
            *ps = tmp;
            break;
        }

        /* ── نقص (DEC) — سج-- ── */
        case 0xD1: {
            uint8_t r = fetch(c) & 0x0F;
            uint8_t v = cpu_get_reg(c, r);
            v--;
            cpu_set_reg(c, r, v);
            c->flags = 0;
            if (v == 0) c->flags |= FLAG_Z;
            if (v & 0x80) c->flags |= FLAG_N;
            break;
        }

        /* ── وزد (INC) — سج++ ── */
        case 0xD2: {
            uint8_t r = fetch(c) & 0x0F;
            uint8_t v = cpu_get_reg(c, r);
            v++;
            cpu_set_reg(c, r, v);
            c->flags = 0;
            if (v == 0) c->flags |= FLAG_Z;
            if (v & 0x80) c->flags |= FLAG_N;
            break;
        }

        /* ── توقف (HALT) ── */
        case 0xD3: {
            c->halted = true;
            break;
        }

        /* ── قارن (CMP) — مح - سج (الأعلام فقط)
           0xE0 + 0xE4-0xE7 (0xE1-0xE3 محجوزة لـ EI/DI/RETI — يتجنبها الـ backend) ── */
        case 0xE0:
        case 0xE4: case 0xE5: case 0xE6: case 0xE7: {
            uint8_t v = cpu_get_reg(c, ext & 0x0F);
            uint16_t r = (uint16_t)c->acc - (uint16_t)v;
            c->flags = 0;
            if ((r & 0xFF) == 0) c->flags |= FLAG_Z;
            if (r & 0x80) c->flags |= FLAG_N;
            if (r > 0xFF) c->flags |= FLAG_C;
            break;
        }

        /* ── EI: تفعيل المقاطعات ── */
        case 0xE1:
            c->int_enabled = true;
            break;

        /* ── DI: تعطيل المقاطعات ── */
        case 0xE2:
            c->int_enabled = false;
            break;

        /* ── RETI: العودة من المقاطعة ── */
        case 0xE3: {
            uint16_t ret_addr = pop(c);
            uint8_t ret_flags = (uint8_t)(pop(c) & 0xFF);
            c->pc = ret_addr;
            c->flags = ret_flags;
            c->int_enabled = true;
            break;
        }

        /* ── LDRI: dst = mem[(hi<<8)|lo] — 4 بايت حسب المواصفة ── */
        case 0xF1: {
            uint8_t operands = fetch(c);
            uint8_t loreg = fetch(c) & 0x0F;
            uint8_t dst = (operands >> 4) & 0x0F;
            uint8_t hi = operands & 0x0F;
            uint16_t addr = ((uint16_t)cpu_get_reg(c, hi) << 8) | cpu_get_reg(c, loreg);
            cpu_set_reg(c, dst, c->memory[addr]);
            break;
        }

        /* ── STRI: mem[(hi<<8)|lo] = src — 4 بايت حسب المواصفة ── */
        case 0xF2: {
            uint8_t operands = fetch(c);
            uint8_t loreg = fetch(c) & 0x0F;
            uint8_t src = (operands >> 4) & 0x0F;
            uint8_t hi = operands & 0x0F;
            uint16_t addr = ((uint16_t)cpu_get_reg(c, hi) << 8) | cpu_get_reg(c, loreg);
            c->memory[addr] = cpu_get_reg(c, src);
            break;
        }

        break;
        } /* end switch(ext) */
        break;
    } /* end case 0xF */

    default:
        break;
    }
}

/* ═══════════════════════════════════════
   المجمّع (Assembler)
   ═══════════════════════════════════════ */

static int is_space(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

static const char* skip_sp(const char *p) {
    while (*p) {
        if (is_space(*p) || *p == ',') { p++; continue; }
        if ((unsigned char)*p == 0xD8 && (unsigned char)*(p+1) == 0x8C) { p += 2; continue; }
        break;
    }
    return p;
}

static const char* read_token(const char *p, char *buf, int sz) {
    p = skip_sp(p);
    int i = 0;
    while (*p && i < sz - 1) {
        /* تخطي الفاصلة العربية (UTF-8: 0xD8 0x8C) */
        if ((unsigned char)*p == 0xD8 && (unsigned char)*(p+1) == 0x8C) break;
        if (is_space(*p) || *p == ',' || *p == ':') break;
        buf[i++] = *p++;
    }
    buf[i] = '\0';
    /* تخطي النقطتين بعد التسمية */
    if (*p == ':') p++;
    return p;
}

static int is_reg(const char *s, int *out) {
    static const char *names[] = {"س0","س1","س2","س3","س4","س5","س6","س7","مح"};
    for (int i = 0; i < 9; i++) {
        if (strcmp(s, names[i]) == 0) { *out = i; return 1; }
    }
    /* إنجليزي */
    static const char *en[] = {"s0","s1","s2","s3","s4","s5","s6","s7","acc","مح"};
    for (int i = 0; i < 9; i++) {
        if (strcmp(s, en[i]) == 0) { *out = i; return 1; }
    }
    return 0;
}

static int parse_imm(const char *s) {
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
        return (int)strtol(s, NULL, 16);
    return atoi(s);
}

static Label* find_lbl(Asm *a, const char *n) {
    for (int i = 0; i < a->label_count; i++)
        if (strcmp(a->labels[i].name, n) == 0)
            return &a->labels[i];
    return NULL;
}

static void add_lbl(Asm *a, const char *n, uint16_t addr) {
    Label *e = find_lbl(a, n);
    if (e) { e->addr = addr; return; }
    if (a->label_count >= 256) return;
    Label *l = &a->labels[a->label_count++];
    snprintf(l->name, sizeof(l->name), "%s", n);
    l->addr = addr;
}

static int find_eq(Asm *a, const char *n) {
    for (int i = 0; i < a->equate_count; i++)
        if (strcmp(a->equates[i].name, n) == 0)
            return a->equates[i].value;
    return -1;
}

static void add_eq(Asm *a, const char *n, int val) {
    if (a->equate_count >= 256) return;
    Equate *e = &a->equates[a->equate_count++];
    snprintf(e->name, sizeof(e->name), "%s", n);
    e->value = val;
}

static Macro* find_macro(Asm *a, const char *n) {
    for (int i = 0; i < a->macro_count; i++)
        if (strcmp(a->macros[i].name, n) == 0)
            return &a->macros[i];
    return NULL;
}

/* حل تعبير بسيط: رقم أو اسم أو label أو label+offset */
static int parse_expr(Asm *a, const char *s) {
    /* تخطي الفراغات */
    while (*s == ' ' || *s == '\t') s++;
    /* إزالة الفراغات الختامية */
    char expr_buf[256];
    snprintf(expr_buf, sizeof(expr_buf), "%s", s);
    int elen = strlen(expr_buf);
    while (elen > 0 && (expr_buf[elen-1] == ' ' || expr_buf[elen-1] == '\t' || expr_buf[elen-1] == '\n')) {
        expr_buf[--elen] = '\0';
    }
    s = expr_buf;
    /* رقم؟ */
    if (s[0] >= '0' && s[0] <= '9') return parse_imm(s);
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) return parse_imm(s);

    /* تحقق من equate */
    int eq = find_eq(a, s);
    if (eq >= 0) return eq;

    /* تحقق من label */
    Label *l = find_lbl(a, s);
    if (l) return l->addr;

    /* label+offset? */
    char buf2[256];
    snprintf(buf2, sizeof(buf2), "%s", s);
    char *plus = strchr(buf2, '+');
    if (!plus) plus = strchr(buf2, '-');
    if (plus) {
        int sign = (*plus == '+') ? 1 : -1;
        *plus = '\0';
        /* تنظيف الفراغات */
        char *lp = buf2;
        while (*lp == ' ' || *lp == '\t') lp++;
        char *rp = lp + strlen(lp) - 1;
        while (rp > lp && (*rp == ' ' || *rp == '\t')) { *rp = '\0'; rp--; }
        const char *off_str = plus + 1;
        while (*off_str == ' ' || *off_str == '\t') off_str++;
        Label *l2 = find_lbl(a, lp);
        if (l2) return l2->addr + sign * parse_imm(off_str);
        int eq2 = find_eq(a, lp);
        if (eq2 >= 0) return eq2 + sign * parse_imm(off_str);
        return sign * parse_imm(off_str);
    }

    return 0;
}

static void emit(Asm *a, uint8_t b) {
    if (a->pass == 2 && a->prog_size < PROG_MAX)
        a->program[a->prog_size] = b;
    a->prog_size++;
}

static void emit16(Asm *a, uint16_t v) {
    emit(a, v & 0xFF);
    emit(a, (v >> 8) & 0xFF);
}

/* ═══════════════════════════════════════
   تحليل سطر واحد
   ═══════════════════════════════════════ */

static int parse_line(Asm *a, const char *line) {
    char buf[256];
    const char *p = line;

    /* تخطي الفراغات */
    p = skip_sp(p);
    if (*p == '\0' || *p == '#') return 0;

    /* تخطي التعليقات */
    if (*p == '#') return 0;

    /* قراءة التسمية (إن وُجدت) */
    char name[64];
    int name_len = 0;
    const char *colon = p;
    while (*colon && *colon != '\n' && *colon != '#') colon++;

    /* البحث عن ':' قبل أول مسافة */
    const char *np = p;
    while (*np && !is_space(*np) && *np != '#') np++;
    if (*(np) == ':') {
        name_len = (int)(np - p);
        if (name_len > 0 && name_len < 64) {
            memcpy(name, p, name_len);
            name[name_len] = '\0';
            p = np + 1;
            p = skip_sp(p);
            if (*p == '\0' || *p == '#') {
                if (a->pass >= 1) add_lbl(a, name, a->prog_size);
                return 0;
            }
        }
    }

    /* قراءة التوكي_n */
    p = read_token(p, buf, sizeof(buf));
    if (buf[0] == '\0') return 0;

    /* ════ Macro definition ════ */
    if (strcmp(buf, ".macro") == 0) {
        char mname[64];
        p = read_token(p, mname, sizeof(mname));
        if (a->pass == 1 && a->macro_count < 64) {
            Macro *m = &a->macros[a->macro_count++];
            snprintf(m->name, sizeof(m->name), "%s", mname);
            m->line_count = 0;
        }
        a->in_macro_def = true;
        snprintf(a->current_macro, sizeof(a->current_macro), "%s", mname);
        return 0;
    }
    if (strcmp(buf, ".endm") == 0) {
        a->in_macro_def = false;
        a->current_macro[0] = '\0';
        return 0;
    }

    /* ════ Inside macro definition: collect lines ════ */
    if (a->in_macro_def) {
        if (a->pass == 1) {
            Macro *m = find_macro(a, a->current_macro);
            if (m && m->line_count < MAX_MACRO_LINES) {
                snprintf(m->lines[m->line_count], 256, "%s", line);
                m->line_count++;
            }
        }
        return 0;
    }

    /* ════ Macro expansion (only if not in definition) ════ */
    if (!a->in_macro_def) {
        Macro *m = find_macro(a, buf);
        if (m) {
            for (int i = 0; i < m->line_count; i++) {
                parse_line(a, m->lines[i]);
            }
            return 0;
        }
    }

    /* ════ Conditional assembly: .if / .else / .endif ════ */
    if (strcmp(buf, ".if") == 0) {
        char expr_buf[128];
        int ei = 0;
        while (*p && *p != '\n' && *p != '#' && ei < 127) {
            if ((unsigned char)*p == 0xD8 && (unsigned char)*(p+1) == 0x8C) break;
            expr_buf[ei++] = *p++;
        }
        expr_buf[ei] = '\0';
        int val = parse_expr(a, expr_buf);
        bool parent_skip = (a->if_nesting > 0) ? a->if_skip_stack[a->if_nesting - 1] : false;
        if (a->if_nesting < MAX_IF_NESTING)
            a->if_skip_stack[a->if_nesting] = parent_skip || (val == 0);
        a->if_nesting++;
        return 0;
    }
    if (strcmp(buf, ".else") == 0) {
        if (a->if_nesting > 0) {
            int top = a->if_nesting - 1;
            bool parent_skip = (top > 0) ? a->if_skip_stack[top - 1] : false;
            a->if_skip_stack[top] = parent_skip || !a->if_skip_stack[top];
        }
        return 0;
    }
    if (strcmp(buf, ".endif") == 0) {
        if (a->if_nesting > 0) a->if_nesting--;
        return 0;
    }
    if (a->if_nesting > 0 && a->if_skip_stack[a->if_nesting - 1]) return 0;

    /* ── تسمية في بداية السطر ── */
    /* هل buf تسمية وليس تعليمة؟ */
    if (name_len == 0) {
        /*可能是 تسمية في بداية السطر */
        const char *check = line;
        check = skip_sp(check);
        char tmp[64];
        const char *end = check;
        while (*end && !is_space(*end) && *end != ':' && *end != '#') end++;
        int tlen = (int)(end - check);
        if (tlen > 0 && tlen < 64 && *end == ':') {
            memcpy(tmp, check, tlen);
            tmp[tlen] = '\0';
            add_lbl(a, tmp, a->prog_size);
            p = end + 1;
            p = skip_sp(p);
            if (*p == '\0' || *p == '#') return 0;
            p = read_token(p, buf, sizeof(buf));
            if (buf[0] == '\0') return 0;
        }
    }

    /* ── التعليمة ── */

    /* توقف */
    if (strcmp(buf, "توقف") == 0 || strcmp(buf, "halt") == 0) {
        emit(a, 0xF0);
        emit(a, 0xD3);
        return 0;
    }

    /* سكون */
    if (strcmp(buf, "سكون") == 0 || strcmp(buf, "nop") == 0) {
        emit(a, 0x00);
        return 0;
    }

    /* حمّل */
    if (strcmp(buf, "حمّل") == 0 || strcmp(buf, "load") == 0) {
        int r = 8;
        char tok2[64];
        p = read_token(p, tok2, sizeof(tok2));
        if (is_reg(tok2, &r)) {
            /* قراءة باقي السطر كتعبير */
            char expr_buf[128] = {0};
            p = skip_sp(p);
            int ei = 0;
            while (*p && *p != '\n' && *p != '#' && ei < 127) {
                if ((unsigned char)*p == 0xD8 && (unsigned char)*(p+1) == 0x8C) break;
                expr_buf[ei++] = *p++;
            }
            expr_buf[ei] = '\0';
            /* إزالة الفراغات من النهاية */
            while (ei > 0 && (expr_buf[ei-1] == ' ' || expr_buf[ei-1] == '\t')) expr_buf[--ei] = '\0';
            int val = parse_expr(a, expr_buf);
            emit(a, 0x10 | (r & 0x0F));
            emit(a, (uint8_t)(val & 0xFF));
        }
        return 0;
    }

    /* نقل — تنسيق: الوجه في بايت2، المصدر في حقل reg */
    if (strcmp(buf, "نقل") == 0 || strcmp(buf, "mov") == 0) {
        int dst = 0, src = 0;
        char tok2[64], tok3[64];
        p = read_token(p, tok2, sizeof(tok2));
        p = read_token(p, tok3, sizeof(tok3));
        if (is_reg(tok2, &dst) && is_reg(tok3, &src)) {
            emit(a, 0x20 | (src & 0x0F));
            emit(a, (uint8_t)(dst & 0x0F));
        }
        return 0;
    }

    /* اقرأ */
    if (strcmp(buf, "اقرأ") == 0 || strcmp(buf, "ld") == 0) {
        int r = 0;
        char tok2[64], tok3[64];
        p = read_token(p, tok2, sizeof(tok2));
        p = read_token(p, tok3, sizeof(tok3));
        if (is_reg(tok2, &r)) {
            int addr = parse_imm(tok3);
            emit(a, 0x30 | (r & 0x0F));
            emit(a, (uint8_t)(addr & 0xFF));
        }
        return 0;
    }

    /* خزن */
    if (strcmp(buf, "خزن") == 0 || strcmp(buf, "st") == 0) {
        int r = 0;
        char tok2[64], tok3[64];
        p = read_token(p, tok2, sizeof(tok2));
        p = read_token(p, tok3, sizeof(tok3));
        if (is_reg(tok2, &r)) {
            int addr = parse_imm(tok3);
            emit(a, 0x40 | (r & 0x0F));
            emit(a, (uint8_t)(addr & 0xFF));
        }
        return 0;
    }

    /* ادفع */
    if (strcmp(buf, "ادفع") == 0 || strcmp(buf, "push") == 0) {
        emit(a, 0x50);
        return 0;
    }

    /* اسحب */
    if (strcmp(buf, "اسحب") == 0 || strcmp(buf, "pop") == 0) {
        emit(a, 0x60);
        return 0;
    }

    /* نداء */
    if (strcmp(buf, "نداء") == 0 || strcmp(buf, "call") == 0) {
        char tok2[64];
        p = read_token(p, tok2, sizeof(tok2));
        int addr = parse_imm(tok2);
        Label *lbl = find_lbl(a, tok2);
        if (lbl) addr = lbl->addr;
        emit(a, 0x70);
        emit16(a, (uint16_t)addr);
        return 0;
    }

    /* إرجاع */
    if (strcmp(buf, "إرجاع") == 0 || strcmp(buf, "ارجع") == 0 || strcmp(buf, "ret") == 0) {
        emit(a, 0x80);
        return 0;
    }

    /* تبريد */
    if (strcmp(buf, "تبريد") == 0 || strcmp(buf, "neg") == 0) {
        emit(a, 0x8F);
        return 0;
    }

    /* جمع */
    if (strcmp(buf, "جمع") == 0 || strcmp(buf, "add") == 0) {
        int r = 0;
        char tok2[64];
        p = read_token(p, tok2, sizeof(tok2));
        if (is_reg(tok2, &r)) {
            emit(a, 0x90 | (r & 0x0F));
        }
        return 0;
    }

    /* طرح */
    if (strcmp(buf, "طرح") == 0 || strcmp(buf, "sub") == 0) {
        int r = 0;
        char tok2[64];
        p = read_token(p, tok2, sizeof(tok2));
        if (is_reg(tok2, &r)) {
            emit(a, 0xA0 | (r & 0x0F));
        }
        return 0;
    }

    /* ضرب */
    if (strcmp(buf, "ضرب") == 0 || strcmp(buf, "mul") == 0) {
        int r = 0;
        char tok2[64];
        p = read_token(p, tok2, sizeof(tok2));
        if (is_reg(tok2, &r)) {
            emit(a, 0xB0 | (r & 0x0F));
        }
        return 0;
    }

    /* قسمة */
    if (strcmp(buf, "قسمة") == 0 || strcmp(buf, "div") == 0) {
        int r = 0;
        char tok2[64];
        p = read_token(p, tok2, sizeof(tok2));
        if (is_reg(tok2, &r)) {
            emit(a, 0xC0 | (r & 0x0F));
        }
        return 0;
    }

    /* باقي */
    if (strcmp(buf, "باقي") == 0 || strcmp(buf, "mod") == 0) {
        int r = 0;
        char tok2[64];
        p = read_token(p, tok2, sizeof(tok2));
        if (is_reg(tok2, &r)) {
            emit(a, 0xD0 | (r & 0x0F));
        }
        return 0;
    }

    /* اطبع */
    if (strcmp(buf, "اطبع") == 0 || strcmp(buf, "print") == 0) {
        emit(a, 0xE0);
        return 0;
    }

    /* اطبع حرف */
    if (strcmp(buf, "اطبع_حرف") == 0 || strcmp(buf, "print_ch") == 0) {
        emit(a, 0xF0);
        emit(a, 0x50);
        return 0;
    }

    /* ادخل */
    if (strcmp(buf, "ادخل") == 0 || strcmp(buf, "input") == 0) {
        emit(a, 0xF0);
        emit(a, 0x60);
        return 0;
    }

    /* قفز */
    if (strcmp(buf, "قفز") == 0 || strcmp(buf, "jmp") == 0) {
        char tok2[64];
        p = read_token(p, tok2, sizeof(tok2));
        int addr = parse_imm(tok2);
        Label *lbl = find_lbl(a, tok2);
        if (lbl) addr = lbl->addr;
        emit(a, 0xF0);
        emit(a, 0x00);
        emit16(a, (uint16_t)addr);
        return 0;
    }

    /* قفز إذا صفر */
    if (strcmp(buf, "قفز_إذا_صفر") == 0 || strcmp(buf, "jz") == 0) {
        char tok2[64];
        p = read_token(p, tok2, sizeof(tok2));
        int addr = parse_imm(tok2);
        Label *lbl = find_lbl(a, tok2);
        if (lbl) addr = lbl->addr;
        emit(a, 0xF0);
        emit(a, 0x10);
        emit16(a, (uint16_t)addr);
        return 0;
    }

    /* قفز إذا غير صفر */
    if (strcmp(buf, "قفز_إذا_غيرصفر") == 0 || strcmp(buf, "jnz") == 0) {
        char tok2[64];
        p = read_token(p, tok2, sizeof(tok2));
        int addr = parse_imm(tok2);
        Label *lbl = find_lbl(a, tok2);
        if (lbl) addr = lbl->addr;
        emit(a, 0xF0);
        emit(a, 0x20);
        emit16(a, (uint16_t)addr);
        return 0;
    }

    /* قفز إذا حمل */
    if (strcmp(buf, "قفز_إذا_حمل") == 0 || strcmp(buf, "jc") == 0) {
        char tok2[64];
        p = read_token(p, tok2, sizeof(tok2));
        int addr = parse_imm(tok2);
        Label *lbl = find_lbl(a, tok2);
        if (lbl) addr = lbl->addr;
        emit(a, 0xF0);
        emit(a, 0x30);
        emit16(a, (uint16_t)addr);
        return 0;
    }

    /* قفز إذا سالب */
    if (strcmp(buf, "قفز_إذا_سالب") == 0 || strcmp(buf, "jn") == 0) {
        char tok2[64];
        p = read_token(p, tok2, sizeof(tok2));
        int addr = parse_imm(tok2);
        Label *lbl = find_lbl(a, tok2);
        if (lbl) addr = lbl->addr;
        emit(a, 0xF0);
        emit(a, 0x40);
        emit16(a, (uint16_t)addr);
        return 0;
    }

    /* عمليات منطقية */
    if (strcmp(buf, "قارن") == 0 || strcmp(buf, "cmp") == 0) {
        int r = 0;
        char tok2[64];
        p = read_token(p, tok2, sizeof(tok2));
        if (is_reg(tok2, &r)) { emit(a, 0xF0); emit(a, 0xE0 | (r & 0x0F)); }
        return 0;
    }
    if (strcmp(buf, "فعّل_مقاطعات") == 0 || strcmp(buf, "ei") == 0) {
        emit(a, 0xF0); emit(a, 0xE1);
        return 0;
    }
    if (strcmp(buf, "عطّل_مقاطعات") == 0 || strcmp(buf, "di") == 0) {
        emit(a, 0xF0); emit(a, 0xE2);
        return 0;
    }
    if (strcmp(buf, "إرجاع_مقاطعة") == 0 || strcmp(buf, "reti") == 0) {
        emit(a, 0xF0); emit(a, 0xE3);
        return 0;
    }
    if (strcmp(buf, "أوحصري") == 0 || strcmp(buf, "xor") == 0) {
        int r = 0;
        char tok2[64];
        p = read_token(p, tok2, sizeof(tok2));
        if (is_reg(tok2, &r)) { emit(a, 0xF0); emit(a, 0x70 | (r & 0x0F)); }
        return 0;
    }
    if (strcmp(buf, "أومنطقي") == 0 || strcmp(buf, "or") == 0) {
        int r = 0;
        char tok2[64];
        p = read_token(p, tok2, sizeof(tok2));
        if (is_reg(tok2, &r)) { emit(a, 0xF0); emit(a, 0x80 | (r & 0x0F)); }
        return 0;
    }
    if (strcmp(buf, "ومنطقي") == 0 || strcmp(buf, "and") == 0) {
        int r = 0;
        char tok2[64];
        p = read_token(p, tok2, sizeof(tok2));
        if (is_reg(tok2, &r)) { emit(a, 0xF0); emit(a, 0x90 | (r & 0x0F)); }
        return 0;
    }
    if (strcmp(buf, "ليس") == 0 || strcmp(buf, "not") == 0) {
        emit(a, 0xF0); emit(a, 0xA0);
        return 0;
    }
    if (strcmp(buf, "تحريك_يسار") == 0 || strcmp(buf, "shl") == 0) {
        emit(a, 0xF0); emit(a, 0xB0);
        return 0;
    }
    if (strcmp(buf, "تحريك_يمين") == 0 || strcmp(buf, "shr") == 0) {
        emit(a, 0xF0); emit(a, 0xC0);
        return 0;
    }
    if (strcmp(buf, "بدل") == 0 || strcmp(buf, "swap") == 0) {
        int dst = 0, src = 0;
        char tok2[64], tok3[64];
        p = read_token(p, tok2, sizeof(tok2));
        p = read_token(p, tok3, sizeof(tok3));
        if (is_reg(tok2, &dst) && is_reg(tok3, &src)) {
            emit(a, 0xF0);
            emit(a, 0xD0);
            emit(a, (uint8_t)(((dst & 0x0F) << 4) | (src & 0x0F)));
        }
        return 0;
    }
    /* اقرأ غير مباشر: LDRI dst, hi, lo → F0 F1 ((dst<<4)|hi) lo (4 بايت حسب المواصفة) */
    if (strcmp(buf, "اقرأ_غيرمباشر") == 0 || strcmp(buf, "ldri") == 0) {
        int dst = 0, hi = 0, lo = 0;
        char tok2[64], tok3[64], tok4[64];
        p = read_token(p, tok2, sizeof(tok2));
        p = read_token(p, tok3, sizeof(tok3));
        p = read_token(p, tok4, sizeof(tok4));
        if (is_reg(tok2, &dst) && is_reg(tok3, &hi) && is_reg(tok4, &lo)) {
            emit(a, 0xF0);
            emit(a, 0xF1);
            emit(a, (uint8_t)(((dst & 0x0F) << 4) | (hi & 0x0F)));
            emit(a, (uint8_t)(lo & 0x0F));
        }
        return 0;
    }
    /* خزن غير مباشر: STRI src, hi, lo → F0 F2 ((src<<4)|hi) lo (4 بايت حسب المواصفة) */
    if (strcmp(buf, "خزن_غيرمباشر") == 0 || strcmp(buf, "stri") == 0) {
        int src = 0, hi = 0, lo = 0;
        char tok2[64], tok3[64], tok4[64];
        p = read_token(p, tok2, sizeof(tok2));
        p = read_token(p, tok3, sizeof(tok3));
        p = read_token(p, tok4, sizeof(tok4));
        if (is_reg(tok2, &src) && is_reg(tok3, &hi) && is_reg(tok4, &lo)) {
            emit(a, 0xF0);
            emit(a, 0xF2);
            emit(a, (uint8_t)(((src & 0x0F) << 4) | (hi & 0x0F)));
            emit(a, (uint8_t)(lo & 0x0F));
        }
        return 0;
    }
    if (strcmp(buf, "نقص") == 0 || strcmp(buf, "dec") == 0) {
        int r = 0;
        char tok2[64];
        p = read_token(p, tok2, sizeof(tok2));
        if (is_reg(tok2, &r)) { emit(a, 0xF0); emit(a, 0xD1); emit(a, (uint8_t)(r & 0x0F)); }
        return 0;
    }
    if (strcmp(buf, "وزد") == 0 || strcmp(buf, "inc") == 0) {
        int r = 0;
        char tok2[64];
        p = read_token(p, tok2, sizeof(tok2));
        if (is_reg(tok2, &r)) { emit(a, 0xF0); emit(a, 0xD2); emit(a, (uint8_t)(r & 0x0F)); }
        return 0;
    }

    /* ══════ تسمية معرّف (equates) ════ */
    /* تنسيق: NAME = value */
    {
        const char *eq_check = line;
        eq_check = skip_sp(eq_check);
        char eq_name[64];
        const char *eq_end = eq_check;
        while (*eq_end && !is_space(*eq_end) && *eq_end != '=' && *eq_end != ':' && *eq_end != '#') eq_end++;
        /* تخطي الفراغات قبل = */
        const char *eq_sign = eq_end;
        while (is_space(*eq_sign)) eq_sign++;
        int eq_len = (int)(eq_end - eq_check);
        if (eq_len > 0 && eq_len < 64 && *eq_sign == '=') {
            memcpy(eq_name, eq_check, eq_len);
            eq_name[eq_len] = '\0';
            /* تخطي = */
            const char *eq_val = eq_sign + 1;
            eq_val = skip_sp(eq_val);
            char val_buf[64];
            const char *vp = eq_val;
            int vi = 0;
            while (*vp && *vp != '\n' && *vp != '#' && vi < 63) {
                if (is_space(*vp) || *vp == ',') break;
                val_buf[vi++] = *vp++;
            }
            val_buf[vi] = '\0';
            if (vi > 0) {
                int v = parse_expr(a, val_buf);
                add_eq(a, eq_name, v);
            }
            return 0;
        }
    }

    /* ════ 데이터 تعليمات ════ */

    /* .db — byte واحد أو أكثر: .db 65, 66, 67 */
    if (strcmp(buf, ".db") == 0 || strcmp(buf, "بيانات") == 0) {
        char tok[64];
        while ((p = read_token(p, tok, sizeof(tok))), tok[0] != '\0') {
            /* سلسلة نصية؟ */
            if (tok[0] == '"') {
                const char *s = tok + 1;
                while (*s && *s != '"') {
                    if (*s == '\\' && *(s+1)) { s++; }
                    emit(a, (uint8_t)*s);
                    s++;
                }
            } else {
                emit(a, (uint8_t)(parse_expr(a, tok) & 0xFF));
            }
        }
        return 0;
    }

    /* .dw — كلمة 16-bit: .dw 0x1234 */
    if (strcmp(buf, ".dw") == 0 || strcmp(buf, "كلمة") == 0) {
        char tok[64];
        while ((p = read_token(p, tok, sizeof(tok))), tok[0] != '\0') {
            emit16(a, (uint16_t)(parse_expr(a, tok) & 0xFFFF));
        }
        return 0;
    }

    /* .ds — سلسلة نصية: .ds "مرحبا" */
    if (strcmp(buf, ".ds") == 0 || strcmp(buf, "نص") == 0) {
        char tok[64];
        p = read_token(p, tok, sizeof(tok));
        if (tok[0] == '"') {
            const char *s = tok + 1;
            while (*s && *s != '"') {
                if (*s == '\\' && *(s+1)) { s++; }
                emit(a, (uint8_t)*s);
                s++;
            }
            emit(a, 0); /* null terminator */
        }
        return 0;
    }

    /* .org — تغيير العنوان: .org 0x100 */
    if (strcmp(buf, ".org") == 0) {
        char tok[64];
        p = read_token(p, tok, sizeof(tok));
        int new_addr = parse_expr(a, tok);
        while (a->prog_size < new_addr) emit(a, 0);
        return 0;
    }

    /* .align — محاذاة: .align 4 */
    if (strcmp(buf, ".align") == 0) {
        char tok[64];
        p = read_token(p, tok, sizeof(tok));
        int align = parse_expr(a, tok);
        if (align > 0) {
            int rem = a->prog_size % align;
            if (rem != 0) {
                int pad = align - rem;
                while (pad-- > 0) emit(a, 0);
            }
        }
        return 0;
    }

    /* .include — تضمين ملف: .include "filename.ضasm" */
    if (strcmp(buf, ".include") == 0 || strcmp(buf, "تضمين") == 0) {
        char tok[256];
        p = read_token(p, tok, sizeof(tok));
        /* إزالة علامات التنصيص */
        if (tok[0] == '"') {
            memmove(tok, tok + 1, strlen(tok));
            int len = strlen(tok);
            if (len > 0 && tok[len-1] == '"') tok[len-1] = '\0';
        }
        {
            FILE *inc = fopen(tok, "r");
            if (inc) {
                char iline[512];
                int saved_line = a->line;
                while (fgets(iline, sizeof(iline), inc)) {
                    a->line++;
                    parse_line(a, iline);
                }
                a->line = saved_line;
                fclose(inc);
            } else {
                fprintf(stderr, "خطأ %d: لا يمكن فتح الملف '%s'\n", a->line, tok);
            }
        }
        return 0;
    }

    a->error_count++;
    fprintf(stderr, "خطأ %d: تعليمة غير معروفة '%s'\n", a->line, buf);
    return -1;
}

/* ═══════════════════════════════════════
   تجميع ملف
   ═══════════════════════════════════════ */

static int assemble(Asm *a, const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) { perror(filename); return -1; }

    char line[512];

    for (a->pass = 1; a->pass <= 2; a->pass++) {
        a->prog_size = 0;
        if (a->pass == 1) { a->label_count = 0; a->equate_count = 0; }
        a->line = 0;
        rewind(f);

        while (fgets(line, sizeof(line), f)) {
            a->line++;
            /* تخطي BOM */
            unsigned char *ul = (unsigned char *)line;
            if (ul[0] == 0xEF && ul[1] == 0xBB && ul[2] == 0xBF) {
                parse_line(a, (const char *)(ul + 3));
            } else {
                parse_line(a, line);
            }
        }
    }

    fclose(f);
    printf("  تجميع: %s\n", filename);
    printf("  تم التجميع: %d بايت\n", a->prog_size);
    if (a->error_count > 0) {
        fprintf(stderr, "  ⚠ %d خطأ\n", a->error_count);
    }
    return 0;
}

/* ═══════════════════════════════════════
   تحميل/حفظ
   ═══════════════════════════════════════ */

static int load_bin(CPU *c, const char *fn) {
    FILE *f = fopen(fn, "rb");
    if (!f) return -1;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    rewind(f);
    if (sz <= 0 || sz > PROG_MAX) { fclose(f); return -1; }
    size_t n = fread(&c->memory[PROG_START], 1, sz, f);
    (void)n;
    fclose(f);
    c->pc = PROG_START;
    return (int)sz;
}

/* ═══════════════════════════════════════
   الرئيسية
   ═══════════════════════════════════════ */

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("╔══════════════════════════════════════════╗\n");
        printf("║  معالج ض — المحاكي والمجمّع (8-bit)     ║\n");
        printf("╠══════════════════════════════════════════╣\n");
        printf("║  الاستخدام: ./dhad_cpu <ملف> [خيارات]   ║\n");
        printf("║  --debug  --hex  --state  --bin          ║\n");
        printf("╚══════════════════════════════════════════╝\n");
        return 1;
    }

    int do_debug = 0, do_hex = 0, do_state = 0, do_savebin = 0, do_debugger = 0;
    const char *filename = NULL;
    const char *outfile = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--debug") == 0) do_debug = 1;
        else if (strcmp(argv[i], "--debugger") == 0 || strcmp(argv[i], "-d") == 0) do_debugger = 1;
        else if (strcmp(argv[i], "--hex") == 0) do_hex = 1;
        else if (strcmp(argv[i], "--state") == 0) do_state = 1;
        else if (strcmp(argv[i], "--bin") == 0) do_savebin = 1;
        else if (strcmp(argv[i], "--out") == 0 && i+1 < argc) outfile = argv[++i];
        else filename = argv[i];
    }
    if (!filename) return 1;

    CPU cpu;
    cpu_init(&cpu);

    uint8_t prog[PROG_MAX];
    int prog_size = 0;
    size_t fn_len = strlen(filename);
    int is_asm = (fn_len > 6 && strcmp(filename + fn_len - 6, ".ضasm") == 0)
              || (fn_len > 4 && strcmp(filename + fn_len - 4, ".asm") == 0);

    if (is_asm) {
        Asm a;
        memset(&a, 0, sizeof(a));
        if (assemble(&a, filename) < 0) return 1;
        prog_size = a.prog_size;
        memcpy(prog, a.program, prog_size);

        if (do_hex) {
            printf("\n  الكود المجمّع:\n");
            for (int i = 0; i < prog_size; i++)
                printf("  0x%04X: %02X\n", i + PROG_START, prog[i]);
        }

        if (do_savebin) {
            const char *ofn = outfile ? outfile : "output.bin";
            FILE *of = fopen(ofn, "wb");
            if (of) { fwrite(prog, 1, prog_size, of); fclose(of); }
            printf("  تم الحفظ: %s (%d بايت)\n", ofn, prog_size);
        }
    } else {
        int sz = load_bin(&cpu, filename);
        if (sz < 0) { fprintf(stderr, "  فشل التحميل\n"); return 1; }
        printf("  تم التحميل: %d بايت\n", sz);
    }

    if (do_debug) {
        /* نسخ البرنامج إلى الذاكرة */
        if (is_asm) {
            memcpy(&cpu.memory[PROG_START], prog, prog_size);
        }
        printf("\n  ═══ بدء التنفيذ ═══\n");
        int max_inst = 10000;
        while (!cpu.halted && cpu.cycles < (uint32_t)max_inst) {
            uint16_t old_pc = cpu.pc;
            cpu_step(&cpu);
            printf("  0x%04X: %02X  | المح:%3d | الأعلام:%c%c%c | SP:%d\n",
                old_pc, cpu.memory[old_pc],
                cpu.acc,
                (cpu.flags & FLAG_Z) ? 'Z' : '-',
                (cpu.flags & FLAG_N) ? 'N' : '-',
                (cpu.flags & FLAG_C) ? 'C' : '-',
                cpu.sp);
        }
        if (cpu.cycles >= (uint32_t)max_inst)
            printf("  ═══ تم تجاوز الحد الأقصى للدورات ═══\n");
    } else if (do_debugger) {
        /* ═══ وضع المصحح التفاعلي ═══ */
        if (is_asm) {
            memcpy(&cpu.memory[PROG_START], prog, prog_size);
        }

        DhadDebugger dbg;
        dbg_init(&dbg);

        /* تحميل التسميات من المجمّع */
        if (is_asm) {
            Asm a;
            memset(&a, 0, sizeof(a));
            if (assemble(&a, filename) >= 0) {
                const char *names[256];
                uint16_t addrs[256];
                int cnt = a.label_count < 256 ? a.label_count : 256;
                for (int i = 0; i < cnt; i++) {
                    names[i] = a.labels[i].name;
                    addrs[i] = a.labels[i].addr;
                }
                dbg_load_labels(&dbg, names, addrs, cnt);
            }
        }

        printf("\n  ═══ المصحح التفاعلي (Debugger) ═══\n");
        printf("  اكتب h للمساعدة\n\n");

        bool first_stop = true;

        while (!cpu.halted) {
            if (dbg.single_step || first_stop) {
                first_stop = false;
                dbg.running = false;

                /* طباعة الحالة */
                printf("  ─────────────────────────────────────\n");
                dbg_print_instruction(&dbg, cpu.memory, cpu.pc);
                printf("  المح: %3d (0x%02X) | الأعلام: %c%c%c | SP: %d | الدورة: %u\n",
                       cpu.acc, cpu.acc,
                       (cpu.flags & FLAG_Z) ? 'Z' : '-',
                       (cpu.flags & FLAG_N) ? 'N' : '-',
                       (cpu.flags & FLAG_C) ? 'C' : '-',
                       cpu.sp, cpu.cycles);

                /* حلقة الأوامر */
                while (!dbg.running && !cpu.halted) {
                    printf("  (ض$dbg) > ");
                    fflush(stdout);
                    char line[256];
                    if (!fgets(line, sizeof(line), stdin)) {
                        dbg.running = true;
                        break;
                    }
                    /* إزالة سطر جديد */
                    line[strcspn(line, "\n")] = '\0';

                    if (strcmp(line, "n") == 0 || strcmp(line, "next") == 0 ||
                        strcmp(line, "s") == 0 || strcmp(line, "step") == 0) {
                        dbg.single_step = true;
                        dbg.running = true;
                    } else if (strcmp(line, "c") == 0 || strcmp(line, "continue") == 0) {
                        dbg.single_step = false;
                        dbg.running = true;
                    } else if (strcmp(line, "r") == 0 || strcmp(line, "regs") == 0) {
                        dbg_print_registers(&dbg, cpu.regs, cpu.acc, cpu.flags);
                    } else if (strncmp(line, "m ", 2) == 0) {
                        uint16_t adr = (uint16_t)strtol(line + 2, NULL, 0);
                        dbg_print_memory(&dbg, cpu.memory, adr, 64);
                    } else if (strcmp(line, "st") == 0 || strcmp(line, "stack") == 0) {
                        dbg_print_stack(&dbg, cpu.stack, cpu.sp);
                    } else if (strncmp(line, "bp ", 3) == 0) {
                        char *arg = line + 3;
                        int slot = dbg_bp_add_label(&dbg, arg);
                        if (slot < 0) {
                            uint16_t addr = (uint16_t)strtol(arg, NULL, 0);
                            slot = dbg_bp_add(&dbg, addr);
                        }
                        if (slot >= 0) printf("  + نقطة توقف #%d\n", slot);
                        else printf("  - خطأ\n");
                    } else if (strncmp(line, "bp-", 3) == 0) {
                        int slot = atoi(line + 3);
                        dbg_bp_remove(&dbg, slot);
                        printf("  - تم حذف #%d\n", slot);
                    } else if (strcmp(line, "bpl") == 0) {
                        dbg_bp_list(&dbg);
                    } else if (strncmp(line, "wp ", 3) == 0) {
                        uint16_t adr = (uint16_t)strtol(line + 3, NULL, 0);
                        int slot = dbg_wp_add_memory(&dbg, adr, WP_CHANGE);
                        if (slot >= 0) printf("  + مراقبة #%d على 0x%04X\n", slot, adr);
                    } else if (strcmp(line, "wpl") == 0) {
                        dbg_wp_list(&dbg);
                    } else if (strncmp(line, "t", 1) == 0) {
                        int cnt = 20;
                        if (line[1] == ' ') cnt = atoi(line + 2);
                        dbg_trace_dump(&dbg, cnt);
                    } else if (strncmp(line, "bp_all", 6) == 0) {
                        /* نقاط توقف في كل مكان */
                        for (uint16_t a = 0; a < prog_size; a++) {
                            uint8_t raw = cpu.memory[PROG_START + a];
                            if (raw == 0xF0) {
                                uint8_t ext = cpu.memory[PROG_START + a + 1];
                                if (ext == 0xD3) { /* HALT */
                                    dbg_bp_add(&dbg, PROG_START + a);
                                }
                            }
                        }
                        printf("  + تمت إضافة نقاط التوقف على HALT\n");
                    } else if (strcmp(line, "h") == 0 || strcmp(line, "help") == 0) {
                        dbg_print_help(&dbg);
                    } else if (strcmp(line, "q") == 0 || strcmp(line, "quit") == 0) {
                        cpu.halted = true;
                    } else if (line[0] != '\0') {
                        printf("  أمر غير معروف: %s\n", line);
                    }
                }
            }

            if (cpu.halted) break;

            /* تنفيذ تعليمة واحدة */
            uint16_t old_pc = cpu.pc;
            uint8_t old_acc = cpu.acc;
            uint8_t old_flags = cpu.flags;
            uint8_t old_sp = cpu.sp;

            /* تسجيل في التتبع */
            dbg_trace_record(&dbg, old_pc, cpu.memory[old_pc],
                           cpu.memory[old_pc + 1], old_acc, old_flags,
                           old_sp, cpu.cycles);

            cpu_step(&cpu);

            dbg.total_instructions++;
            dbg.total_cycles = cpu.cycles;

            /* فحص نقاط المراقبة */
            dbg_wp_check(&dbg, cpu.pc, 0, cpu.acc, false);

            /* فحص نقاط التوقف */
            bool hit = false;
            if (dbg_bp_check(&dbg, cpu.pc, &hit)) {
                printf("  ● نقطة توقف عند 0x%04X\n", cpu.pc);
                dbg.single_step = true;
                dbg.running = false;
                first_stop = false;
                continue;
            }

            /* فحص الحد الأقصى */
            if (cpu.cycles >= dbg.max_cycles) {
                printf("  ═══ تم تجاوز الحد الأقصى (%u دورة) ═══\n", dbg.max_cycles);
                break;
            }
        }

        printf("\n  ═══ انتهى التنفيذ ═══\n");
        printf("  الدورات: %u | التعليمات: %u | اصابات BP: %u | مراقبة WP: %u\n",
               dbg.total_cycles, dbg.total_instructions, dbg.bp_hits, dbg.wp_triggers);
    } else {
        /* تنفيذ عادي */
        if (is_asm) {
            memcpy(&cpu.memory[PROG_START], prog, prog_size);
        }
        while (!cpu.halted && cpu.cycles < 100000)
            cpu_step(&cpu);
    }

    if (do_state) {
        printf("\n═══════════════════════════════════════\n");
        printf("  حالة معالج ض (8-bit)\n");
        printf("═══════════════════════════════════════\n");
        printf("  عداد البرنامج (PC): 0x%04X\n", cpu.pc);
        printf("  مؤشر المكدس (SP):   %d\n", cpu.sp);
        printf("  عدد الدورات:        %d\n", cpu.cycles);
        printf("───────────────────────────────────────\n");
        printf("  السجلات:\n");
        for (int i = 0; i < REG_COUNT; i++)
            printf("    س%d = %3d (0x%02X)\n", i, cpu.regs[i], cpu.regs[i]);
        printf("    المح = %3d (0x%02X)\n", cpu.acc, cpu.acc);
        printf("───────────────────────────────────────\n");
        printf("  الأعلام: Z=%d N=%d C=%d\n",
            (cpu.flags & FLAG_Z) ? 1 : 0,
            (cpu.flags & FLAG_N) ? 1 : 0,
            (cpu.flags & FLAG_C) ? 1 : 0);
        printf("  الحالة:  %s\n", cpu.halted ? "متوقف" : "يعمل");
        printf("═══════════════════════════════════════\n");
    }

    printf("\n  النتيجة (مح): %d\n", cpu.acc);
    printf("  الدورات:      %d\n", cpu.cycles);
    printf("  الحالة:       %s\n", cpu.halted ? "متوقف" : "يعمل");

    return 0;
}
