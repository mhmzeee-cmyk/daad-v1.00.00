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

using namespace std;

/* ═══════════════════════════════════════════════════════════════════════════════
 *  ثوابت المعالج — من dhad_isa_constants.h
 * ═══════════════════════════════════════════════════════════════════════════════ */

constexpr int DHAD_DATA_WIDTH     = 8;
constexpr int DHAD_ADDR_WIDTH     = 16;
constexpr int DHAD_REG_COUNT      = 8;
constexpr int DHAD_STACK_SIZE     = 32;
constexpr int DHAD_MEM_SIZE       = (1 << DHAD_ADDR_WIDTH);
constexpr int DHAD_PROG_START     = 0x0000;
constexpr int DHAD_MEM_STACK_END  = 0x01F;
constexpr int DHAD_MAX_CYCLES     = 100000;
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

struct DhadCPU {
    /* السجلات */
    uint8_t  regs[DHAD_REG_COUNT] = {};
    uint8_t  acc    = 0;
    uint8_t  flags  = 0;

    /* عداد البرنامج */
    uint16_t pc     = DHAD_PROG_START;

    /* مؤشر المكدس */
    uint8_t  sp     = 0;

    /* الذاكرة */
    uint8_t  memory[DHAD_MEM_SIZE] = {};

    /* المكدس */
    uint16_t stack[DHAD_STACK_SIZE] = {};
    int      stack_top = -1;

    /* الحالة */
    bool     halted   = false;
    uint32_t cycle    = 0;
    uint32_t inst_count = 0;

    /* I/O */
    uint8_t  io_out  = 0;
    uint8_t  io_in   = 0;
    string   output  = "";

    /* المقاطعات */
    bool     int_enabled = false;
    uint8_t  int_mask    = 0xFF;
    uint8_t  int_pending = 0;
    uint16_t ivt_addr    = 0x0040;

    /* آخر تعليمة */
    uint8_t  last_op     = 0;
    uint8_t  last_ext    = 0;
    uint16_t last_addr   = 0;
    uint8_t  last_operand= 0;

    /* حجم البرنامج */
    uint16_t prog_size = 0;
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

static void draw_registers(const DhadCPU& cpu, int x, int y) {
    draw_box(x, y, 24, 12, "═══ السجلات ═══", CLR::CYAN);

    for (int i = 0; i < 8; i++) {
        gotoxy(x + 2, y + 2 + i);
        cout << get_reg_color(i) << CLR::BOLD << get_reg_name(i)
             << ": " << CLR::WHITE << "0x" << hex << (int)cpu.regs[i]
             << dec << " (" << (int)cpu.regs[i] << ")" << CLR::RESET;
    }

    gotoxy(x + 2, y + 10);
    cout << CLR::RED << CLR::BOLD << "مح"
         << ": " << CLR::WHITE << "0x" << hex << (int)cpu.acc
         << dec << " (" << (int)cpu.acc << ")" << CLR::RESET;
}

/* ═══════════════════════════════════════════════════════════════════════════════
 *  قسم: الأعلام
 * ═══════════════════════════════════════════════════════════════════════════════ */

static void draw_flags(const DhadCPU& cpu, int x, int y) {
    draw_box(x, y, 24, 6, "═══ الأعلام ═══", CLR::BLUE);

    struct { const char* name; uint8_t bit; const string& color; } flags[] = {
        {"Z(صفر) ", 0x01, CLR::GREEN},
        {"N(سالب)", 0x02, CLR::RED},
        {"C(حمل) ", 0x04, CLR::YELLOW},
        {"V(فيض) ", 0x08, CLR::MAGENTA}
    };

    for (int i = 0; i < 4; i++) {
        gotoxy(x + 2, y + 2 + i);
        bool set = (cpu.flags & flags[i].bit) != 0;
        cout << CLR::BOLD << flags[i].name << ": "
             << (set ? CLR::BG_GREEN : CLR::BG_RED)
             << (set ? " 1 " : " 0 ")
             << CLR::RESET;
    }
}

/* ═══════════════════════════════════════════════════════════════════════════════
 *  قسم: الحالة
 * ═══════════════════════════════════════════════════════════════════════════════ */

static void draw_state(const DhadCPU& cpu, int x, int y) {
    draw_box(x, y, 24, 7, "═══ الحالة ═══", CLR::YELLOW);

    gotoxy(x + 2, y + 2);
    cout << CLR::BOLD << "PC    : " << CLR::WHITE << "0x" << hex << setw(3) << setfill('0') << cpu.pc << dec << CLR::RESET;

    gotoxy(x + 2, y + 3);
    cout << CLR::BOLD << "SP    : " << CLR::WHITE << (int)cpu.sp << CLR::RESET;

    gotoxy(x + 2, y + 4);
    cout << CLR::BOLD << "الدورات: " << CLR::WHITE << cpu.cycle << CLR::RESET;

    gotoxy(x + 2, y + 5);
    cout << CLR::BOLD << "التعليمات: " << CLR::WHITE << cpu.inst_count << CLR::RESET;

    gotoxy(x + 2, y + 6);
    cout << CLR::BOLD << "الحالة: "
         << (cpu.halted ? CLR::RED : CLR::GREEN)
         << CLR::BOLD << (cpu.halted ? " متوقف " : "  يعمل  ")
         << CLR::RESET;
}

/* ═══════════════════════════════════════════════════════════════════════════════
 *  قسم: المكدس
 * ═══════════════════════════════════════════════════════════════════════════════ */

static void draw_stack(const DhadCPU& cpu, int x, int y) {
    draw_box(x, y, 24, 12, "═══ المكدس ═══", CLR::MAGENTA);

    gotoxy(x + 2, y + 2);
    cout << CLR::BOLD << "SP: " << CLR::WHITE << (int)cpu.sp << CLR::RESET;

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
                 << "0x" << hex << setw(4) << setfill('0') << cpu.stack[i] << dec << CLR::RESET;
        } else {
            cout << CLR::DIM << "[" << i << "]  ---" << CLR::RESET;
        }
    }
}

/* ═══════════════════════════════════════════════════════════════════════════════
 *  قسم: الذاكرة
 * ═══════════════════════════════════════════════════════════════════════════════ */

static void draw_memory(const DhadCPU& cpu, int x, int y) {
    draw_box(x, y, 40, 18, "═══ الذاكرة ═══", CLR::GREEN);

    gotoxy(x + 2, y + 2);
    cout << CLR::BOLD << " العنوان │ القيمة │ الحرف │ التعليمة" << CLR::RESET;
    draw_line(x + 1, y + 3, 38, CLR::DIM);

    uint16_t start = (cpu.pc >= 5) ? cpu.pc - 5 : DHAD_PROG_START;
    if (start < DHAD_PROG_START) start = DHAD_PROG_START;
    uint16_t end_addr = min((uint16_t)(start + 12), (uint16_t)DHAD_MEM_SIZE);

    for (int i = 0; i < 12 && (start + i) < end_addr; i++) {
        uint16_t addr = start + i;
        uint8_t val = cpu.memory[addr];
        bool is_current = (addr == cpu.pc);

        gotoxy(x + 2, y + 4 + i);

        if (is_current) cout << CLR::BG_GREEN << CLR::BLACK;
        cout << "0x" << hex << setw(4) << setfill('0') << addr << dec << "  │  "
             << CLR::WHITE << setw(2) << setfill('0') << hex << (int)val << dec << CLR::RESET << "  │  ";

        if (val >= 32 && val < 127)
            cout << CLR::CYAN << (char)val << CLR::RESET << "   │  ";
        else
            cout << CLR::DIM << "." << CLR::RESET << "   │  ";

        /* اسم التعليمة */
        if (addr == cpu.pc) {
            uint8_t opc = (val >> 4) & 0x0F;
            uint8_t r = val & 0x0F;
            if (opc == OP_EXT) {
                uint8_t ext_val = (addr + 1 < DHAD_MEM_SIZE) ? cpu.memory[addr + 1] : 0;
                cout << CLR::MAGENTA << CLR::BOLD << get_ext_name(ext_val & 0xF0) << CLR::RESET;
            } else if (opc == OP_LOAD) {
                uint8_t next = (addr + 1 < DHAD_MEM_SIZE) ? cpu.memory[addr + 1] : 0;
                cout << CLR::CYAN << CLR::BOLD << get_op_name(opc) << " " << (r < 8 ? get_reg_name(r) : "مح") << ", " << (int)next << CLR::RESET;
            } else if (opc == OP_CALL) {
                uint8_t lo = cpu.memory[addr + 1];
                uint8_t hi = cpu.memory[addr + 2];
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

static void draw_inst_tree(const DhadCPU& cpu, int x, int y) {
    draw_box(x, y, 40, 18, "═══ شجرة التعليمات ═══", CLR::CYAN);

    uint16_t start = (cpu.pc >= 5) ? cpu.pc - 5 : DHAD_PROG_START;
    if (start < DHAD_PROG_START) start = DHAD_PROG_START;

    for (int i = 0; i < 12; i++) {
        uint16_t addr = start + i;
        if (addr >= DHAD_MEM_SIZE) break;

        uint8_t val = cpu.memory[addr];
        uint8_t op4 = (val >> 4) & 0x0F;
        uint8_t reg = val & 0x0F;
        bool is_current = (addr == cpu.pc);

        gotoxy(x + 2, y + 2 + i);

        if (is_current) cout << CLR::GREEN << CLR::BOLD << "▶ " << CLR::RESET;
        else            cout << "  ";

        cout << CLR::DIM << "0x" << hex << setw(4) << setfill('0') << addr << dec << CLR::RESET << " ";
        cout << CLR::WHITE << setw(2) << setfill('0') << hex << (int)val << dec << CLR::RESET << " ";

        if (op4 == OP_EXT) {
            uint8_t ext_val = (addr + 1 < DHAD_MEM_SIZE) ? cpu.memory[addr + 1] : 0;
            uint8_t ext_op = ext_val & 0xF0;
            cout << CLR::MAGENTA << get_ext_name(ext_op);
            if (ext_op <= 0x40) {
                uint8_t lo = cpu.memory[addr + 1];
                uint8_t hi = cpu.memory[addr + 2];
                uint16_t tgt = (hi << 8) | lo;
                cout << " 0x" << hex << setw(4) << setfill('0') << tgt << dec;
            } else {
                uint8_t r = ext_val & 0x0F;
                if (r < 8) cout << " " << get_reg_name(r);
            }
        } else {
            cout << CLR::CYAN << get_op_name(op4);
            if (op4 == OP_LOAD) {
                uint8_t next = (addr + 1 < DHAD_MEM_SIZE) ? cpu.memory[addr + 1] : 0;
                if (reg < 8)
                    cout << " " << get_reg_name(reg) << ", " << (int)next;
                else
                    cout << " مح, " << (int)next;
            }
            else if (op4 == OP_CALL) {
                uint8_t lo = cpu.memory[addr + 1];
                uint8_t hi = cpu.memory[addr + 2];
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

static void draw_output(const DhadCPU& cpu, int x, int y) {
    draw_box(x, y, 90, 5, "═══ الإخراج ═══", CLR::GREEN);

    gotoxy(x + 2, y + 2);
    cout << CLR::DIM << "stdout: " << CLR::RESET;

    if (!cpu.output.empty()) {
        cout << CLR::WHITE << CLR::BOLD;
        int max_w = 80;
        int len = min((int)cpu.output.size(), max_w);
        for (int i = 0; i < len; i++) {
            char c = cpu.output[i];
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

static void draw_perf(const DhadCPU& cpu, double cps, int x, int y) {
    draw_box(x, y, 90, 5, "═══ الأداء ═══", CLR::YELLOW);

    gotoxy(x + 2, y + 2);
    cout << CLR::BOLD << "الدورات/ثانية: " << CLR::WHITE << fixed << setprecision(0) << cps << CLR::RESET;

    gotoxy(x + 2, y + 3);
    cout << CLR::BOLD << "حجم البرنامج: " << CLR::WHITE << cpu.prog_size << " بايت"
         << CLR::RESET << "    "
         << CLR::BOLD << "تعليمات: " << CLR::WHITE << cpu.inst_count << CLR::RESET;
}

/* ═══════════════════════════════════════════════════════════════════════════════
 *  شريط المعلومات السفلي
 * ═══════════════════════════════════════════════════════════════════════════════ */

static void draw_status_bar(const DhadCPU& cpu, double cps, int x, int y) {
    gotoxy(x, y);
    cout << CLR::BG_BLUE << CLR::WHITE << CLR::BOLD;
    cout << " ض (Dhad v2.0.0 — 8-bit) │ "
         << "PC: 0x" << hex << setw(4) << setfill('0') << cpu.pc << dec << " │ "
         << "SP: " << (int)cpu.sp << " │ "
         << "D: " << cpu.cycle << " │ "
         << "Z:" << ((cpu.flags & 0x01) ? "1" : "0") << " "
         << "N:" << ((cpu.flags & 0x02) ? "1" : "0") << " "
         << "C:" << ((cpu.flags & 0x04) ? "1" : "0") << " │ "
         << setprecision(0) << fixed << cps << " د/ث"
         << CLR::RESET;
}

/* ═══════════════════════════════════════════════════════════════════════════════
 *  تنفيذ تعليمة واحدة
 * ═══════════════════════════════════════════════════════════════════════════════ */

static void step(DhadCPU& cpu) {
    if (cpu.halted) return;

    if (cpu.int_enabled) {
        uint8_t pending = cpu.int_pending & cpu.int_mask;
        if (pending) {
            for (int i = 0; i < 8; i++) {
                if (pending & (1 << i)) {
                    cpu.int_pending &= ~(1 << i);
                    cpu.stack[++cpu.sp] = cpu.flags;
                    cpu.stack[++cpu.sp] = cpu.pc;
                    cpu.int_enabled = false;
                    cpu.pc = cpu.ivt_addr + (i * 4);
                    cpu.cycle += 3;
                    return;
                }
            }
        }
    }

    uint8_t raw = cpu.memory[cpu.pc++];
    uint8_t opc = (raw >> 4) & 0x0F;
    uint8_t reg = raw & 0x0F;

    cpu.last_op = raw;
    cpu.last_operand = reg;
    cpu.cycle++;
    cpu.inst_count++;

    switch (opc) {
        case 0x0: break; /* سكون */

        case 0x1: { /* حمّل */
            uint8_t val = cpu.memory[cpu.pc++];
            if (reg < 8) cpu.regs[reg] = val; else cpu.acc = val;
            break;
        }

        case 0x2: { /* نقل */
            uint8_t dst = cpu.memory[cpu.pc++];
            cpu.regs[dst & 0x07] = cpu.regs[reg & 0x07];
            break;
        }

        case 0x3: { /* اقرأ */
            uint8_t addr = cpu.memory[cpu.pc++];
            cpu.last_addr = addr;
            uint8_t v = cpu.memory[addr];
            if (reg < 8) cpu.regs[reg] = v; else cpu.acc = v;
            break;
        }

        case 0x4: { /* خزن */
            uint8_t addr = cpu.memory[cpu.pc++];
            cpu.last_addr = addr;
            cpu.memory[addr] = (reg < 8) ? cpu.regs[reg] : cpu.acc;
            break;
        }

        case 0x5: /* ادفع */
            if (cpu.sp < DHAD_STACK_SIZE) {
                cpu.stack[cpu.sp] = cpu.acc;
                cpu.stack_top = cpu.sp;
                cpu.sp++;
            }
            break;

        case 0x6: /* اسحب */
            if (cpu.sp > 0) {
                cpu.sp--;
                cpu.acc = (uint8_t)(cpu.stack[cpu.sp] & 0xFF);
                cpu.stack_top = cpu.sp - 1;
            }
            break;

        case 0x7: { /* نداء */
            uint8_t lo = cpu.memory[cpu.pc++];
            uint8_t hi = cpu.memory[cpu.pc++];
            uint16_t addr = (hi << 8) | lo;
            cpu.last_addr = addr;
            if (cpu.sp < DHAD_STACK_SIZE) {
                cpu.stack[cpu.sp] = cpu.pc;
                cpu.stack_top = cpu.sp;
                cpu.sp++;
                cpu.pc = addr;
            }
            break;
        }

        case 0x8: /* إرجاع / تبريد */
            if (reg == 0) {
                if (cpu.sp > 0) {
                    cpu.sp--;
                    cpu.pc = cpu.stack[cpu.sp];
                    cpu.stack_top = cpu.sp - 1;
                }
            } else {
                cpu.acc = (uint8_t)(-(int8_t)cpu.acc);
                cpu.flags = 0;
                if (cpu.acc == 0) cpu.flags |= 0x01;
                if (cpu.acc & 0x80) cpu.flags |= 0x02;
            }
            break;

        case 0x9: { /* جمع */
            uint8_t v = cpu.regs[reg & 7];
            uint16_t sum = (uint16_t)cpu.acc + (uint16_t)v;
            cpu.acc = (uint8_t)(sum & 0xFF);
            cpu.flags = 0;
            if (cpu.acc == 0) cpu.flags |= 0x01;
            if (cpu.acc & 0x80) cpu.flags |= 0x02;
            if (sum > 0xFF) cpu.flags |= 0x04;
            break;
        }

        case 0xA: { /* طرح */
            uint8_t v = cpu.regs[reg & 7];
            int diff = (int)cpu.acc - (int)v;
            cpu.flags = 0;
            cpu.acc = (uint8_t)(diff & 0xFF);
            if (cpu.acc == 0) cpu.flags |= 0x01;
            if (cpu.acc & 0x80) cpu.flags |= 0x02;
            if (diff < 0) cpu.flags |= 0x04;
            break;
        }

        case 0xB: { /* ضرب */
            uint8_t v = cpu.regs[reg & 7];
            uint16_t prod = (uint16_t)cpu.acc * (uint16_t)v;
            cpu.acc = (uint8_t)(prod & 0xFF);
            cpu.flags = 0;
            if (cpu.acc == 0) cpu.flags |= 0x01;
            if (cpu.acc & 0x80) cpu.flags |= 0x02;
            break;
        }

        case 0xC: { /* قسمة */
            uint8_t v = cpu.regs[reg & 7];
            if (v) {
                cpu.acc = cpu.acc / v;
                cpu.flags = 0;
                if (cpu.acc == 0) cpu.flags |= 0x01;
                if (cpu.acc & 0x80) cpu.flags |= 0x02;
            }
            break;
        }

        case 0xD: { /* باقي */
            uint8_t v = cpu.regs[reg & 7];
            if (v) {
                cpu.acc = cpu.acc % v;
                cpu.flags = 0;
                if (cpu.acc == 0) cpu.flags |= 0x01;
                if (cpu.acc & 0x80) cpu.flags |= 0x02;
            }
            break;
        }

        case 0xE: /* اطبع */
            cpu.output += to_string((int)cpu.acc);
            break;

        case 0xF: { /* موسعة */
            uint8_t ex = cpu.memory[cpu.pc++];
            cpu.last_ext = ex;

            if (ex <= 0x40) {
                /* قفزات: عنوان 16-bit */
                uint8_t lo = cpu.memory[cpu.pc++];
                uint8_t hi = cpu.memory[cpu.pc++];
                uint16_t addr = (hi << 8) | lo;
                cpu.last_addr = addr;
                bool jump = false;
                switch (ex) {
                    case EXT_JMP: jump = true; break;
                    case EXT_JZ:  jump = (cpu.flags & 0x01); break;
                    case EXT_JNZ: jump = !(cpu.flags & 0x01); break;
                    case EXT_JC:  jump = (cpu.flags & 0x04); break;
                    case EXT_JN:  jump = (cpu.flags & 0x02); break;
                }
                if (jump) cpu.pc = addr;
            } else if (ex == EXT_PRINT_CH) {
                cpu.output += (char)cpu.acc;
            } else if (ex == EXT_INPUT) {
                cpu.acc = (uint8_t)(rand() % 256);
            } else if (ex == 0xD0) {
                uint8_t op2 = cpu.memory[cpu.pc++];
                uint8_t dst = (op2 >> 4) & 0x0F;
                uint8_t src = op2 & 0x0F;
                uint8_t *pd = (dst < 8) ? &cpu.regs[dst] : &cpu.acc;
                uint8_t *ps = (src < 8) ? &cpu.regs[src] : &cpu.acc;
                uint8_t tmp = *pd; *pd = *ps; *ps = tmp;
            } else if (ex == 0xD1) {
                uint8_t r = cpu.memory[cpu.pc++] & 0x07;
                cpu.regs[r]--;
                cpu.flags = 0;
                if (cpu.regs[r] == 0) cpu.flags |= 0x01;
                if (cpu.regs[r] & 0x80) cpu.flags |= 0x02;
            } else if (ex == 0xD2) {
                uint8_t r = cpu.memory[cpu.pc++] & 0x07;
                cpu.regs[r]++;
                cpu.flags = 0;
                if (cpu.regs[r] == 0) cpu.flags |= 0x01;
                if (cpu.regs[r] & 0x80) cpu.flags |= 0x02;
            } else if (ex == 0xD3) {
                cpu.halted = true;
            } else if ((ex & 0xF0) == 0xE0) {
                uint8_t v = cpu.regs[ex & 0x07];
                uint16_t r = (uint16_t)cpu.acc - (uint16_t)v;
                cpu.flags = 0;
                if ((r & 0xFF) == 0) cpu.flags |= 0x01;
                if (r & 0x80) cpu.flags |= 0x02;
                if (r > 0xFF) cpu.flags |= 0x04;
            } else if (ex == 0xE1) {
                cpu.int_enabled = true;
            } else if (ex == 0xE2) {
                cpu.int_enabled = false;
            } else if (ex == 0xE3) {
                uint16_t ret_addr = cpu.stack[--cpu.sp];
                uint8_t ret_flags = (uint8_t)(cpu.stack[--cpu.sp] & 0xFF);
                cpu.pc = ret_addr;
                cpu.flags = ret_flags;
                cpu.int_enabled = true;
            } else {
                uint8_t rg = reg & 7;
                switch (ex) {
                    case EXT_XOR: cpu.acc ^= cpu.regs[rg]; break;
                    case EXT_OR:  cpu.acc |= cpu.regs[rg]; break;
                    case EXT_AND: cpu.acc &= cpu.regs[rg]; break;
                    case EXT_NOT: cpu.acc = ~cpu.acc; break;
                    case EXT_SHL: cpu.acc <<= 1; break;
                    case EXT_SHR: cpu.acc >>= 1; break;
                }
                cpu.flags = 0;
                if (cpu.acc == 0) cpu.flags |= 0x01;
                if (cpu.acc & 0x80) cpu.flags |= 0x02;
            }
            break;
        }
    }
}

/* ═══════════════════════════════════════════════════════════════════════════════
 *  تحميل البرنامج
 * ═══════════════════════════════════════════════════════════════════════════════ */

static int load_bin(DhadCPU& cpu, const string& filename) {
    ifstream f(filename, ios::binary | ios::ate);
    if (!f.is_open()) return -1;

    streamsize sz = f.tellg();
    if (sz <= 0 || sz > 0xF000) return -1;

    f.seekg(0, ios::beg);
    f.read((char*)&cpu.memory[DHAD_PROG_START], sz);
    cpu.prog_size = (uint16_t)sz;
    cpu.pc = DHAD_PROG_START;
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

    DhadCPU cpu = {};
    cpu.pc = DHAD_PROG_START;
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
            cps = (double)(cpu.cycle - last_cycle) / elapsed;
            last_cycle = cpu.cycle;
            start_time = now;
        }

        /* تحديث */
        if (mode == RUN && !cpu.halted) {
            int steps = speed;
            for (int i = 0; i < steps && !cpu.halted; i++) {
                step(cpu);
            }
            need_redraw = true;
            usleep(max(1, 1000 / speed));
        } else if (mode == FAST && !cpu.halted) {
            for (int i = 0; i < 100 && !cpu.halted; i++) {
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
                cpu = DhadCPU();
                cpu.pc = DHAD_PROG_START;
                cpu.stack_top = -1;
                cpu.prog_size = 0;
                load_bin(cpu, argv[1]);
                mode = PAUSED;
                need_redraw = true;
            }
            if (ch >= '1' && ch <= '9') {
                speed = ch - '0';
                need_redraw = true;
            }
        }

        if (cpu.halted && mode != PAUSED) {
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
    if (cpu.halted) {
        cout << CLR::GREEN << CLR::BOLD << "✓ انتهى التنفيذ بعد " << cpu.cycle << " دورة" << CLR::RESET;
    } else {
        cout << CLR::RED << CLR::BOLD << "✗ تم الإيقاف يدوياً" << CLR::RESET;
    }

    gotoxy(2, 55);
    cout << endl;
    return 0;
}
