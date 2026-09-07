/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ║                                                                            ║
 * ║   المحاكي البصري — معالج ض (Dhad Visual Simulator)                        ║
 * ║                                                                            ║
 * ║   يعرض حالة المعالج بصرياً في التيرمنال مع ألوان ANSI                     ║
 * ║   وشجرة تعليمات حية وذاكرة متحركة                                        ║
 * ║                                                                            ║
 * ╚══════════════════════════════════════════════════════════════════════════════
 */

#define _DEFAULT_SOURCE
#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include "dhad_isa_constants.h"

/* ═══════════════════════════════════════════════════════════════════════════════
 *  الألوان
 * ═══════════════════════════════════════════════════════════════════════════════ */

#define CLR_RESET       "\033[0m"
#define CLR_BOLD        "\033[1m"
#define CLR_DIM         "\033[2m"
#define CLR_RED         "\033[31m"
#define CLR_GREEN       "\033[32m"
#define CLR_YELLOW      "\033[33m"
#define CLR_BLUE        "\033[34m"
#define CLR_MAGENTA     "\033[35m"
#define CLR_CYAN        "\033[36m"
#define CLR_WHITE       "\033[37m"
#define CLR_BG_BLACK    "\033[40m"
#define CLR_BG_RED      "\033[41m"
#define CLR_BG_GREEN    "\033[42m"
#define CLR_BG_YELLOW   "\033[43m"
#define CLR_BG_BLUE     "\033[44m"
#define CLR_BG_MAGENTA  "\033[45m"
#define CLR_BG_CYAN     "\033[46m"
#define CLR_BG_WHITE    "\033[47m"

/* ═══════════════════════════════════════════════════════════════════════════════
 *  هيكل المعالج
 * ═══════════════════════════════════════════════════════════════════════════════ */

typedef struct {
    uint8_t     regs[8];         /* س0 - س7 */
    uint8_t     acc;             /* المح */
    uint8_t     flags;           /* الأعلام */
    uint16_t    pc;              /* عداد البرنامج */
    uint8_t     sp;              /* مؤشر المكدس */
    uint8_t     memory[65536];   /* الذاكرة الكاملة */
    bool        halted;
    uint32_t    cycle;           /* عدد الدورات */
    uint16_t    stack[32];       /* المكدس المرئي */
    int         stack_top;       /* أعلى عنصر في المكدس */
    uint8_t     io_out;          /* منفذ الإخراج */
    uint8_t     io_in;           /* منفذ الإدخال */
    char        last_output[256];/* آخر إخراج */
    int         output_len;      /* طول آخر إخراج */
    uint16_t    program_size;    /* حجم البرنامج */
    bool        int_enabled;     /* تفعيل المقاطعات */
    uint8_t     int_mask;        /* قناع المقاطعات */
    uint8_t     int_pending;     /* مقاطعات معلقة */
    uint16_t    ivt_addr;        /* عنوان جدول المتجهات */
} VisCPU;

/* ═══════════════════════════════════════════════════════════════════════════════
 *  الرسم
 * ═══════════════════════════════════════════════════════════════════════════════ */

static void clear_screen(void) {
    printf("\033[2J\033[H");
}

static void goto_xy(int x, int y) {
    printf("\033[%d;%dH", y, x);
}

static void draw_box(int x, int y, int w, int h, const char *title, const char *color) {
    goto_xy(x, y);
    printf("%s╔", color);
    for (int i = 0; i < w - 2; i++) printf("═");
    printf("╗");

    if (title) {
        goto_xy(x + 2, y);
        printf(" %s%s%s ", CLR_BOLD, title, CLR_RESET);
    }

    for (int i = 1; i < h - 1; i++) {
        goto_xy(x, y + i);
        printf("%s║", color);
        goto_xy(x + w - 1, y + i);
        printf("║%s", CLR_RESET);
    }

    goto_xy(x, y + h - 1);
    printf("%s╚", color);
    for (int i = 0; i < w - 2; i++) printf("═");
    printf("╝%s", CLR_RESET);
}

static void draw_line(int x, int y, int w, const char *color) {
    goto_xy(x, y);
    printf("%s", color);
    for (int i = 0; i < w; i++) printf("─");
    printf("%s", CLR_RESET);
}

/* ═══════════════════════════════════════════════════════════════════════════════
 *  عرض السجلات
 * ═══════════════════════════════════════════════════════════════════════════════ */

static const char *reg_colors[] = {
    CLR_CYAN, CLR_GREEN, CLR_YELLOW, CLR_MAGENTA,
    CLR_CYAN, CLR_GREEN, CLR_YELLOW, CLR_MAGENTA
};

static const char *reg_names_ar[] = {
    "س0", "س1", "س2", "س3", "س4", "س5", "س6", "س7"
};

static void draw_registers(VisCPU *cpu, int x, int y) {
    draw_box(x, y, 22, 12, "═══ السجلات ═══", CLR_CYAN);

    for (int i = 0; i < 8; i++) {
        goto_xy(x + 2, y + 2 + i);
        printf("%s%s%s: %s0x%X%s",
            CLR_BOLD, reg_colors[i], reg_names_ar[i],
            CLR_WHITE, cpu->regs[i], CLR_RESET);
    }

    goto_xy(x + 2, y + 10);
    printf("%sمح%s: %s0x%X%s (%d)",
        CLR_BOLD, CLR_RED, CLR_WHITE, cpu->acc, CLR_RESET, cpu->acc);
}

/* ═══════════════════════════════════════════════════════════════════════════════
 *  عرض الأعلام
 * ═══════════════════════════════════════════════════════════════════════════════ */

static void draw_flags(VisCPU *cpu, int x, int y) {
    draw_box(x, y, 22, 6, "═══ الأعلام ═══", CLR_BLUE);

    const char *flag_names[] = { "Z(صفر)", "N(سالب)", "C(حمل)", "V(فيض)" };
    uint8_t flag_bits[] = { 0x01, 0x02, 0x04, 0x08 };

    for (int i = 0; i < 4; i++) {
        goto_xy(x + 2, y + 2 + i);
        bool set = (cpu->flags & flag_bits[i]) != 0;
        printf("%s%s: %s%s%s",
            CLR_BOLD, flag_names[i],
            set ? CLR_BG_GREEN : CLR_BG_RED,
            set ? " 1 " : " 0 ",
            CLR_RESET);
    }
}

/* ═══════════════════════════════════════════════════════════════════════════════
 *  عرض عداد البرنامج والحالة
 * ═══════════════════════════════════════════════════════════════════════════════ */

static void draw_pc_state(VisCPU *cpu, int x, int y) {
    draw_box(x, y, 22, 5, "═══ الحالة ═══", CLR_YELLOW);

    goto_xy(x + 2, y + 2);
    printf("%sPC%s: %s0x%04X%s",
        CLR_BOLD, CLR_YELLOW, CLR_WHITE, cpu->pc, CLR_RESET);

    goto_xy(x + 2, y + 3);
    printf("%sالدورة%s: %s%d%s",
        CLR_BOLD, CLR_YELLOW, CLR_WHITE, cpu->cycle, CLR_RESET);

    goto_xy(x + 2, y + 4);
    printf("%sالحالة%s: %s%s%s",
        CLR_BOLD, CLR_YELLOW,
        cpu->halted ? CLR_RED : CLR_GREEN,
        cpu->halted ? " متوقف " : " يعمل  ",
        CLR_RESET);
}

/* ═══════════════════════════════════════════════════════════════════════════════
 *  عرض المكدس
 * ═══════════════════════════════════════════════════════════════════════════════ */

static void draw_stack(VisCPU *cpu, int x, int y) {
    draw_box(x, y, 22, 12, "═══ المكدس ═══", CLR_MAGENTA);

    goto_xy(x + 2, y + 2);
    printf("%sSP%s: %s%d%s",
        CLR_BOLD, CLR_MAGENTA, CLR_WHITE, cpu->sp, CLR_RESET);

    draw_line(x + 1, y + 3, 20, CLR_DIM);

    int start = (cpu->stack_top >= 5) ? cpu->stack_top - 4 : 0;
    int end = (cpu->stack_top >= 5) ? cpu->stack_top + 1 : 6;

    for (int i = start; i < end && i < 32; i++) {
        goto_xy(x + 2, y + 4 + (i - start));
        if (i <= cpu->stack_top) {
            bool is_top = (i == cpu->stack_top);
            printf("%s[%d]%s %s0x%04X%s",
                is_top ? CLR_BOLD : CLR_DIM,
                i, CLR_RESET,
                is_top ? CLR_WHITE : CLR_DIM,
                cpu->stack[i], CLR_RESET);
        } else {
            printf("%s[%d]%s %s---%s",
                CLR_DIM, i, CLR_RESET, CLR_DIM, CLR_RESET);
        }
    }
}

/* ═══════════════════════════════════════════════════════════════════════════════
 *  عرض الذاكرة
 * ═══════════════════════════════════════════════════════════════════════════════ */

static void draw_memory(VisCPU *cpu, int x, int y) {
    draw_box(x, y, 36, 18, "═══ الذاكرة ═══", CLR_GREEN);

    goto_xy(x + 2, y + 2);
    printf("%sالعنوان  │ القيمة │ الرمز%s", CLR_BOLD, CLR_RESET);
    draw_line(x + 1, y + 3, 34, CLR_DIM);

    /* عرض الذاكرة حول PC */
    uint16_t start = (cpu->pc >= 4) ? cpu->pc - 4 : 0;
    if (start < 0x800) start = 0x800;
    if (start > cpu->program_size + 0x800) start = cpu->program_size + 0x800;

    for (int i = 0; i < 12 && (start + i) <= 0xFFF; i++) {
        uint16_t addr = start + i;
        uint8_t val = cpu->memory[addr];
        bool is_current = (addr == cpu->pc);

        goto_xy(x + 2, y + 4 + i);
        printf("%s0x%03X%s │ %s%02X%s    │ %s%c%s",
            CLR_DIM, addr, CLR_RESET,
            is_current ? CLR_BG_GREEN : CLR_WHITE, val,
            is_current ? CLR_RESET : CLR_RESET,
            (val >= 32 && val < 127) ? CLR_CYAN : CLR_DIM,
            (val >= 32 && val < 127) ? val : '.',
            CLR_RESET);

        if (is_current) {
            goto_xy(x + 33, y + 4 + i);
            printf("%s←%s", CLR_GREEN, CLR_RESET);
        }
    }
}

/* ═══════════════════════════════════════════════════════════════════════════════
 *  عرض شجرة التعليمات
 * ═══════════════════════════════════════════════════════════════════════════════ */

static const char* get_opcode_name(uint8_t opcode4, uint8_t ext) {
    if (opcode4 == 0xF) {
        switch (ext) {
            case 0x00: return "قفز";
            case 0x10: return "قفز_إذا_صفر";
            case 0x20: return "قفز_إذا_غيرصفر";
            case 0x30: return "قفز_إذا_حمل";
            case 0x40: return "قفز_إذا_سالب";
            case 0x50: return "اطبع_حرف";
            case 0x60: return "ادخل";
            case 0x70: return "أوحصري";
            case 0x80: return "أومنطقي";
            case 0x90: return "ومنطقي";
            case 0xA0: return "ليس";
            case 0xB0: return "تحريك_يسار";
            case 0xC0: return "تحريك_يمين";
            default:   return "???";
        }
    }
    switch (opcode4) {
        case 0x0: return "توقف";
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
        default:   return "???";
    }
}

static void draw_instruction_tree(VisCPU *cpu, int x, int y) {
    draw_box(x, y, 36, 18, "═══ شجرة التعليمات ═══", CLR_CYAN);

    uint16_t start = (cpu->pc >= 3) ? cpu->pc - 3 : 0;
    for (int i = 0; i < 12; i++) {
        uint16_t addr = start + i;
        if (addr > cpu->program_size) break;

        uint8_t raw = cpu->memory[addr];
        uint8_t opc = (raw >> 4) & 0x0F;
        uint8_t reg = raw & 0x0F;
        bool is_current = (addr == cpu->pc);

        goto_xy(x + 2, y + 2 + i);

        if (is_current) {
            printf("%s▶%s ", CLR_GREEN, CLR_RESET);
        } else {
            printf("  ");
        }

        printf("%s0x%04X%s ", CLR_DIM, addr, CLR_RESET);
        printf("%s%02X%s ", CLR_WHITE, raw, CLR_RESET);

        const char *name = get_opcode_name(opc, reg & 0xF0);
        if (opc == 0xF) {
            printf("%s%s%s", CLR_MAGENTA, name, CLR_RESET);
            if (addr + 1 <= cpu->program_size) {
                uint8_t b2 = cpu->memory[addr + 1];
                printf(" %s%02X%s", CLR_DIM, b2, CLR_RESET);
            }
        } else if (opc == 0x1) {
            printf("%s%s%s", CLR_CYAN, name, CLR_RESET);
            if (addr + 1 <= cpu->program_size) {
                uint8_t val = cpu->memory[addr + 1];
                printf(" %s%s%s %s%d%s", CLR_YELLOW,
                    reg < 8 ? reg_names_ar[reg] : "مح",
                    CLR_RESET, CLR_WHITE, val, CLR_RESET);
            }
        } else {
            printf("%s%s%s", CLR_CYAN, name, CLR_RESET);
            if (reg < 8) {
                printf(" %s%s%s", CLR_YELLOW, reg_names_ar[reg], CLR_RESET);
            }
        }
    }
}

/* ═══════════════════════════════════════════════════════════════════════════════
 *  عرض الإخراج
 * ═══════════════════════════════════════════════════════════════════════════════ */

static void draw_output(VisCPU *cpu, int x, int y) {
    draw_box(x, y, 74, 6, "═══ الإخراج ═══", CLR_GREEN);

    goto_xy(x + 2, y + 2);
    printf("%s--- stdout ---", CLR_DIM);

    goto_xy(x + 2, y + 3);
    if (cpu->output_len > 0) {
        printf("%s", CLR_WHITE);
        for (int i = 0; i < cpu->output_len && i < 68; i++) {
            putchar(cpu->last_output[i]);
        }
        printf("%s", CLR_RESET);
    } else {
        printf("%s(لا يوجد إخراج بعد)%s", CLR_DIM, CLR_RESET);
    }
}

/* ═══════════════════════════════════════════════════════════════════════════════
 *  عرض شريط المعلومات السفلي
 * ═══════════════════════════════════════════════════════════════════════════════ */

static void draw_status_bar(VisCPU *cpu, int x, int y) {
    goto_xy(x, y);
    printf("%s%s", CLR_BG_BLUE, CLR_WHITE);
    printf(" معالج ض (Dhad v1.0.0 — 4-bit) │ ");
    printf("الدورة: %d │ ", cpu->cycle);
    printf("PC: 0x%03X │ ", cpu->pc);
    printf("SP: %d │ ", cpu->sp);
    printf("الأعلام: %s%s%s%s%s%s%s%s ",
        (cpu->flags & 0x01) ? CLR_GREEN : "", (cpu->flags & 0x01) ? "Z" : "",
        (cpu->flags & 0x02) ? CLR_RED : "", (cpu->flags & 0x02) ? "N" : "",
        (cpu->flags & 0x04) ? CLR_YELLOW : "", (cpu->flags & 0x04) ? "C" : "",
        (cpu->flags & 0x08) ? CLR_MAGENTA : "", (cpu->flags & 0x08) ? "V" : "");
    printf("%s", CLR_RESET);
}

/* ═══════════════════════════════════════════════════════════════════════════════
 *  تنفيذ تعليمة واحدة
 * ═══════════════════════════════════════════════════════════════════════════════ */

static void execute_one(VisCPU *cpu) {
    if (cpu->halted) return;

    uint8_t raw = cpu->memory[cpu->pc++];
    uint8_t opc = (raw >> 4) & 0x0F;
    uint8_t reg = raw & 0x0F;

    cpu->cycle++;

    switch (opc) {
        case 0x0: break; /* سكون */

        case 0x1: { /* حمّل */
            uint8_t val = cpu->memory[cpu->pc++];
            if (reg < 8) cpu->regs[reg] = val; else cpu->acc = val;
            break;
        }

        case 0x2: { /* نقل */
            uint8_t dst = cpu->memory[cpu->pc++];
            cpu->regs[dst & 0x07] = cpu->regs[reg & 0x07];
            break;
        }

        case 0x3: { /* اقرأ */
            uint8_t addr = cpu->memory[cpu->pc++];
            uint8_t v = cpu->memory[addr];
            if (reg < 8) cpu->regs[reg] = v; else cpu->acc = v;
            break;
        }

        case 0x4: { /* خزن */
            uint8_t addr = cpu->memory[cpu->pc++];
            cpu->memory[addr] = (reg < 8) ? cpu->regs[reg] : cpu->acc;
            break;
        }

        case 0x5: /* ادفع */
            if (cpu->sp < 32) {
                cpu->stack[cpu->sp] = cpu->acc;
                cpu->stack_top = cpu->sp;
                cpu->sp++;
            }
            break;

        case 0x6: /* اسحب */
            if (cpu->sp > 0) {
                cpu->sp--;
                cpu->acc = (uint8_t)(cpu->stack[cpu->sp] & 0xFF);
                cpu->stack_top = cpu->sp - 1;
            }
            break;

        case 0x7: { /* نداء */
            uint8_t lo = cpu->memory[cpu->pc++];
            uint8_t hi = cpu->memory[cpu->pc++];
            uint16_t addr = (hi << 8) | lo;
            if (cpu->sp < 32) {
                cpu->stack[cpu->sp] = cpu->pc;
                cpu->stack_top = cpu->sp;
                cpu->sp++;
                cpu->pc = addr;
            }
            break;
        }

        case 0x8: /* إرجاع / تبريد */
            if (reg == 0) {
                if (cpu->sp > 0) {
                    cpu->sp--;
                    cpu->pc = cpu->stack[cpu->sp];
                    cpu->stack_top = cpu->sp - 1;
                }
            } else {
                cpu->acc = (uint8_t)(-(int8_t)cpu->acc);
                cpu->flags = 0;
                if (cpu->acc == 0) cpu->flags |= 0x01;
                if (cpu->acc & 0x80) cpu->flags |= 0x02;
            }
            break;

        case 0x9: { /* جمع */
            uint8_t v = cpu->regs[reg & 7];
            uint16_t sum = (uint16_t)cpu->acc + (uint16_t)v;
            cpu->acc = (uint8_t)(sum & 0xFF);
            cpu->flags = 0;
            if (cpu->acc == 0) cpu->flags |= 0x01;
            if (cpu->acc & 0x80) cpu->flags |= 0x02;
            if (sum > 0xFF) cpu->flags |= 0x04;
            break;
        }

        case 0xA: { /* طرح */
            uint8_t v = cpu->regs[reg & 7];
            int diff = (int)cpu->acc - (int)v;
            cpu->flags = 0;
            cpu->acc = (uint8_t)(diff & 0xFF);
            if (cpu->acc == 0) cpu->flags |= 0x01;
            if (cpu->acc & 0x80) cpu->flags |= 0x02;
            if (diff < 0) cpu->flags |= 0x04;
            break;
        }

        case 0xB: { /* ضرب */
            uint8_t v = cpu->regs[reg & 7];
            uint16_t prod = (uint16_t)cpu->acc * (uint16_t)v;
            cpu->acc = (uint8_t)(prod & 0xFF);
            cpu->flags = 0;
            if (cpu->acc == 0) cpu->flags |= 0x01;
            if (cpu->acc & 0x80) cpu->flags |= 0x02;
            break;
        }

        case 0xC: { /* قسمة */
            uint8_t v = cpu->regs[reg & 7];
            if (v) {
                cpu->acc = cpu->acc / v;
                cpu->flags = 0;
                if (cpu->acc == 0) cpu->flags |= 0x01;
                if (cpu->acc & 0x80) cpu->flags |= 0x02;
            }
            break;
        }

        case 0xD: { /* باقي */
            uint8_t v = cpu->regs[reg & 7];
            if (v) {
                cpu->acc = cpu->acc % v;
                cpu->flags = 0;
                if (cpu->acc == 0) cpu->flags |= 0x01;
                if (cpu->acc & 0x80) cpu->flags |= 0x02;
            }
            break;
        }

        case 0xE: /* اطبع */
            {
                char buf[16];
                int len = snprintf(buf, sizeof(buf), "%d", cpu->acc);
                int curlen = (int)strlen(cpu->last_output);
                if (curlen + len < 255) {
                    memcpy(cpu->last_output + curlen, buf, len);
                    cpu->last_output[curlen + len] = '\0';
                    cpu->output_len = curlen + len;
                }
            }
            break;

        case 0xF: /* موسعة */
            {
                uint8_t ex = cpu->memory[cpu->pc++];

                if (ex <= 0x40) {
                    /* قفزات: عنوان 16-bit */
                    uint8_t lo = cpu->memory[cpu->pc++];
                    uint8_t hi = cpu->memory[cpu->pc++];
                    uint16_t addr = (hi << 8) | lo;
                    bool jump = false;
                    switch (ex) {
                        case 0x00: jump = true; break;
                        case 0x10: jump = (cpu->flags & 0x01); break;
                        case 0x20: jump = !(cpu->flags & 0x01); break;
                        case 0x30: jump = (cpu->flags & 0x04); break;
                        case 0x40: jump = (cpu->flags & 0x02); break;
                    }
                    if (jump) cpu->pc = addr;
                } else if (ex == 0x50) {
                    /* اطبع حرف */
                    int curlen = (int)strlen(cpu->last_output);
                    if (curlen + 1 < 255) {
                        cpu->last_output[curlen] = (char)cpu->acc;
                        cpu->last_output[curlen + 1] = '\0';
                        cpu->output_len = curlen + 1;
                    }
                } else if (ex == 0x60) {
                    cpu->acc = (uint8_t)(rand() % 256);
                } else if (ex == 0xD0) {
                    uint8_t op2 = cpu->memory[cpu->pc++];
                    uint8_t dst = (op2 >> 4) & 0x0F;
                    uint8_t src = op2 & 0x0F;
                    uint8_t *pd = (dst < 8) ? &cpu->regs[dst] : &cpu->acc;
                    uint8_t *ps = (src < 8) ? &cpu->regs[src] : &cpu->acc;
                    uint8_t tmp = *pd; *pd = *ps; *ps = tmp;
                } else if (ex == 0xD1) {
                    uint8_t r = cpu->memory[cpu->pc++] & 0x07;
                    cpu->regs[r]--;
                    cpu->flags = 0;
                    if (cpu->regs[r] == 0) cpu->flags |= 0x01;
                    if (cpu->regs[r] & 0x80) cpu->flags |= 0x02;
                } else if (ex == 0xD2) {
                    uint8_t r = cpu->memory[cpu->pc++] & 0x07;
                    cpu->regs[r]++;
                    cpu->flags = 0;
                    if (cpu->regs[r] == 0) cpu->flags |= 0x01;
                    if (cpu->regs[r] & 0x80) cpu->flags |= 0x02;
                } else if (ex == 0xD3) {
                    cpu->halted = true;
                } else if ((ex & 0xF0) == 0xE0) {
                    uint8_t v = cpu->regs[ex & 0x07];
                    uint16_t r = (uint16_t)cpu->acc - (uint16_t)v;
                    cpu->flags = 0;
                    if ((r & 0xFF) == 0) cpu->flags |= 0x01;
                    if (r & 0x80) cpu->flags |= 0x02;
                    if (r > 0xFF) cpu->flags |= 0x04;
                } else if (ex == 0xE1) {
                    cpu->int_enabled = true;
                } else if (ex == 0xE2) {
                    cpu->int_enabled = false;
                } else if (ex == 0xE3) {
                    uint16_t ret_addr = cpu->stack[--cpu->sp];
                    uint8_t ret_flags = (uint8_t)(cpu->stack[--cpu->sp] & 0xFF);
                    cpu->pc = ret_addr;
                    cpu->flags = ret_flags;
                    cpu->int_enabled = true;
                } else {
                    /* عمليات منطقية */
                    uint8_t rg = reg & 7;
                    switch (ex & 0xF0) {
                        case 0x70: cpu->acc ^= cpu->regs[rg]; break;
                        case 0x80: cpu->acc |= cpu->regs[rg]; break;
                        case 0x90: cpu->acc &= cpu->regs[rg]; break;
                        case 0xA0: cpu->acc = ~cpu->acc; break;
                        case 0xB0: cpu->acc <<= 1; break;
                        case 0xC0: cpu->acc >>= 1; break;
                    }
                    cpu->flags = 0;
                    if (cpu->acc == 0) cpu->flags |= 0x01;
                    if (cpu->acc & 0x80) cpu->flags |= 0x02;
                }
            }
            break;
    }
}

/* ═══════════════════════════════════════════════════════════════════════════════
 *  تحميل البرنامج
 * ═══════════════════════════════════════════════════════════════════════════════ */

static int load_program(VisCPU *cpu, const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) return -1;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (size <= 0 || size > 0xF000) {
        fclose(f);
        return -1;
    }

    memset(cpu->memory, 0, sizeof(cpu->memory));
    size_t read_result = fread(&cpu->memory[0], 1, size, f);
    if (read_result != (size_t)size) {
        fclose(f);
        return -1;
    }
    cpu->program_size = (uint16_t)size;
    fclose(f);

    cpu->pc = 0x800;
    return 0;
}

/* ═══════════════════════════════════════════════════════════════════════════════
 *  الحلقة الرئيسية
 * ═══════════════════════════════════════════════════════════════════════════════ */

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("الاستخدام: %s <ملف.bin> [أبطأ|سريع|تلقائي]\n", argv[0]);
        printf("  --slow    : خطوة بخطوة (اضغط Enter)\n");
        printf("  --fast    : تنفيذ سريع (ثاني واحدة لكل 10 دورات)\n");
        printf("  --auto    : تنفيذ تلقائي بسرعة متوسطة\n");
        return 1;
    }

    VisCPU cpu;
    memset(&cpu, 0, sizeof(cpu));
    cpu.pc = 0x800;
    cpu.stack_top = -1;

    if (load_program(&cpu, argv[1]) != 0) {
        printf("خطأ: لا يمكن تحميل الملف '%s'\n", argv[1]);
        return 1;
    }

    int mode = 0; /* 0=auto, 1=slow, 2=fast */
    if (argc > 2) {
        if (strcmp(argv[2], "--slow") == 0 || strcmp(argv[2], "أبطأ") == 0) mode = 1;
        if (strcmp(argv[2], "--fast") == 0 || strcmp(argv[2], "سريع") == 0) mode = 2;
    }

    /* الوضع التفاعلي */
    signal(SIGINT, SIG_IGN);

    while (!cpu.halted && cpu.cycle < 100000) {
        clear_screen();

        /* العنوان */
        goto_xy(2, 1);
        printf("%s%s╔══════════════════════════════════════════════════════════════════════╗%s\n", CLR_BOLD, CLR_CYAN, CLR_RESET);
        goto_xy(2, 2);
        printf("%s%s║                    محاكي المعالج — ض (Dhad Visual Simulator)         ║%s\n", CLR_BOLD, CLR_CYAN, CLR_RESET);
        goto_xy(2, 3);
        printf("%s%s╚══════════════════════════════════════════════════════════════════════╝%s\n", CLR_BOLD, CLR_CYAN, CLR_RESET);

        draw_registers(&cpu, 2, 5);
        draw_flags(&cpu, 25, 5);
        draw_pc_state(&cpu, 48, 5);
        draw_stack(&cpu, 2, 18);
        draw_memory(&cpu, 25, 18);
        draw_instruction_tree(&cpu, 62, 18);
        draw_output(&cpu, 2, 32);
        draw_status_bar(&cpu, 2, 39);

        if (mode == 1) {
            /* وضع خطوة بخطوة */
            goto_xy(2, 41);
            printf("%s%s[Enter] للخطوة التالية │ [q] للخروج │ [s] للتجميع السريع%s", CLR_BOLD, CLR_YELLOW, CLR_RESET);
            fflush(stdout);
            int ch = getchar();
            if (ch == 'q' || ch == 'Q') break;
            if (ch == 's' || ch == 'S') {
                while (!cpu.halted && cpu.cycle < 100000) {
                    execute_one(&cpu);
                }
            } else {
                execute_one(&cpu);
            }
        } else if (mode == 2) {
            usleep(100000);
            for (int i = 0; i < 5 && !cpu.halted; i++) {
                execute_one(&cpu);
            }
        } else {
            usleep(200000);
            execute_one(&cpu);
        }
    }

    /* الشاشة النهائية */
    clear_screen();
    goto_xy(2, 1);
    printf("%s%s╔══════════════════════════════════════════════════════════════════════╗%s\n", CLR_BOLD, CLR_CYAN, CLR_RESET);
    goto_xy(2, 2);
    printf("%s%s║                    محاكي المعالج — ض (Dhad Visual Simulator)         ║%s\n", CLR_BOLD, CLR_CYAN, CLR_RESET);
    goto_xy(2, 3);
    printf("%s%s╚══════════════════════════════════════════════════════════════════════╝%s\n", CLR_BOLD, CLR_CYAN, CLR_RESET);

    draw_registers(&cpu, 2, 5);
    draw_flags(&cpu, 25, 5);
    draw_pc_state(&cpu, 48, 5);
    draw_stack(&cpu, 2, 18);
    draw_memory(&cpu, 25, 18);
    draw_instruction_tree(&cpu, 62, 18);
    draw_output(&cpu, 2, 32);
    draw_status_bar(&cpu, 2, 39);

    goto_xy(2, 41);
    if (cpu.halted) {
        printf("%s%s✓ البرنامج أنهى التنفيذ بعد %d دورة%s", CLR_BOLD, CLR_GREEN, cpu.cycle, CLR_RESET);
    } else {
        printf("%s%s✗ تم إيقاف التنفيذ بعد %d دورة%s", CLR_BOLD, CLR_RED, cpu.cycle, CLR_RESET);
    }

    goto_xy(2, 43);
    printf("\n");
    return 0;
}
