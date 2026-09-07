#include "dhad_isa.h"
#include <stdio.h>
#include <string.h>

/* ═══════════════════════════════════════════════════
   محاكي معالج ض — المعالج الرئيسي
   ═══════════════════════════════════════════════════ */

/* ── أسماء السجلات بالعربي ── */
static const char *reg_names[] = {
    "س0", "س1", "س2", "س3",
    "س4", "س5", "س6", "س7",
    "مح", "أعلام"
};

static const char *op_names[] = {
    [DHAD_OP_NOP]      = "سكون",
    [DHAD_OP_HALT]     = "توقف",
    [DHAD_OP_MOV_IMM]  = "حمّل",
    [DHAD_OP_MOV_REG]  = "نقل",
    [DHAD_OP_LOAD]     = "اقرأ_ذاكرة",
    [DHAD_OP_STORE]    = "خزن_ذاكرة",
    [DHAD_OP_PUSH]     = "ادفع",
    [DHAD_OP_POP]      = "اسحب",
    [DHAD_OP_CALL]     = "نداء",
    [DHAD_OP_RET]      = "إرجاع",
    [DHAD_OP_ADD]      = "جمع",
    [DHAD_OP_SUB]      = "طرح",
    [DHAD_OP_MUL]      = "ضرب",
    [DHAD_OP_DIV]      = "قسمة",
    [DHAD_OP_MOD]      = "باقي",
    [DHAD_OP_AND]      = "ومنطقي",
    [DHAD_OP_OR]       = "أومنطقي",
    [DHAD_OP_JMP]      = "قفز",
    [DHAD_OP_JZ]       = "قفز_إذاصفر",
    [DHAD_OP_JNZ]      = "قفز_إذا_غيرصفر",
    [DHAD_OP_JC]       = "قفز_إذا_حمل",
    [DHAD_OP_JN]       = "قفز_إذا_سالب",
    [DHAD_OP_XOR]      = "أوحصري",
    [DHAD_OP_NOT]      = "ليس",
    [DHAD_OP_SHL]      = "shift_left",
    [DHAD_OP_SHR]      = "shift_right",
    [DHAD_OP_NEG]      = "تبريد",
    [DHAD_OP_CMP]      = "قارن",
    [DHAD_OP_CMP_IMM]  = "قارن_فوري",
    [DHAD_OP_PRINT]    = "اطبع",
    [DHAD_OP_INPUT]    = "ادخل",
    [DHAD_OP_PRINT_CH] = "اطبع_حرف",
};

const char* dhad_reg_name(uint8_t reg_id) {
    if (reg_id <= DHAD_REG_F) return reg_names[reg_id];
    return "؟";
}

const char* dhad_op_name(uint8_t opcode) {
    if (opcode <= DHAD_OP_PRINT_CH) return op_names[opcode];
    return "؟";
}

/* ── تحديث الأعلام ── */
void dhad_update_flags(DhadCPU *cpu, uint8_t result) {
    cpu->flags = 0;
    if (result == 0)
        cpu->flags |= DHAD_FLAG_Z;
    if (result & 0x08)  /* bit 3 = sign bit for 4-bit */
        cpu->flags |= DHAD_FLAG_N;
}

/* ── جلب تعليمة من الذاكرة ── */
uint8_t dhad_fetch_byte(DhadCPU *cpu) {
    uint8_t byte = cpu->memory[cpu->pc & 0xFFF];
    cpu->pc = (cpu->pc + 1) & 0xFFF;
    cpu->last_fetch = byte;
    return byte;
}

uint16_t dhad_fetch_word(DhadCPU *cpu) {
    uint8_t lo = dhad_fetch_byte(cpu);
    uint8_t hi = dhad_fetch_byte(cpu);
    return (uint16_t)((hi << 8) | lo);
}

/* ── المكدس ── */
void dhad_push_stack(DhadCPU *cpu, uint16_t value) {
    if (cpu->sp < DHAD_STACK_SIZE - 1) {
        cpu->stack[cpu->sp++] = value;
    }
}

uint16_t dhad_pop_stack(DhadCPU *cpu) {
    if (cpu->sp > 0) {
        return cpu->stack[--cpu->sp];
    }
    return 0;
}

/* ── تهيئة المعالج ── */
void dhad_cpu_init(DhadCPU *cpu) {
    memset(cpu, 0, sizeof(DhadCPU));
    cpu->pc = 0x800;  /* بداية ROM */
    cpu->sp = 0;
    cpu->halted = false;
    cpu->running = false;
    cpu->cycle_count = 0;
}

/* ── إعادة تعيين ── */
void dhad_cpu_reset(DhadCPU *cpu) {
    uint16_t pc = cpu->pc;
    memset(cpu->regs, 0, sizeof(cpu->regs));
    cpu->acc = 0;
    cpu->flags = 0;
    cpu->pc = 0x800;
    cpu->sp = 0;
    cpu->halted = false;
    cpu->cycle_count = 0;
    (void)pc;
}

/* ── تحميل برنامج ── */
int dhad_cpu_load_program(DhadCPU *cpu, const uint8_t *program, uint16_t size) {
    if (size > 0x700) {  /* الحد الأقصى لحجم البرنامج */
        fprintf(stderr, "خطأ: البرنامج أكبر من الذاكرة المتاحة (%d > %d)\n",
                size, 0x700);
        return -1;
    }
    memcpy(&cpu->memory[0x800], program, size);
    cpu->pc = 0x800;
    return 0;
}

/* ── تنفيذ تعليمة واحدة ── */
void dhad_cpu_step(DhadCPU *cpu) {
    if (cpu->halted) return;

    uint8_t raw = dhad_fetch_byte(cpu);
    cpu->last_opcode = raw;
    cpu->cycle_count++;

    uint8_t opcode = (raw >> 4) & 0x0F;
    uint8_t operand = raw & 0x0F;

    switch (opcode) {

    /* ═══════════════ تعليمات النظام ═══════════════ */

    case DHAD_OP_NOP:
        /* سكون: لا تفعل شيئاً */
        break;

    case DHAD_OP_HALT:
        /* توقف */
        cpu->halted = true;
        cpu->running = false;
        break;

    /* ═══════════════ تعليمات النقل ═══════════════ */

    case DHAD_OP_MOV_IMM: {
        /* حمّل سج, قيمة (4-bit immediate)
         * التعليمة التالية تحتوي القيمة الكاملة (8-bit)
         */
        uint8_t reg_id = operand;
        uint8_t imm_val = dhad_fetch_byte(cpu);
        if (reg_id <= 7) {
            cpu->regs[reg_id] = imm_val & 0x0F;
        } else if (reg_id == DHAD_REG_A) {
            cpu->acc = imm_val & 0x0F;
        }
        break;
    }

    case DHAD_OP_MOV_REG: {
        /* نقل سج2, سج1
         * operand = (dst << 2) | src (4 bit)
         */
        uint8_t dst = (operand >> 2) & 0x03;
        uint8_t src = operand & 0x03;
        uint8_t val = 0;
        if (src <= 7) val = cpu->regs[src];
        else if (src == 8) val = cpu->acc;
        if (dst <= 7) cpu->regs[dst] = val;
        else if (dst == 8) cpu->acc = val;
        break;
    }

    case DHAD_OP_LOAD: {
        /* اقرأ من الذاكرة: سج = الذاكرة[عنوان]
         * operand = reg_id, التعليمة التالية = العنوان (8-bit)
         */
        uint8_t reg_id = operand;
        uint8_t addr = dhad_fetch_byte(cpu);
        uint8_t val = cpu->memory[addr & 0x0FFF];
        if (reg_id <= 7) cpu->regs[reg_id] = val;
        else if (reg_id == 8) cpu->acc = val;
        break;
    }

    case DHAD_OP_STORE: {
        /* اكتب للذاكرة: الذاكرة[عنوان] = سج
         * operand = reg_id, التعليمة التالية = العنوان (8-bit)
         */
        uint8_t reg_id = operand;
        uint8_t addr = dhad_fetch_byte(cpu);
        uint8_t val = 0;
        if (reg_id <= 7) val = cpu->regs[reg_id];
        else if (reg_id == 8) val = cpu->acc;
        cpu->memory[addr & 0x0FFF] = val;
        break;
    }

    case DHAD_OP_PUSH:
        /* ادفع المح للمكدس */
        dhad_push_stack(cpu, cpu->acc);
        break;

    case DHAD_OP_POP:
        /* اسحب من المكدس للمح */
        cpu->acc = (uint8_t)(dhad_pop_stack(cpu) & 0x0F);
        break;

    case DHAD_OP_CALL: {
        /* نداء: push PC+1, jump للعنوان (8-bit)
         */
        uint16_t addr = dhad_fetch_byte(cpu);
        dhad_push_stack(cpu, cpu->pc);
        cpu->pc = 0x800 + addr;
        break;
    }

    case DHAD_OP_RET:
        /* ارجاع: pop PC */
        cpu->pc = dhad_pop_stack(cpu);
        break;

    /* ═══════════════ التعليمات الحسابية ═══════════════ */

    case DHAD_OP_ADD: {
        /* جمع: مح = مح + سج
         * operand = reg_id
         */
        uint8_t val = (operand <= 7) ? cpu->regs[operand] : cpu->acc;
        uint16_t result = (uint16_t)(cpu->acc + val);
        cpu->acc = (uint8_t)(result & 0x0F);
        cpu->flags = 0;
        if (cpu->acc == 0) cpu->flags |= DHAD_FLAG_Z;
        if (result > 0x0F) cpu->flags |= DHAD_FLAG_C;
        break;
    }

    case DHAD_OP_SUB: {
        /* طرح: مح = مح - سج */
        uint8_t val = (operand <= 7) ? cpu->regs[operand] : cpu->acc;
        int16_t result = (int16_t)(cpu->acc - val);
        cpu->acc = (uint8_t)(result & 0x0F);
        cpu->flags = 0;
        if (cpu->acc == 0) cpu->flags |= DHAD_FLAG_Z;
        if (result < 0) cpu->flags |= DHAD_FLAG_N;
        break;
    }

    case DHAD_OP_MUL: {
        /* ضرب: مح = مح × سج */
        uint8_t val = (operand <= 7) ? cpu->regs[operand] : cpu->acc;
        uint16_t result = (uint16_t)(cpu->acc * val);
        cpu->acc = (uint8_t)(result & 0x0F);
        dhad_update_flags(cpu, cpu->acc);
        if (result > 0x0F) cpu->flags |= DHAD_FLAG_C;
        break;
    }

    case DHAD_OP_DIV: {
        /* قسمة: مح = مح ÷ سج */
        uint8_t val = (operand <= 7) ? cpu->regs[operand] : cpu->acc;
        if (val == 0) {
            cpu->halted = true;
            break;
        }
        cpu->acc = (uint8_t)((cpu->acc / val) & 0x0F);
        dhad_update_flags(cpu, cpu->acc);
        break;
    }

    case DHAD_OP_MOD: {
        /* باقي: مح = مح % سج */
        uint8_t val = (operand <= 7) ? cpu->regs[operand] : cpu->acc;
        if (val == 0) {
            cpu->halted = true;
            break;
        }
        cpu->acc = (uint8_t)((cpu->acc % val) & 0x0F);
        dhad_update_flags(cpu, cpu->acc);
        break;
    }

    /* ═══════════════ تعليمات المنطق ═══════════════ */

    case DHAD_OP_AND: {
        uint8_t val = (operand <= 7) ? cpu->regs[operand] : cpu->acc;
        cpu->acc = (uint8_t)((cpu->acc & val) & 0x0F);
        dhad_update_flags(cpu, cpu->acc);
        break;
    }

    case DHAD_OP_OR: {
        uint8_t val = (operand <= 7) ? cpu->regs[operand] : cpu->acc;
        cpu->acc = (uint8_t)((cpu->acc | val) & 0x0F);
        dhad_update_flags(cpu, cpu->acc);
        break;
    }

    case DHAD_OP_XOR: {
        uint8_t val = (operand <= 7) ? cpu->regs[operand] : cpu->acc;
        cpu->acc = (uint8_t)((cpu->acc ^ val) & 0x0F);
        dhad_update_flags(cpu, cpu->acc);
        break;
    }

    case DHAD_OP_NOT:
        cpu->acc = (uint8_t)((~cpu->acc) & 0x0F);
        dhad_update_flags(cpu, cpu->acc);
        break;

    case DHAD_OP_SHL:
        cpu->acc = (uint8_t)((cpu->acc << 1) & 0x0F);
        dhad_update_flags(cpu, cpu->acc);
        break;

    case DHAD_OP_SHR:
        cpu->acc = (uint8_t)((cpu->acc >> 1) & 0x0F);
        dhad_update_flags(cpu, cpu->acc);
        break;

    case DHAD_OP_NEG:
        cpu->acc = (uint8_t)((-cpu->acc) & 0x0F);
        dhad_update_flags(cpu, cpu->acc);
        break;

    /* ═══════════════ تعليمات القفز ═══════════════ */

    case DHAD_OP_JMP: {
        uint8_t addr = dhad_fetch_byte(cpu);
        cpu->pc = 0x800 + addr;
        break;
    }

    case DHAD_OP_JZ: {
        uint8_t addr = dhad_fetch_byte(cpu);
        if (cpu->flags & DHAD_FLAG_Z)
            cpu->pc = 0x800 + addr;
        break;
    }

    case DHAD_OP_JNZ: {
        uint8_t addr = dhad_fetch_byte(cpu);
        if (!(cpu->flags & DHAD_FLAG_Z))
            cpu->pc = 0x800 + addr;
        break;
    }

    case DHAD_OP_JC: {
        uint8_t addr = dhad_fetch_byte(cpu);
        if (cpu->flags & DHAD_FLAG_C)
            cpu->pc = 0x800 + addr;
        break;
    }

    case DHAD_OP_JN: {
        uint8_t addr = dhad_fetch_byte(cpu);
        if (cpu->flags & DHAD_FLAG_N)
            cpu->pc = 0x800 + addr;
        break;
    }

    /* ═══════════════ تعليمات المقارنة ═══════════════ */

    case DHAD_OP_CMP: {
        uint8_t val = (operand <= 7) ? cpu->regs[operand] : cpu->acc;
        int16_t result = (int16_t)(cpu->acc - val);
        cpu->flags = 0;
        if ((result & 0x0F) == 0) cpu->flags |= DHAD_FLAG_Z;
        if (result < 0) cpu->flags |= DHAD_FLAG_N;
        break;
    }

    case DHAD_OP_CMP_IMM: {
        uint8_t imm = dhad_fetch_byte(cpu);
        int16_t result = (int16_t)(cpu->acc - (imm & 0x0F));
        cpu->flags = 0;
        if ((result & 0x0F) == 0) cpu->flags |= DHAD_FLAG_Z;
        if (result < 0) cpu->flags |= DHAD_FLAG_N;
        break;
    }

    /* ═══════════════ تعليمات I/O ═══════════════ */

    case DHAD_OP_PRINT:
        /* طباعة القيمة الرقمية للمح */
        printf("%d", cpu->acc);
        break;

    case DHAD_OP_PRINT_CH:
        /* طباعة المح كحرف ASCII */
        printf("%c", 'A' + (cpu->acc & 0x0F));
        break;

    case DHAD_OP_INPUT: {
        /* إدخال رقم من المستخدم */
        int val = 0;
        scanf("%d", &val);
        cpu->acc = (uint8_t)(val & 0x0F);
        break;
    }

    default:
        /* تعليمة غير معروفة: توقف */
        fprintf(stderr, "خطأ: تعليمة غير معروفة: 0x%02X عند العنوان 0x%03X\n",
                raw, cpu->pc - 1);
        cpu->halted = true;
        break;
    }
}

/* ── تشغيل حتى التوقف ── */
void dhad_cpu_run(DhadCPU *cpu, uint32_t max_cycles) {
    cpu->running = true;
    while (cpu->running && !cpu->halted && cpu->cycle_count < max_cycles) {
        dhad_cpu_step(cpu);
    }
}

/* ── طباعة حالة المعالج ── */
void dhad_cpu_print_state(const DhadCPU *cpu) {
    printf("═══════════════════════════════════════\n");
    printf("  حالة معالج ض\n");
    printf("═══════════════════════════════════════\n");
    printf(" عداد البرنامج (PC): 0x%03X\n", cpu->pc);
    printf(" مؤشر المكدس (SP):   %d\n", cpu->sp);
    printf(" عدد الدورات:        %u\n", cpu->cycle_count);
    printf("───────────────────────────────────────\n");
    printf(" السجلات:\n");
    for (int i = 0; i < DHAD_REG_COUNT; i++) {
        printf("   %s = %d (0x%X)\n", reg_names[i], cpu->regs[i], cpu->regs[i]);
    }
    printf("   %s = %d (0x%X)\n", "المح", cpu->acc, cpu->acc);
    printf("───────────────────────────────────────\n");
    printf(" الأعلام: Z=%d N=%d C=%d V=%d\n",
           (cpu->flags & DHAD_FLAG_Z) ? 1 : 0,
           (cpu->flags & DHAD_FLAG_N) ? 1 : 0,
           (cpu->flags & DHAD_FLAG_C) ? 1 : 0,
           (cpu->flags & DHAD_FLAG_V) ? 1 : 0);
    printf(" الحالة: %s\n", cpu->halted ? "متوقف" : "يعمل");
    printf("═══════════════════════════════════════\n");
}
