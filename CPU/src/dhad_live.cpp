/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ║                                                                            ║
 * ║   المحاكي اللحظي — معالج ض (Dhad Live Simulator)                          ║
 * ║                                                                            ║
 * ║   محاكي C++ بصري يعرض كل تفاصيل المعالج بالوقت الحقيقي                    ║
 * ║   مع تحديث لحظي وتحكم كامل بالتنفيذ                                       ║
 * ║                                                                            ║
 * ╚══════════════════════════════════════════════════════════════════════════════
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include <atomic>
#include <vector>
#include <map>
#include <iomanip>
#include <algorithm>
#include <signal.h>
#include <unistd.h>

extern "C" {
#include "dhad_cpu.h"
}

using namespace std;

/* ═══════════════════════════════════════════════════════════════════════════════
 *  ثوابت المعالج — من dhad_cpu.h (المرجع الموحد)
 * ═══════════════════════════════════════════════════════════════════════════════ */

constexpr int DHAD_MEM_STACK_END  = 0x01F;
constexpr int DHAD_IO_OUT_ADDR    = 0x0F0;
constexpr int DHAD_IO_IN_ADDR     = 0x0F1;

/* ─── Opcodes ─── */
constexpr uint8_t OP_HALT    = 0x0;
constexpr uint8_t OP_LOAD    = 0x1;
constexpr uint8_t OP_MOV     = 0x2;
constexpr uint8_t OP_LDMEM   = 0x3;
constexpr uint8_t OP_STMEM   = 0x4;
constexpr uint8_t OP_PUSH    = 0x5;
constexpr uint8_t OP_POP     = 0x6;
constexpr uint8_t OP_CALL    = 0x7;
constexpr uint8_t OP_RET     = 0x8;
constexpr uint8_t OP_ADD     = 0x9;
constexpr uint8_t OP_SUB     = 0xA;
constexpr uint8_t OP_MUL     = 0xB;
constexpr uint8_t OP_DIV     = 0xC;
constexpr uint8_t OP_MOD     = 0xD;
constexpr uint8_t OP_PRINT   = 0xE;
constexpr uint8_t OP_EXT     = 0xF;

/* ─── Extended Opcodes ─── */
constexpr uint8_t EXT_JMP     = 0x00;
constexpr uint8_t EXT_JZ      = 0x10;
constexpr uint8_t EXT_JNZ     = 0x20;
constexpr uint8_t EXT_JC      = 0x30;
constexpr uint8_t EXT_JN      = 0x40;
constexpr uint8_t EXT_PRINT_CH= 0x50;
constexpr uint8_t EXT_INPUT   = 0x60;
constexpr uint8_t EXT_XOR     = 0x70;
constexpr uint8_t EXT_OR      = 0x80;
constexpr uint8_t EXT_AND     = 0x90;
constexpr uint8_t EXT_NOT     = 0xA0;
constexpr uint8_t EXT_SHL     = 0xB0;
constexpr uint8_t EXT_SHR     = 0xC0;

/* ═══════════════════════════════════════════════════════════════════════════════
 *  ألوان ANSI
 * ═══════════════════════════════════════════════════════════════════════════════ */

namespace CLR {
    const string RESET     = "\033[0m";
    const string BOLD      = "\033[1m";
    const string DIM       = "\033[2m";
    const string RED       = "\033[31m";
    const string GREEN     = "\033[32m";
    const string YELLOW    = "\033[33m";
    const string BLUE      = "\033[34m";
    const string MAGENTA   = "\033[35m";
    const string CYAN      = "\033[36m";
    const string WHITE     = "\033[37m";
    const string BG_BLACK  = "\033[40m";
    const string BG_RED    = "\033[41m";
    const string BG_GREEN  = "\033[42m";
    const string BG_YELLOW = "\033[43m";
    const string BG_BLUE   = "\033[44m";
    const string BG_CYAN   = "\033[46m";
    const string BLACK     = "\033[30m";
}

/* ═══════════════════════════════════════════════════════════════════════════════
 *  هيكل المعالج
 * ═══════════════════════════════════════════════════════════════════════════════ */

struct LiveCPU {
    /* Unified CPU core — MUST be first for proper initialization */
    DhadCPU C = {};

    /* Live-specific metadata */
    int      stack_top = -1;
    uint32_t inst_count = 0;
    string   output_buf = "";
    uint8_t  last_op     = 0;
    uint8_t  last_ext    = 0;
    uint16_t last_addr   = 0;
    uint8_t  last_operand= 0;

    LiveCPU() {
        dhad_cpu_init(&C);
        C.ivt_addr = 0x0040;  /* Live default IVT */
    }

    /* Unified CPU core access */
    DhadCPU* core() { return &C; }
};

/* ═══════════════════════════════════════════════════════════════════════════════
 *  أسماء التعليمات
 * ═══════════════════════════════════════════════════════════════════════════════ */

static const char* get_op_name(uint8_t op) {
    switch (op) {
        case OP_HALT:   return "توقف";
        case OP_LOAD:   return "حمّل";
        case OP_MOV:    return "نقل";
        case OP_LDMEM:  return "اقرأ";
        case OP_STMEM:  return "خزن";
        case OP_PUSH:   return "ادفع";
        case OP_POP:    return "اسحب";
        case OP_CALL:   return "نداء";
        case OP_RET:    return "إرجاع";
        case OP_ADD:    return "جمع";
        case OP_SUB:    return "طرح";
        case OP_MUL:    return "ضرب";
        case OP_DIV:    return "قسمة";
        case OP_MOD:    return "باقي";
        case OP_PRINT:  return "اطبع";
        case OP_EXT:    return "(موسعة)";
        default:        return "???";
    }
}

static const char* get_ext_name(uint8_t ext) {
    switch (ext) {
        case EXT_JMP:       return "قفز";
        case EXT_JZ:        return "قفز.صفر";
        case EXT_JNZ:       return "قفز.غيرصفر";
        case EXT_JC:        return "قفز.حمل";
        case EXT_JN:        return "قفز.سالب";
        case EXT_PRINT_CH:  return "اطبع.حرف";
        case EXT_INPUT:     return "ادخل";
        case EXT_XOR:       return "أوحصري";
        case EXT_OR:        return "أومنطقي";
        case EXT_AND:       return "ومنطقي";
        case EXT_NOT:       return "ليس";
        case EXT_SHL:       return " shifted.يسار";
        case EXT_SHR:       return " shifted.يمين";
        default:            return "???";
    }
}

static const char* get_reg_name(int r) {
    static const char* names[] = {"س0","س1","س2","س3","س4","س5","س6","س7"};
    if (r >= 0 && r < 8) return names[r];
    return "مح";
}

static const string& get_reg_color(int r) {
    static const string colors[] = {
        CLR::CYAN, CLR::GREEN, CLR::YELLOW, CLR::MAGENTA,
        CLR::CYAN, CLR::GREEN, CLR::YELLOW, CLR::MAGENTA
    };
    if (r >= 0 && r < 8) return colors[r];
    return CLR::RED;
}

/* ═══════════════════════════════════════════════════════════════════════════════
 *  رسم الصناديق
 * ═══════════════════════════════════════════════════════════════════════════════ */

static void cls() { cout << "\033[2J\033[H"; }
static void gotoxy(int x, int y) { cout << "\033[" << y << ";" << x << "H"; }
static void flush() { cout.flush(); }

static void draw_box(int x, int y, int w, int h, const string& title, const string& color) {
    gotoxy(x, y);
    cout << color << "╔";
    for (int i = 0; i < w - 2; i++) cout << "═";
    cout << "╗" << CLR::RESET;

    if (!title.empty()) {
        gotoxy(x + 2, y);
        cout << " " << CLR::BOLD << title << CLR::RESET << " ";
    }

    for (int i = 1; i < h - 1; i++) {
        gotoxy(x, y + i);
        cout << color << "║";
        gotoxy(x + w - 1, y + i);
        cout << "║" << CLR::RESET;
    }

    gotoxy(x, y + h - 1);
    cout << color << "╚";
    for (int i = 0; i < w - 2; i++) cout << "═";
    cout << "╝" << CLR::RESET;
}

static void draw_line(int x, int y, int w, const string& color) {
    gotoxy(x, y);
    cout << color;
    for (int i = 0; i < w; i++) cout << "─";
    cout << CLR::RESET;
}

/* ═══════════════════════════════════════════════════════════════════════════════
 *  قسم: السجلات
 * ═══════════════════════════════════════════════════════════════════════════════ */

static void draw_registers(const LiveCPU& cpu, int x, int y) {
    draw_box(x, y, 24, 12, "═══ السجلات ═══", CLR::CYAN);

    for (int i = 0; i < 8; i++) {
        gotoxy(x + 2, y + 2 + i);
        cout << get_reg_color(i) << CLR::BOLD << get_reg_name(i)
             << ": " << CLR::WHITE << "0x" << hex << (int)cpu.C.regs[i]
             << dec << " (" << (int)cpu.C.regs[i] << ")" << CLR::RESET;
    }

    gotoxy(x + 2, y + 10);
    cout << CLR::RED << CLR::BOLD << "مح"
         << ": " << CLR::WHITE << "0x" << hex << (int)cpu.C.acc
         << dec << " (" << (int)cpu.C.acc << ")" << CLR::RESET;
}

/* ═══════════════════════════════════════════════════════════════════════════════
 *  قسم: الأعلام
 * ═══════════════════════════════════════════════════════════════════════════════ */

static void draw_flags(const LiveCPU& cpu, int x, int y) {
    draw_box(x, y, 24, 6, "═══ الأعلام ═══", CLR::BLUE);

    struct { const char* name; uint8_t bit; const string& color; } flags[] = {
        {"Z(صفر) ", 0x01, CLR::GREEN},
        {"N(سالب)", 0x02, CLR::RED},
        {"C(حمل) ", 0x04, CLR::YELLOW},
        {"V(فيض) ", 0x08, CLR::MAGENTA}
    };

    for (int i = 0; i < 4; i++) {
        gotoxy(x + 2, y + 2 + i);
        bool set = (cpu.C.flags & flags[i].bit) != 0;
        cout << CLR::BOLD << flags[i].name << ": "
             << (set ? CLR::BG_GREEN : CLR::BG_RED)
             << (set ? " 1 " : " 0 ")
             << CLR::RESET;
    }
}

/* ═══════════════════════════════════════════════════════════════════════════════
 *  قسم: الحالة
 * ═══════════════════════════════════════════════════════════════════════════════ */

static void draw_state(const LiveCPU& cpu, int x, int y) {
    draw_box(x, y, 24, 7, "═══ الحالة ═══", CLR::YELLOW);

    gotoxy(x + 2, y + 2);
    cout << CLR::BOLD << "PC    : " << CLR::WHITE << "0x" << hex << setw(3) << setfill('0') << cpu.C.pc << dec << CLR::RESET;

    gotoxy(x + 2, y + 3);
    cout << CLR::BOLD << "SP    : " << CLR::WHITE << (int)cpu.C.sp << CLR::RESET;

    gotoxy(x + 2, y + 4);
    cout << CLR::BOLD << "الدورات: " << CLR::WHITE << cpu.C.cycles << CLR::RESET;

    gotoxy(x + 2, y + 5);
    cout << CLR::BOLD << "التعليمات: " << CLR::WHITE << cpu.inst_count << CLR::RESET;

    gotoxy(x + 2, y + 6);
    cout << CLR::BOLD << "الحالة: "
         << (cpu.C.halted ? CLR::RED : CLR::GREEN)
         << CLR::BOLD << (cpu.C.halted ? " متوقف " : "  يعمل  ")
         << CLR::RESET;
}

/* ═══════════════════════════════════════════════════════════════════════════════
 *  قسم: المكدس
 * ═══════════════════════════════════════════════════════════════════════════════ */

static void draw_stack(const LiveCPU& cpu, int x, int y) {
    draw_box(x, y, 24, 12, "═══ المكدس ═══", CLR::MAGENTA);

    gotoxy(x + 2, y + 2);
    cout << CLR::BOLD << "SP: " << CLR::WHITE << (int)cpu.C.sp << CLR::RESET;

    draw_line(x + 1, y + 3, 22, CLR::DIM);

    int start = (cpu.stack_top >= 5) ? cpu.stack_top - 4 : 0;
    int end = min(6, cpu.stack_top + 2);
    if (start < 0) start = 0;

    for (int i = start; i < end && i < DHAD_STACK_SIZE; i++) {
        gotoxy(x + 2, y + 4 + (i - start));
        if (i <= cpu.stack_top) {
            bool is_top = (i == cpu.stack_top);
            cout << (is_top ? CLR::BOLD : CLR::DIM) << "[" << i << "]" << CLR::RESET
                 << " " << (is_top ? CLR::WHITE : CLR::DIM)
                 << "0x" << hex << setw(4) << setfill('0') << cpu.C.stack[i] << dec << CLR::RESET;
        } else {
            cout << CLR::DIM << "[" << i << "]  ---" << CLR::RESET;
        }
    }
}

/* ═══════════════════════════════════════════════════════════════════════════════
 *  قسم: الذاكرة
 * ═══════════════════════════════════════════════════════════════════════════════ */

static void draw_memory(const LiveCPU& cpu, int x, int y) {
    draw_box(x, y, 40, 18, "═══ الذاكرة ═══", CLR::GREEN);

    gotoxy(x + 2, y + 2);
    cout << CLR::BOLD << " العنوان │ القيمة │ الحرف │ التعليمة" << CLR::RESET;
    draw_line(x + 1, y + 3, 38, CLR::DIM);

    uint16_t start = (cpu.C.pc >= 5) ? cpu.C.pc - 5 : DHAD_PROG_START;
    uint16_t end_addr = min((uint16_t)(start + 12), (uint16_t)DHAD_MEM_SIZE);

    for (int i = 0; i < 12 && (start + i) < end_addr; i++) {
        uint16_t addr = start + i;
        uint8_t val = cpu.C.memory[addr];
        bool is_current = (addr == cpu.C.pc);

        gotoxy(x + 2, y + 4 + i);

        if (is_current) cout << CLR::BG_GREEN << CLR::BLACK;
        cout << "0x" << hex << setw(4) << setfill('0') << addr << dec << "  │  "
             << CLR::WHITE << setw(2) << setfill('0') << hex << (int)val << dec << CLR::RESET << "  │  ";

        if (val >= 32 && val < 127)
            cout << CLR::CYAN << (char)val << CLR::RESET << "   │  ";
        else
            cout << CLR::DIM << "." << CLR::RESET << "   │  ";

        /* اسم التعليمة */
        if (addr == cpu.C.pc) {
            uint8_t opc = (val >> 4) & 0x0F;
            uint8_t r = val & 0x0F;
            if (opc == OP_EXT) {
                uint8_t ext_val = (addr + 1 < DHAD_MEM_SIZE) ? cpu.C.memory[addr + 1] : 0;
                cout << CLR::MAGENTA << CLR::BOLD << get_ext_name(ext_val & 0xF0) << CLR::RESET;
            } else if (opc == OP_LOAD) {
                uint8_t next = (addr + 1 < DHAD_MEM_SIZE) ? cpu.C.memory[addr + 1] : 0;
                cout << CLR::CYAN << CLR::BOLD << get_op_name(opc) << " " << (r < 8 ? get_reg_name(r) : "مح") << ", " << (int)next << CLR::RESET;
            } else if (opc == OP_CALL) {
                uint8_t lo = cpu.C.memory[addr + 1];
                uint8_t hi = cpu.C.memory[addr + 2];
                uint16_t tgt = (hi << 8) | lo;
                cout << CLR::CYAN << CLR::BOLD << get_op_name(opc) << " 0x" << hex << setw(4) << setfill('0') << tgt << dec << CLR::RESET;
            } else {
                cout << CLR::CYAN << CLR::BOLD << get_op_name(opc);
                if (r < 8 && opc != OP_HALT && opc != OP_PRINT && opc != OP_PUSH && opc != OP_POP && opc != OP_RET)
                    cout << " " << get_reg_name(r);
                cout << CLR::RESET;
            }
        }

        if (is_current) {
            cout << CLR::BG_GREEN << CLR::BLACK << " ◀" << CLR::RESET;
        }
    }
}

/* ═══════════════════════════════════════════════════════════════════════════════
 *  قسم: شجرة التعليمات
 * ═══════════════════════════════════════════════════════════════════════════════ */

static void draw_inst_tree(const LiveCPU& cpu, int x, int y) {
    draw_box(x, y, 40, 18, "═══ شجرة التعليمات ═══", CLR::CYAN);

    uint16_t start = (cpu.C.pc >= 5) ? cpu.C.pc - 5 : DHAD_PROG_START;

    for (int i = 0; i < 12; i++) {
        uint16_t addr = start + i;

        uint8_t val = cpu.C.memory[addr];
        uint8_t op4 = (val >> 4) & 0x0F;
        uint8_t reg = val & 0x0F;
        bool is_current = (addr == cpu.C.pc);

        gotoxy(x + 2, y + 2 + i);

        if (is_current) cout << CLR::GREEN << CLR::BOLD << "▶ " << CLR::RESET;
        else            cout << "  ";

        cout << CLR::DIM << "0x" << hex << setw(4) << setfill('0') << addr << dec << CLR::RESET << " ";
        cout << CLR::WHITE << setw(2) << setfill('0') << hex << (int)val << dec << CLR::RESET << " ";

        if (op4 == OP_EXT) {
            uint8_t ext_val = (addr + 1 < DHAD_MEM_SIZE) ? cpu.C.memory[addr + 1] : 0;
            uint8_t ext_op = ext_val & 0xF0;
            cout << CLR::MAGENTA << get_ext_name(ext_op);
            if (ext_op <= 0x40) {
                uint8_t lo = cpu.C.memory[addr + 1];
                uint8_t hi = cpu.C.memory[addr + 2];
                uint16_t tgt = (hi << 8) | lo;
                cout << " 0x" << hex << setw(4) << setfill('0') << tgt << dec;
            } else {
                uint8_t r = ext_val & 0x0F;
                if (r < 8) cout << " " << get_reg_name(r);
            }
        } else {
            cout << CLR::CYAN << get_op_name(op4);
            if (op4 == OP_LOAD) {
                uint8_t next = (addr + 1 < DHAD_MEM_SIZE) ? cpu.C.memory[addr + 1] : 0;
                if (reg < 8)
                    cout << " " << get_reg_name(reg) << ", " << (int)next;
                else
                    cout << " مح, " << (int)next;
            }
            else if (op4 == OP_CALL) {
                uint8_t lo = cpu.C.memory[addr + 1];
                uint8_t hi = cpu.C.memory[addr + 2];
                uint16_t tgt = (hi << 8) | lo;
                cout << " 0x" << hex << setw(4) << setfill('0') << tgt << dec;
            }
            else if (reg < 8 && op4 != OP_HALT && op4 != OP_PRINT && op4 != OP_PUSH && op4 != OP_POP && op4 != OP_RET)
                cout << " " << get_reg_name(reg);
        }
        cout << CLR::RESET;
    }
}

/* ═══════════════════════════════════════════════════════════════════════════════
 *  قسم: الإخراج
 * ═══════════════════════════════════════════════════════════════════════════════ */

static void draw_output(const LiveCPU& cpu, int x, int y) {
    draw_box(x, y, 90, 5, "═══ الإخراج ═══", CLR::GREEN);

    gotoxy(x + 2, y + 2);
    cout << CLR::DIM << "stdout: " << CLR::RESET;

    if (!cpu.output_buf.empty()) {
        cout << CLR::WHITE << CLR::BOLD;
        int max_w = 80;
        int len = min((int)cpu.output_buf.size(), max_w);
        for (int i = 0; i < len; i++) {
            char c = cpu.output_buf[i];
            if (c >= 32 && c < 127) cout << c;
            else cout << CLR::DIM << "[" << hex << (int)c << dec << "]" << CLR::WHITE;
        }
        cout << CLR::RESET;
    } else {
        cout << CLR::DIM << "(لا يوجد إخراج)" << CLR::RESET;
    }
}

/* ═══════════════════════════════════════════════════════════════════════════════
 *  قسم: الأداء
 * ═══════════════════════════════════════════════════════════════════════════════ */

static void draw_perf(const LiveCPU& cpu, double cps, int x, int y) {
    draw_box(x, y, 90, 5, "═══ الأداء ═══", CLR::YELLOW);

    gotoxy(x + 2, y + 2);
    cout << CLR::BOLD << "الدورات/ثانية: " << CLR::WHITE << fixed << setprecision(0) << cps << CLR::RESET;

    gotoxy(x + 2, y + 3);
    cout << CLR::BOLD << "حجم البرنامج: " << CLR::WHITE << cpu.C.prog_size << " بايت"
         << CLR::RESET << "    "
         << CLR::BOLD << "تعليمات: " << CLR::WHITE << cpu.inst_count << CLR::RESET;
}

/* ═══════════════════════════════════════════════════════════════════════════════
 *  شريط المعلومات السفلي
 * ═══════════════════════════════════════════════════════════════════════════════ */

static void draw_status_bar(const LiveCPU& cpu, double cps, int x, int y) {
    gotoxy(x, y);
    cout << CLR::BG_BLUE << CLR::WHITE << CLR::BOLD;
    cout << " ض (Dhad v2.0.0 — 8-bit) │ "
         << "PC: 0x" << hex << setw(4) << setfill('0') << cpu.C.pc << dec << " │ "
         << "SP: " << (int)cpu.C.sp << " │ "
         << "D: " << cpu.C.cycles << " │ "
         << "Z:" << ((cpu.C.flags & 0x01) ? "1" : "0") << " "
         << "N:" << ((cpu.C.flags & 0x02) ? "1" : "0") << " "
         << "C:" << ((cpu.C.flags & 0x04) ? "1" : "0") << " │ "
         << setprecision(0) << fixed << cps << " د/ث"
         << CLR::RESET;
}

/* ═══════════════════════════════════════════════════════════════════════════════
 *  تنفيذ تعليمة واحدة
 * ═══════════════════════════════════════════════════════════════════════════════ */

static void step(LiveCPU& cpu) {
    if (cpu.C.halted) return;

    /* Track instruction for display before execution */
    uint8_t raw = cpu.C.memory[cpu.C.pc];
    cpu.last_op = raw;
    uint8_t opc = (raw >> 4) & 0x0F;
    cpu.last_operand = raw & 0x0F;

    if (opc == 0xF) {
        cpu.last_ext = cpu.C.memory[cpu.C.pc + 1];
        if (cpu.last_ext <= 0x40) {
            cpu.last_addr = cpu.C.memory[cpu.C.pc + 2] | ((uint16_t)cpu.C.memory[cpu.C.pc + 3] << 8);
        }
    } else if (opc == 0x3 || opc == 0x4) {
        cpu.last_addr = cpu.C.memory[cpu.C.pc + 1];
    }

    /* Delegate to unified CPU core */
    DhadCPU* c = cpu.core();
    c->output_func = NULL;
    dhad_cpu_step(c);
    cpu.inst_count++;
    cpu.stack_top = (cpu.C.sp > 0) ? cpu.C.sp - 1 : -1;

    /* Collect any output */
    if (c->output_len > 0) {
        cpu.output_buf.append(c->output, c->output_len);
        c->output_len = 0;
        c->output[0] = '\0';
    }
}
static int load_bin(LiveCPU& cpu, const string& filename) {
    ifstream f(filename, ios::binary | ios::ate);
    if (!f.is_open()) return -1;

    streamsize sz = f.tellg();
    if (sz <= 0 || sz > 0xF000) return -1;

    f.seekg(0, ios::beg);
    f.read((char*)&cpu.C.memory[DHAD_PROG_START], sz);
    cpu.C.prog_size = (uint16_t)sz;
    cpu.C.pc = DHAD_PROG_START;
    return (int)sz;
}

/* ═══════════════════════════════════════════════════════════════════════════════
 *  العنوان
 * ═══════════════════════════════════════════════════════════════════════════════ */

static void draw_header() {
    gotoxy(2, 1);
    cout << CLR::BOLD << CLR::CYAN
         << "╔══════════════════════════════════════════════════════════════════════════════════════╗"
         << CLR::RESET;
    gotoxy(2, 2);
    cout << CLR::BOLD << CLR::CYAN
         << "║                    محاكي المعالج اللحظي — ض (Dhad Live Simulator)                  ║"
         << CLR::RESET;
    gotoxy(2, 3);
    cout << CLR::BOLD << CLR::CYAN
         << "╚══════════════════════════════════════════════════════════════════════════════════════╝"
         << CLR::RESET;
}

/* ═══════════════════════════════════════════════════════════════════════════════
 *  لوحة التحكم
 * ═══════════════════════════════════════════════════════════════════════════════ */

static void draw_controls(int x, int y) {
    gotoxy(x, y);
    cout << CLR::BG_BLACK << CLR::WHITE << CLR::BOLD;
    cout << " [Enter] خطوة │ [S] سريع │ [R] تشغيل │ [P] إيقاف │ [Q] خروج │ [H] إعادة │ [1-9] سرعة";
    cout << CLR::RESET;
}

/* ═══════════════════════════════════════════════════════════════════════════════
 *  الحلقة الرئيسية
 * ═══════════════════════════════════════════════════════════════════════════════ */

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "الاستخدام: " << argv[0] << " <ملف.bin>\n";
        cout << "  شغّل المجمّع أولاً: ./dhad_cpu program.ضasm --bin --out program.bin\n";
        return 1;
    }

    LiveCPU cpu = {};
    cpu.C.pc = DHAD_PROG_START;
    cpu.stack_top = -1;

    if (load_bin(cpu, argv[1]) < 0) {
        cerr << CLR::RED << "خطأ: لا يمكن تحميل '" << argv[1] << "'" << CLR::RESET << endl;
        return 1;
    }

    signal(SIGINT, SIG_IGN);

    /* أوضاع التشغيل */
    enum Mode { PAUSED, STEP, FAST, RUN };
    Mode mode = PAUSED;
    int speed = 1; /* 1-9 */
    bool need_redraw = true;

    auto start_time = chrono::steady_clock::now();
    uint32_t last_cycle = 0;
    double cps = 0;

    cls();
    draw_header();

    while (true) {
        auto now = chrono::steady_clock::now();
        double elapsed = chrono::duration<double>(now - start_time).count();
        if (elapsed > 0.5) {
            cps = (double)(cpu.C.cycles - last_cycle) / elapsed;
            last_cycle = cpu.C.cycles;
            start_time = now;
        }

        /* تحديث */
        if (mode == RUN && !cpu.C.halted) {
            int steps = speed;
            for (int i = 0; i < steps && !cpu.C.halted; i++) {
                step(cpu);
            }
            need_redraw = true;
            usleep(max(1, 1000 / speed));
        } else if (mode == FAST && !cpu.C.halted) {
            for (int i = 0; i < 100 && !cpu.C.halted; i++) {
                step(cpu);
            }
            need_redraw = true;
            usleep(10000);
        }

        /* رسم */
        if (need_redraw) {
            draw_registers(cpu, 2, 5);
            draw_flags(cpu, 27, 5);
            draw_state(cpu, 52, 5);
            draw_stack(cpu, 77, 5);
            draw_memory(cpu, 2, 18);
            draw_inst_tree(cpu, 43, 18);
            draw_output(cpu, 2, 37);
            draw_perf(cpu, cps, 2, 43);
            draw_status_bar(cpu, cps, 2, 49);
            draw_controls(2, 51);
            flush();
            need_redraw = false;
        }

        /* قراءة المدخلات */
        /* استخدام select للتحقق من وجود مدخلات بدون حظر */
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        struct timeval tv = {0, 50000}; /* 50ms */
        if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0) {
            int ch = getchar();
            if (ch == 'q' || ch == 'Q') break;
            if (ch == '\n' && mode == PAUSED) {
                step(cpu);
                need_redraw = true;
            }
            if (ch == 's' || ch == 'S') {
                mode = (mode == FAST) ? PAUSED : FAST;
                need_redraw = true;
            }
            if (ch == 'r' || ch == 'R') {
                mode = (mode == RUN) ? PAUSED : RUN;
                need_redraw = true;
            }
            if (ch == 'p' || ch == 'P') {
                mode = PAUSED;
                need_redraw = true;
            }
            if (ch == 'h' || ch == 'H') {
                /* إعادة تشغيل */
                cpu = LiveCPU();
                cpu.C.pc = DHAD_PROG_START;
                cpu.stack_top = -1;
                cpu.C.prog_size = 0;
                load_bin(cpu, argv[1]);
                mode = PAUSED;
                need_redraw = true;
            }
            if (ch >= '1' && ch <= '9') {
                speed = ch - '0';
                need_redraw = true;
            }
        }

        if (cpu.C.halted && mode != PAUSED) {
            mode = PAUSED;
            need_redraw = true;
        }
    }

    /* الشاشة النهائية */
    cls();
    draw_header();
    draw_registers(cpu, 2, 5);
    draw_flags(cpu, 27, 5);
    draw_state(cpu, 52, 5);
    draw_stack(cpu, 77, 5);
    draw_memory(cpu, 2, 18);
    draw_inst_tree(cpu, 43, 18);
    draw_output(cpu, 2, 37);
    draw_perf(cpu, cps, 2, 43);
    draw_status_bar(cpu, cps, 2, 49);

    gotoxy(2, 53);
    if (cpu.C.halted) {
        cout << CLR::GREEN << CLR::BOLD << " انتهى التنفيذ بعد " << cpu.C.cycles << " دورة" << CLR::RESET;
    } else {
        cout << CLR::RED << CLR::BOLD << " تم الإيقاف يدوياً" << CLR::RESET;
    }

    gotoxy(2, 55);
    cout << endl;
    return 0;
}
