#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include "dhad_asm.h"

static int asm_is_space(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

static const char* asm_skip_sp(const char *p) {
    while (*p) {
        if (asm_is_space(*p) || *p == ',') { p++; continue; }
        if ((unsigned char)*p == 0xD8 && (unsigned char)*(p+1) == 0x8C) { p += 2; continue; }
        break;
    }
    return p;
}

static const char* asm_read_token(const char *p, char *buf, int sz) {
    p = asm_skip_sp(p);
    int i = 0;
    while (*p && i < sz - 1) {
        if ((unsigned char)*p == 0xD8 && (unsigned char)*(p+1) == 0x8C) break;
        if (asm_is_space(*p) || *p == ',' || *p == ':') break;
        buf[i++] = *p++;
    }
    buf[i] = '\0';
    if (*p == ':') p++;
    return p;
}

static int asm_is_reg(const char *s, int *out) {
    static const char *names[] = {"س0","س1","س2","س3","س4","س5","س6","س7","مح"};
    for (int i = 0; i < 9; i++) {
        if (strcmp(s, names[i]) == 0) { *out = i; return 1; }
    }
    static const char *en[] = {"s0","s1","s2","s3","s4","s5","s6","s7","acc","مح"};
    for (int i = 0; i < 9; i++) {
        if (strcmp(s, en[i]) == 0) { *out = i; return 1; }
    }
    return 0;
}

static int asm_parse_imm(const char *s) {
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
        return (int)strtol(s, NULL, 16);
    return atoi(s);
}

static AsmLabel* asm_find_lbl(DhadAsm *a, const char *n) {
    for (int i = 0; i < a->label_count; i++)
        if (strcmp(a->labels[i].name, n) == 0)
            return &a->labels[i];
    return NULL;
}

static void asm_add_lbl(DhadAsm *a, const char *n, uint16_t addr) {
    AsmLabel *e = asm_find_lbl(a, n);
    if (e) { e->addr = addr; return; }
    if (a->label_count >= ASM_MAX_LABELS) return;
    AsmLabel *l = &a->labels[a->label_count++];
    snprintf(l->name, sizeof(l->name), "%s", n);
    l->addr = addr;
}

static int asm_find_eq(DhadAsm *a, const char *n) {
    for (int i = 0; i < a->equate_count; i++)
        if (strcmp(a->equates[i].name, n) == 0)
            return a->equates[i].value;
    return -1;
}

static void asm_add_eq(DhadAsm *a, const char *n, int val) {
    if (a->equate_count >= ASM_MAX_EQUATES) return;
    AsmEquate *e = &a->equates[a->equate_count++];
    snprintf(e->name, sizeof(e->name), "%s", n);
    e->value = val;
}

static AsmMacro* asm_find_macro(DhadAsm *a, const char *n) {
    for (int i = 0; i < a->macro_count; i++)
        if (strcmp(a->macros[i].name, n) == 0)
            return &a->macros[i];
    return NULL;
}

static int asm_parse_expr(DhadAsm *a, const char *s) {
    while (*s == ' ' || *s == '\t') s++;
    char expr_buf[256];
    snprintf(expr_buf, sizeof(expr_buf), "%s", s);
    int elen = strlen(expr_buf);
    while (elen > 0 && (expr_buf[elen-1] == ' ' || expr_buf[elen-1] == '\t' || expr_buf[elen-1] == '\n')) {
        expr_buf[--elen] = '\0';
    }
    s = expr_buf;
    if (s[0] >= '0' && s[0] <= '9') return asm_parse_imm(s);
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) return asm_parse_imm(s);

    int eq = asm_find_eq(a, s);
    if (eq >= 0) return eq;

    AsmLabel *l = asm_find_lbl(a, s);
    if (l) return l->addr;

    char buf2[256];
    snprintf(buf2, sizeof(buf2), "%s", s);
    char *plus = strchr(buf2, '+');
    if (!plus) plus = strchr(buf2, '-');
    if (plus) {
        int sign = (*plus == '+') ? 1 : -1;
        *plus = '\0';
        char *lp = buf2;
        while (*lp == ' ' || *lp == '\t') lp++;
        char *rp = lp + strlen(lp) - 1;
        while (rp > lp && (*rp == ' ' || *rp == '\t')) { *rp = '\0'; rp--; }
        const char *off_str = plus + 1;
        while (*off_str == ' ' || *off_str == '\t') off_str++;
        AsmLabel *l2 = asm_find_lbl(a, lp);
        if (l2) return l2->addr + sign * asm_parse_imm(off_str);
        int eq2 = asm_find_eq(a, lp);
        if (eq2 >= 0) return eq2 + sign * asm_parse_imm(off_str);
        return sign * asm_parse_imm(off_str);
    }

    return 0;
}

static void asm_emit(DhadAsm *a, uint8_t b) {
    if (a->pass == 2 && a->prog_size < ASM_PROG_MAX)
        a->program[a->prog_size] = b;
    a->prog_size++;
}

static void asm_emit16(DhadAsm *a, uint16_t v) {
    asm_emit(a, v & 0xFF);
    asm_emit(a, (v >> 8) & 0xFF);
}

static int asm_parse_line(DhadAsm *a, const char *line) {
    char buf[256];
    const char *p = line;

    p = asm_skip_sp(p);
    if (*p == '\0' || *p == '#') return 0;
    if (*p == '#') return 0;

    char name[64];
    int name_len = 0;
    const char *np = p;
    while (*np && !asm_is_space(*np) && *np != '#') np++;
    if (*(np) == ':') {
        name_len = (int)(np - p);
        if (name_len > 0 && name_len < 64) {
            memcpy(name, p, name_len);
            name[name_len] = '\0';
            p = np + 1;
            p = asm_skip_sp(p);
            if (*p == '\0' || *p == '#') {
                if (a->pass >= 1) asm_add_lbl(a, name, a->prog_size);
                return 0;
            }
        }
    }

    p = asm_read_token(p, buf, sizeof(buf));
    if (buf[0] == '\0') return 0;

    if (strcmp(buf, ".macro") == 0) {
        char mname[64];
        p = asm_read_token(p, mname, sizeof(mname));
        if (a->pass == 1 && a->macro_count < ASM_MAX_MACROS) {
            AsmMacro *m = &a->macros[a->macro_count++];
            snprintf(m->name, sizeof(m->name), "%s", mname);
            m->line_count = 0;
        }
        a->in_macro_def = 1;
        snprintf(a->current_macro, sizeof(a->current_macro), "%s", mname);
        return 0;
    }
    if (strcmp(buf, ".endm") == 0) {
        a->in_macro_def = 0;
        a->current_macro[0] = '\0';
        return 0;
    }

    if (a->in_macro_def) {
        if (a->pass == 1) {
            AsmMacro *m = asm_find_macro(a, a->current_macro);
            if (m && m->line_count < ASM_MAX_MACRO_LINES) {
                snprintf(m->lines[m->line_count], 256, "%s", line);
                m->line_count++;
            }
        }
        return 0;
    }

    if (!a->in_macro_def) {
        AsmMacro *m = asm_find_macro(a, buf);
        if (m) {
            for (int i = 0; i < m->line_count; i++) {
                asm_parse_line(a, m->lines[i]);
            }
            return 0;
        }
    }

    if (strcmp(buf, ".if") == 0) {
        char expr_buf[128];
        int ei = 0;
        while (*p && *p != '\n' && *p != '#' && ei < 127) {
            if ((unsigned char)*p == 0xD8 && (unsigned char)*(p+1) == 0x8C) break;
            expr_buf[ei++] = *p++;
        }
        expr_buf[ei] = '\0';
        int val = asm_parse_expr(a, expr_buf);
        int parent_skip = (a->if_nesting > 0) ? a->if_skip_stack[a->if_nesting - 1] : 0;
        if (a->if_nesting < ASM_MAX_IF_NESTING)
            a->if_skip_stack[a->if_nesting] = parent_skip || (val == 0);
        a->if_nesting++;
        return 0;
    }
    if (strcmp(buf, ".else") == 0) {
        if (a->if_nesting > 0) {
            int top = a->if_nesting - 1;
            int parent_skip = (top > 0) ? a->if_skip_stack[top - 1] : 0;
            a->if_skip_stack[top] = parent_skip || !a->if_skip_stack[top];
        }
        return 0;
    }
    if (strcmp(buf, ".endif") == 0) {
        if (a->if_nesting > 0) a->if_nesting--;
        return 0;
    }
    if (a->if_nesting > 0 && a->if_skip_stack[a->if_nesting - 1]) return 0;

    if (name_len == 0) {
        const char *check = line;
        check = asm_skip_sp(check);
        char tmp[64];
        const char *end = check;
        while (*end && !asm_is_space(*end) && *end != ':' && *end != '#') end++;
        int tlen = (int)(end - check);
        if (tlen > 0 && tlen < 64 && *end == ':') {
            memcpy(tmp, check, tlen);
            tmp[tlen] = '\0';
            asm_add_lbl(a, tmp, a->prog_size);
            p = end + 1;
            p = asm_skip_sp(p);
            if (*p == '\0' || *p == '#') return 0;
            p = asm_read_token(p, buf, sizeof(buf));
            if (buf[0] == '\0') return 0;
        }
    }

    if (strcmp(buf, "توقف") == 0 || strcmp(buf, "halt") == 0) {
        asm_emit(a, 0xF0); asm_emit(a, 0xD3); return 0;
    }
    if (strcmp(buf, "سكون") == 0 || strcmp(buf, "nop") == 0) {
        asm_emit(a, 0x00); return 0;
    }
    if (strcmp(buf, "حمّل") == 0 || strcmp(buf, "load") == 0) {
        int r = 8;
        char tok2[64];
        p = asm_read_token(p, tok2, sizeof(tok2));
        if (asm_is_reg(tok2, &r)) {
            char expr_buf[128] = {0};
            p = asm_skip_sp(p);
            int ei = 0;
            while (*p && *p != '\n' && *p != '#' && ei < 127) {
                if ((unsigned char)*p == 0xD8 && (unsigned char)*(p+1) == 0x8C) break;
                expr_buf[ei++] = *p++;
            }
            expr_buf[ei] = '\0';
            while (ei > 0 && (expr_buf[ei-1] == ' ' || expr_buf[ei-1] == '\t')) expr_buf[--ei] = '\0';
            int val = asm_parse_expr(a, expr_buf);
            asm_emit(a, 0x10 | (r & 0x0F));
            asm_emit(a, (uint8_t)(val & 0xFF));
        }
        return 0;
    }
    if (strcmp(buf, "نقل") == 0 || strcmp(buf, "mov") == 0) {
        int dst = 0, src = 0;
        char tok2[64], tok3[64];
        p = asm_read_token(p, tok2, sizeof(tok2));
        p = asm_read_token(p, tok3, sizeof(tok3));
        if (asm_is_reg(tok2, &dst) && asm_is_reg(tok3, &src)) {
            asm_emit(a, 0x20 | (src & 0x0F));
            asm_emit(a, (uint8_t)(dst & 0x0F));
        }
        return 0;
    }
    if (strcmp(buf, "اقرأ") == 0 || strcmp(buf, "ld") == 0) {
        int r = 0;
        char tok2[64], tok3[64];
        p = asm_read_token(p, tok2, sizeof(tok2));
        p = asm_read_token(p, tok3, sizeof(tok3));
        if (asm_is_reg(tok2, &r)) {
            int addr = asm_parse_imm(tok3);
            asm_emit(a, 0x30 | (r & 0x0F));
            asm_emit(a, (uint8_t)(addr & 0xFF));
        }
        return 0;
    }
    if (strcmp(buf, "خزن") == 0 || strcmp(buf, "st") == 0) {
        int r = 0;
        char tok2[64], tok3[64];
        p = asm_read_token(p, tok2, sizeof(tok2));
        p = asm_read_token(p, tok3, sizeof(tok3));
        if (asm_is_reg(tok2, &r)) {
            int addr = asm_parse_imm(tok3);
            asm_emit(a, 0x40 | (r & 0x0F));
            asm_emit(a, (uint8_t)(addr & 0xFF));
        }
        return 0;
    }
    if (strcmp(buf, "ادفع") == 0 || strcmp(buf, "push") == 0) {
        asm_emit(a, 0x50); return 0;
    }
    if (strcmp(buf, "اسحب") == 0 || strcmp(buf, "pop") == 0) {
        asm_emit(a, 0x60); return 0;
    }
    if (strcmp(buf, "نداء") == 0 || strcmp(buf, "call") == 0) {
        char tok2[64];
        p = asm_read_token(p, tok2, sizeof(tok2));
        int addr = asm_parse_imm(tok2);
        AsmLabel *lbl = asm_find_lbl(a, tok2);
        if (lbl) addr = lbl->addr;
        asm_emit(a, 0x70);
        asm_emit16(a, (uint16_t)addr);
        return 0;
    }
    if (strcmp(buf, "إرجاع") == 0 || strcmp(buf, "ارجع") == 0 || strcmp(buf, "ret") == 0) {
        asm_emit(a, 0x80); return 0;
    }
    if (strcmp(buf, "تبريد") == 0 || strcmp(buf, "neg") == 0) {
        asm_emit(a, 0x8F); return 0;
    }
    if (strcmp(buf, "جمع") == 0 || strcmp(buf, "add") == 0) {
        int r = 0;
        char tok2[64];
        p = asm_read_token(p, tok2, sizeof(tok2));
        if (asm_is_reg(tok2, &r)) {
            asm_emit(a, 0x90 | (r & 0x0F));
        }
        return 0;
    }
    if (strcmp(buf, "طرح") == 0 || strcmp(buf, "sub") == 0) {
        int r = 0;
        char tok2[64];
        p = asm_read_token(p, tok2, sizeof(tok2));
        if (asm_is_reg(tok2, &r)) {
            asm_emit(a, 0xA0 | (r & 0x0F));
        }
        return 0;
    }
    if (strcmp(buf, "ضرب") == 0 || strcmp(buf, "mul") == 0) {
        int r = 0;
        char tok2[64];
        p = asm_read_token(p, tok2, sizeof(tok2));
        if (asm_is_reg(tok2, &r)) {
            asm_emit(a, 0xB0 | (r & 0x0F));
        }
        return 0;
    }
    if (strcmp(buf, "قسمة") == 0 || strcmp(buf, "div") == 0) {
        int r = 0;
        char tok2[64];
        p = asm_read_token(p, tok2, sizeof(tok2));
        if (asm_is_reg(tok2, &r)) {
            asm_emit(a, 0xC0 | (r & 0x0F));
        }
        return 0;
    }
    if (strcmp(buf, "باقي") == 0 || strcmp(buf, "mod") == 0) {
        int r = 0;
        char tok2[64];
        p = asm_read_token(p, tok2, sizeof(tok2));
        if (asm_is_reg(tok2, &r)) {
            asm_emit(a, 0xD0 | (r & 0x0F));
        }
        return 0;
    }
    if (strcmp(buf, "اطبع") == 0 || strcmp(buf, "print") == 0) {
        asm_emit(a, 0xE0); return 0;
    }
    if (strcmp(buf, "اطبع_حرف") == 0 || strcmp(buf, "print_ch") == 0) {
        asm_emit(a, 0xF0); asm_emit(a, 0x50); return 0;
    }
    if (strcmp(buf, "ادخل") == 0 || strcmp(buf, "input") == 0) {
        asm_emit(a, 0xF0); asm_emit(a, 0x60); return 0;
    }
    if (strcmp(buf, "قفز") == 0 || strcmp(buf, "jmp") == 0) {
        char tok2[64];
        p = asm_read_token(p, tok2, sizeof(tok2));
        int addr = asm_parse_imm(tok2);
        AsmLabel *lbl = asm_find_lbl(a, tok2);
        if (lbl) addr = lbl->addr;
        asm_emit(a, 0xF0); asm_emit(a, 0x00); asm_emit16(a, (uint16_t)addr);
        return 0;
    }
    if (strcmp(buf, "قفز_إذا_صفر") == 0 || strcmp(buf, "jz") == 0) {
        char tok2[64];
        p = asm_read_token(p, tok2, sizeof(tok2));
        int addr = asm_parse_imm(tok2);
        AsmLabel *lbl = asm_find_lbl(a, tok2);
        if (lbl) addr = lbl->addr;
        asm_emit(a, 0xF0); asm_emit(a, 0x10); asm_emit16(a, (uint16_t)addr);
        return 0;
    }
    if (strcmp(buf, "قفز_إذا_غيرصفر") == 0 || strcmp(buf, "jnz") == 0) {
        char tok2[64];
        p = asm_read_token(p, tok2, sizeof(tok2));
        int addr = asm_parse_imm(tok2);
        AsmLabel *lbl = asm_find_lbl(a, tok2);
        if (lbl) addr = lbl->addr;
        asm_emit(a, 0xF0); asm_emit(a, 0x20); asm_emit16(a, (uint16_t)addr);
        return 0;
    }
    if (strcmp(buf, "قفز_إذا_حمل") == 0 || strcmp(buf, "jc") == 0) {
        char tok2[64];
        p = asm_read_token(p, tok2, sizeof(tok2));
        int addr = asm_parse_imm(tok2);
        AsmLabel *lbl = asm_find_lbl(a, tok2);
        if (lbl) addr = lbl->addr;
        asm_emit(a, 0xF0); asm_emit(a, 0x30); asm_emit16(a, (uint16_t)addr);
        return 0;
    }
    if (strcmp(buf, "قفز_إذا_سالب") == 0 || strcmp(buf, "jn") == 0) {
        char tok2[64];
        p = asm_read_token(p, tok2, sizeof(tok2));
        int addr = asm_parse_imm(tok2);
        AsmLabel *lbl = asm_find_lbl(a, tok2);
        if (lbl) addr = lbl->addr;
        asm_emit(a, 0xF0); asm_emit(a, 0x40); asm_emit16(a, (uint16_t)addr);
        return 0;
    }
    if (strcmp(buf, "قارن") == 0 || strcmp(buf, "cmp") == 0) {
        int r = 0;
        char tok2[64];
        p = asm_read_token(p, tok2, sizeof(tok2));
        /* س1-س3 محجوزة للمقاطعات (E1-E3) — رفض صريح بدل ترميز متباين.
         * استخدم س0 أو س4-س7 (انظر ISA_COMPATIBILITY.md). */
        if (asm_is_reg(tok2, &r)) {
            if (r >= 1 && r <= 3) {
                a->error_count++;
                snprintf(a->last_error, sizeof(a->last_error), "خطأ %d: قارن مع س%d غير مدعوم (0x%X محجوز للمقاطعات) — استخدم س0 أو س4-س7", a->line, r, 0xE0 | r);
            } else {
                asm_emit(a, 0xF0);
                asm_emit(a, 0xE0 | (r & 0x0F));
            }
        }
        return 0;
    }
    if (strcmp(buf, "فعّل_مقاطعات") == 0 || strcmp(buf, "ei") == 0) {
        asm_emit(a, 0xF0); asm_emit(a, 0xE1); return 0;
    }
    if (strcmp(buf, "عطّل_مقاطعات") == 0 || strcmp(buf, "di") == 0) {
        asm_emit(a, 0xF0); asm_emit(a, 0xE2); return 0;
    }
    if (strcmp(buf, "إرجاع_مقاطععة") == 0 || strcmp(buf, "إرجاع_مقاطععة") == 0 || strcmp(buf, "reti") == 0) {
        asm_emit(a, 0xF0); asm_emit(a, 0xE3); return 0;
    }
    if (strcmp(buf, "أوحصري") == 0 || strcmp(buf, "xor") == 0) {
        int r = 0;
        char tok2[64];
        p = asm_read_token(p, tok2, sizeof(tok2));
        if (asm_is_reg(tok2, &r)) { asm_emit(a, 0xF0); asm_emit(a, 0x70 | (r & 0x0F)); }
        return 0;
    }
    if (strcmp(buf, "أومنطقي") == 0 || strcmp(buf, "or") == 0) {
        int r = 0;
        char tok2[64];
        p = asm_read_token(p, tok2, sizeof(tok2));
        if (asm_is_reg(tok2, &r)) { asm_emit(a, 0xF0); asm_emit(a, 0x80 | (r & 0x0F)); }
        return 0;
    }
    if (strcmp(buf, "ومنطقي") == 0 || strcmp(buf, "and") == 0) {
        int r = 0;
        char tok2[64];
        p = asm_read_token(p, tok2, sizeof(tok2));
        if (asm_is_reg(tok2, &r)) { asm_emit(a, 0xF0); asm_emit(a, 0x90 | (r & 0x0F)); }
        return 0;
    }
    if (strcmp(buf, "ليس") == 0 || strcmp(buf, "not") == 0) {
        asm_emit(a, 0xF0); asm_emit(a, 0xA0); return 0;
    }
    if (strcmp(buf, "تحريك_يسار") == 0 || strcmp(buf, "shl") == 0) {
        asm_emit(a, 0xF0); asm_emit(a, 0xB0); return 0;
    }
    if (strcmp(buf, "تحريك_يمين") == 0 || strcmp(buf, "shr") == 0) {
        asm_emit(a, 0xF0); asm_emit(a, 0xC0); return 0;
    }
    if (strcmp(buf, "بدل") == 0 || strcmp(buf, "swap") == 0) {
        int dst = 0, src = 0;
        char tok2[64], tok3[64];
        p = asm_read_token(p, tok2, sizeof(tok2));
        p = asm_read_token(p, tok3, sizeof(tok3));
        if (asm_is_reg(tok2, &dst) && asm_is_reg(tok3, &src)) {
            asm_emit(a, 0xF0);
            asm_emit(a, 0xD0);
            asm_emit(a, (uint8_t)(((dst & 0x0F) << 4) | (src & 0x0F)));
        }
        return 0;
    }
    /* اقرأ غير مباشر: LDRI dst, hi, lo → F0 F1 ((dst<<4)|hi) lo (4 بايت حسب المواصفة) */
    if (strcmp(buf, "اقرأ_غيرمباشر") == 0 || strcmp(buf, "ldri") == 0) {
        int dst = 0, hi = 0, lo = 0;
        char tok2[64], tok3[64], tok4[64];
        p = asm_read_token(p, tok2, sizeof(tok2));
        p = asm_read_token(p, tok3, sizeof(tok3));
        p = asm_read_token(p, tok4, sizeof(tok4));
        if (asm_is_reg(tok2, &dst) && asm_is_reg(tok3, &hi) && asm_is_reg(tok4, &lo)) {
            asm_emit(a, 0xF0);
            asm_emit(a, 0xF1);
            asm_emit(a, (uint8_t)(((dst & 0x0F) << 4) | (hi & 0x0F)));
            asm_emit(a, (uint8_t)(lo & 0x0F));
        }
        return 0;
    }
    /* خزن غير مباشر: STRI src, hi, lo → F0 F2 ((src<<4)|hi) lo (4 بايت حسب المواصفة) */
    if (strcmp(buf, "خزن_غيرمباشر") == 0 || strcmp(buf, "stri") == 0) {
        int src = 0, hi = 0, lo = 0;
        char tok2[64], tok3[64], tok4[64];
        p = asm_read_token(p, tok2, sizeof(tok2));
        p = asm_read_token(p, tok3, sizeof(tok3));
        p = asm_read_token(p, tok4, sizeof(tok4));
        if (asm_is_reg(tok2, &src) && asm_is_reg(tok3, &hi) && asm_is_reg(tok4, &lo)) {
            asm_emit(a, 0xF0);
            asm_emit(a, 0xF2);
            asm_emit(a, (uint8_t)(((src & 0x0F) << 4) | (hi & 0x0F)));
            asm_emit(a, (uint8_t)(lo & 0x0F));
        }
        return 0;
    }
    if (strcmp(buf, "نقص") == 0 || strcmp(buf, "dec") == 0) {
        int r = 0;
        char tok2[64];
        p = asm_read_token(p, tok2, sizeof(tok2));
        if (asm_is_reg(tok2, &r)) { asm_emit(a, 0xF0); asm_emit(a, 0xD1); asm_emit(a, (uint8_t)(r & 0x0F)); }
        return 0;
    }
    if (strcmp(buf, "وزد") == 0 || strcmp(buf, "inc") == 0) {
        int r = 0;
        char tok2[64];
        p = asm_read_token(p, tok2, sizeof(tok2));
        if (asm_is_reg(tok2, &r)) { asm_emit(a, 0xF0); asm_emit(a, 0xD2); asm_emit(a, (uint8_t)(r & 0x0F)); }
        return 0;
    }

    {
        const char *eq_check = line;
        eq_check = asm_skip_sp(eq_check);
        char eq_name[64];
        const char *eq_end = eq_check;
        while (*eq_end && !asm_is_space(*eq_end) && *eq_end != '=' && *eq_end != ':' && *eq_end != '#') eq_end++;
        const char *eq_sign = eq_end;
        while (asm_is_space(*eq_sign)) eq_sign++;
        int eq_len = (int)(eq_end - eq_check);
        if (eq_len > 0 && eq_len < 64 && *eq_sign == '=') {
            memcpy(eq_name, eq_check, eq_len);
            eq_name[eq_len] = '\0';
            const char *eq_val = eq_sign + 1;
            eq_val = asm_skip_sp(eq_val);
            char val_buf[64];
            const char *vp = eq_val;
            int vi = 0;
            while (*vp && *vp != '\n' && *vp != '#' && vi < 63) {
                if (asm_is_space(*vp) || *vp == ',') break;
                val_buf[vi++] = *vp++;
            }
            val_buf[vi] = '\0';
            if (vi > 0) {
                int v = asm_parse_expr(a, val_buf);
                asm_add_eq(a, eq_name, v);
            }
            return 0;
        }
    }

    if (strcmp(buf, ".db") == 0 || strcmp(buf, "بيانات") == 0) {
        char tok[64];
        while ((p = asm_read_token(p, tok, sizeof(tok))), tok[0] != '\0') {
            if (tok[0] == '"') {
                const char *s = tok + 1;
                while (*s && *s != '"') {
                    if (*s == '\\' && *(s+1)) { s++; }
                    asm_emit(a, (uint8_t)*s);
                    s++;
                }
            } else {
                asm_emit(a, (uint8_t)(asm_parse_expr(a, tok) & 0xFF));
            }
        }
        return 0;
    }
    if (strcmp(buf, ".dw") == 0 || strcmp(buf, "كلمة") == 0) {
        char tok[64];
        while ((p = asm_read_token(p, tok, sizeof(tok))), tok[0] != '\0') {
            asm_emit16(a, (uint16_t)(asm_parse_expr(a, tok) & 0xFFFF));
        }
        return 0;
    }
    if (strcmp(buf, ".ds") == 0 || strcmp(buf, "نص") == 0) {
        char tok[64];
        p = asm_read_token(p, tok, sizeof(tok));
        if (tok[0] == '"') {
            const char *s = tok + 1;
            while (*s && *s != '"') {
                if (*s == '\\' && *(s+1)) { s++; }
                asm_emit(a, (uint8_t)*s);
                s++;
            }
            asm_emit(a, 0);
        }
        return 0;
    }
    if (strcmp(buf, ".org") == 0) {
        char tok[64];
        p = asm_read_token(p, tok, sizeof(tok));
        int new_addr = asm_parse_expr(a, tok);
        while (a->prog_size < new_addr) asm_emit(a, 0);
        return 0;
    }
    if (strcmp(buf, ".align") == 0) {
        char tok[64];
        p = asm_read_token(p, tok, sizeof(tok));
        int align = asm_parse_expr(a, tok);
        if (align > 0) {
            int rem = a->prog_size % align;
            if (rem != 0) {
                int pad = align - rem;
                while (pad-- > 0) asm_emit(a, 0);
            }
        }
        return 0;
    }
    if (strcmp(buf, ".include") == 0 || strcmp(buf, "تضمين") == 0) {
        char tok[256];
        p = asm_read_token(p, tok, sizeof(tok));
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
                    asm_parse_line(a, iline);
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
    snprintf(a->last_error, sizeof(a->last_error), "خطأ %d: تعليمة غير معروفة '%s'", a->line, buf);
    return -1;
}

int dhad_asm(DhadAsm *a, const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        snprintf(a->last_error, sizeof(a->last_error), "لا يمكن فتح الملف: %s", filename);
        return -1;
    }

    char line[512];

    for (a->pass = 1; a->pass <= 2; a->pass++) {
        a->prog_size = 0;
        if (a->pass == 1) { a->label_count = 0; a->equate_count = 0; }
        a->line = 0;
        rewind(f);

        while (fgets(line, sizeof(line), f)) {
            a->line++;
            unsigned char *ul = (unsigned char *)line;
            if (ul[0] == 0xEF && ul[1] == 0xBB && ul[2] == 0xBF) {
                asm_parse_line(a, (const char *)(ul + 3));
            } else {
                asm_parse_line(a, line);
            }
        }
    }

    fclose(f);

    if (a->error_count > 0) {
        snprintf(a->last_error, sizeof(a->last_error), "%d خطأ في التجميع", a->error_count);
        return -1;
    }
    return 0;
}
