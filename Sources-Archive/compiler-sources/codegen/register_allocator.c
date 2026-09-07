/**
 * @file register_allocator.c
 * @brief تنفيذ مخصص السجلات - Linear Scan Register Allocator
 *
 * Algorithm:
 * 1. Sort intervals by start point
 * 2. Process intervals in order
 * 3. Expire old intervals
 * 4. Try to allocate a free register
 * 5. If no free register, spill the longest interval
 */

#include "register_allocator.h"
#include <stdlib.h>
#include <string.h>

static const PhysicalRegister int_alloc_order[] = {
    REG_RDI, REG_RSI, REG_RDX, REG_RCX, REG_R8, REG_R9,
    REG_RAX, REG_R10, REG_R11,
    REG_RBX, REG_R12, REG_R13, REG_R14, REG_R15
};
#define NUM_INT_REGS 14

static const PhysicalRegister int_callee_saved_regs[] = {
    REG_RBX, REG_R12, REG_R13, REG_R14, REG_R15
};
#define NUM_CALLEE_SAVED 5

static const PhysicalRegister int_caller_saved_regs[] = {
    REG_RAX, REG_RCX, REG_RDX, REG_RSI, REG_RDI,
    REG_R8, REG_R9, REG_R10, REG_R11
};
#define NUM_CALLER_SAVED 9

static const PhysicalRegister float_caller_saved_regs[] = {
    REG_XMM0, REG_XMM1, REG_XMM2, REG_XMM3,
    REG_XMM4, REG_XMM5, REG_XMM6, REG_XMM7
};
#define NUM_FLOAT_CALLER_SAVED 8

static int compare_intervals(const void* a, const void* b) {
    const LiveInterval* ia = (const LiveInterval*)a;
    const LiveInterval* ib = (const LiveInterval*)b;
    if (ia->start != ib->start) return ia->start - ib->start;
    return ib->end - ia->end;
}

RegisterAllocator* reg_alloc_create(void) {
    RegisterAllocator* alloc = (RegisterAllocator*)calloc(1, sizeof(RegisterAllocator));
    if (!alloc) return NULL;
    alloc->interval_capacity = 256;
    alloc->intervals = (LiveInterval*)calloc(alloc->interval_capacity, sizeof(LiveInterval));
    alloc->int_callee_saved = (PhysicalRegister*)calloc(NUM_CALLEE_SAVED, sizeof(PhysicalRegister));
    alloc->int_caller_saved = (PhysicalRegister*)calloc(NUM_CALLER_SAVED, sizeof(PhysicalRegister));
    alloc->float_caller_saved = (PhysicalRegister*)calloc(NUM_FLOAT_CALLER_SAVED, sizeof(PhysicalRegister));

    if (!alloc->intervals || !alloc->int_callee_saved ||
        !alloc->int_caller_saved || !alloc->float_caller_saved) {
        free(alloc->intervals);
        free(alloc->int_callee_saved);
        free(alloc->int_caller_saved);
        free(alloc->float_caller_saved);
        free(alloc);
        return NULL;
    }

    memcpy(alloc->int_callee_saved, int_callee_saved_regs, NUM_CALLEE_SAVED * sizeof(PhysicalRegister));
    alloc->int_callee_saved_count = NUM_CALLEE_SAVED;
    memcpy(alloc->int_caller_saved, int_caller_saved_regs, NUM_CALLER_SAVED * sizeof(PhysicalRegister));
    alloc->int_caller_saved_count = NUM_CALLER_SAVED;
    memcpy(alloc->float_caller_saved, float_caller_saved_regs, NUM_FLOAT_CALLER_SAVED * sizeof(PhysicalRegister));
    alloc->float_caller_saved_count = NUM_FLOAT_CALLER_SAVED;

    alloc->stack_offset = 0;
    alloc->spill_slots = 0;
    alloc->frame_size = 0;

    return alloc;
}

void reg_alloc_destroy(RegisterAllocator* alloc) {
    if (!alloc) return;
    free(alloc->intervals);
    free(alloc->int_callee_saved);
    free(alloc->int_caller_saved);
    free(alloc->float_caller_saved);
    free(alloc);
}

void reg_alloc_add_interval(RegisterAllocator* alloc, int ir_reg, int start, int end) {
    if (!alloc) return;
    if (alloc->interval_count >= alloc->interval_capacity) {
        int new_cap = alloc->interval_capacity * 2;
        LiveInterval* tmp = (LiveInterval*)realloc(alloc->intervals, new_cap * sizeof(LiveInterval));
        if (!tmp) return;
        alloc->intervals = tmp;
        alloc->interval_capacity = new_cap;
    }
    LiveInterval* interval = &alloc->intervals[alloc->interval_count++];
    interval->ir_reg = ir_reg;
    interval->start = start;
    interval->end = end;
    interval->phys_reg = REG_NONE;
    interval->spill_type = SPILL_NONE;
    interval->spill_offset = -1;
    interval->is_active = 0;
    interval->is_spilled = 0;
}

static int find_free_register(RegisterAllocator* alloc, LiveInterval* current) {
    int used[NUM_INT_REGS];
    memset(used, 0, sizeof(used));

    for (int i = 0; i < alloc->interval_count; i++) {
        LiveInterval* other = &alloc->intervals[i];
        if (other == current || other->phys_reg == REG_NONE || other->is_spilled) continue;
        if (other->start <= current->end && other->end >= current->start) {
            for (int j = 0; j < NUM_INT_REGS; j++) {
                if (int_alloc_order[j] == other->phys_reg) {
                    used[j] = 1;
                    break;
                }
            }
        }
    }

    for (int j = 0; j < NUM_INT_REGS; j++) {
        if (!used[j]) {
            return int_alloc_order[j];
        }
    }
    return REG_NONE;
}

static LiveInterval* find_spill_candidate(RegisterAllocator* alloc, LiveInterval* current) {
    LiveInterval* longest = NULL;
    for (int i = 0; i < alloc->interval_count; i++) {
        LiveInterval* other = &alloc->intervals[i];
        if (other == current || other->phys_reg == REG_NONE || other->is_spilled) continue;
        if (other->start <= current->end && other->end >= current->start) {
            if (!longest || other->end > longest->end) {
                longest = other;
            }
        }
    }
    return longest;
}

static int allocate_spill_slot(RegisterAllocator* alloc) {
    int offset = alloc->stack_offset;
    alloc->stack_offset += 8;
    alloc->spill_slots++;
    return offset;
}

void reg_alloc_linear_scan(RegisterAllocator* alloc) {
    if (!alloc || alloc->interval_count == 0) return;

    qsort(alloc->intervals, alloc->interval_count, sizeof(LiveInterval), compare_intervals);

    int* active = (int*)calloc(alloc->interval_count, sizeof(int));
    if (!active) return;
    int active_count = 0;

    for (int i = 0; i < alloc->interval_count; i++) {
        LiveInterval* current = &alloc->intervals[i];

        int new_active = 0;
        for (int j = 0; j < active_count; j++) {
            LiveInterval* other = &alloc->intervals[active[j]];
            if (other->end >= current->start) {
                active[new_active++] = active[j];
            }
        }
        active_count = new_active;

        PhysicalRegister free_reg = find_free_register(alloc, current);
        if (free_reg != REG_NONE) {
            current->phys_reg = free_reg;
            current->is_active = 1;
        } else {
            LiveInterval* spill = find_spill_candidate(alloc, current);
            if (spill) {
                spill->phys_reg = REG_NONE;
                spill->is_spilled = 1;
                spill->spill_type = SPILL_STACK;
                spill->spill_offset = allocate_spill_slot(alloc);

                current->phys_reg = find_free_register(alloc, current);
                if (current->phys_reg == REG_NONE) {
                    current->phys_reg = REG_NONE;
                    current->is_spilled = 1;
                    current->spill_type = SPILL_STACK;
                    current->spill_offset = allocate_spill_slot(alloc);
                } else {
                    current->is_active = 1;
                }
            } else {
                current->phys_reg = REG_NONE;
                current->is_spilled = 1;
                current->spill_type = SPILL_STACK;
                current->spill_offset = allocate_spill_slot(alloc);
            }
        }

        if (active_count < alloc->interval_count) {
            active[active_count++] = i;
        }
    }

    alloc->frame_size = alloc->stack_offset;
    if (alloc->frame_size % 16 != 0) {
        alloc->frame_size = (alloc->frame_size + 15) & ~15;
    }

    free(active);
}

/* وضع المكدس: الافتراضي مفعّل لأن التخصيص الفيزيائي الحالي يفترض حياة
   سجلات عبر الكتل الأساسية وهو افتراض خاطئ ينهار عند رؤوس الحلقات.
   TODO: مخصص نطاقات حياة صحيح داخل الكتلة ثم عطّل هذا الوضع. */
static int g_stack_mode = 1;

void reg_alloc_set_stack_mode(int enabled) { g_stack_mode = enabled ? 1 : 0; }
int  reg_alloc_get_stack_mode(void)         { return g_stack_mode; }

PhysicalRegister reg_alloc_get_physical(RegisterAllocator* alloc, int ir_reg) {
    if (g_stack_mode) return REG_NONE;
    if (!alloc) return REG_NONE;
    for (int i = 0; i < alloc->interval_count; i++) {
        if (alloc->intervals[i].ir_reg == ir_reg && alloc->intervals[i].phys_reg != REG_NONE) {
            return alloc->intervals[i].phys_reg;
        }
    }
    return REG_NONE;
}

int reg_alloc_is_spilled(RegisterAllocator* alloc, int ir_reg) {
    if (!alloc) return 0;
    for (int i = 0; i < alloc->interval_count; i++) {
        if (alloc->intervals[i].ir_reg == ir_reg) {
            return alloc->intervals[i].is_spilled;
        }
    }
    return 0;
}

int reg_alloc_get_spill_offset(RegisterAllocator* alloc, int ir_reg) {
    if (!alloc) return -1;
    for (int i = 0; i < alloc->interval_count; i++) {
        if (alloc->intervals[i].ir_reg == ir_reg) {
            return alloc->intervals[i].spill_offset;
        }
    }
    return -1;
}

int reg_alloc_get_frame_size(RegisterAllocator* alloc) {
    return alloc ? alloc->frame_size : 0;
}

const char* reg_alloc_phys_name(PhysicalRegister reg) {
    switch (reg) {
        case REG_RAX: return "%rax";
        case REG_RBX: return "%rbx";
        case REG_RCX: return "%rcx";
        case REG_RDX: return "%rdx";
        case REG_RSI: return "%rsi";
        case REG_RDI: return "%rdi";
        case REG_RBP: return "%rbp";
        case REG_RSP: return "%rsp";
        case REG_R8:  return "%r8";
        case REG_R9:  return "%r9";
        case REG_R10: return "%r10";
        case REG_R11: return "%r11";
        case REG_R12: return "%r12";
        case REG_R13: return "%r13";
        case REG_R14: return "%r14";
        case REG_R15: return "%r15";
        case REG_XMM0: return "%xmm0";
        case REG_XMM1: return "%xmm1";
        case REG_XMM2: return "%xmm2";
        case REG_XMM3: return "%xmm3";
        case REG_XMM4: return "%xmm4";
        case REG_XMM5: return "%xmm5";
        case REG_XMM6: return "%xmm6";
        case REG_XMM7: return "%xmm7";
        default: return "???";
    }
}

int reg_alloc_is_callee_saved(RegisterAllocator* alloc, PhysicalRegister reg) {
    if (!alloc) return 0;
    for (int i = 0; i < alloc->int_callee_saved_count; i++) {
        if (alloc->int_callee_saved[i] == reg) return 1;
    }
    return 0;
}

int reg_alloc_is_caller_saved(RegisterAllocator* alloc, PhysicalRegister reg) {
    if (!alloc) return 0;
    for (int i = 0; i < alloc->int_caller_saved_count; i++) {
        if (alloc->int_caller_saved[i] == reg) return 1;
    }
    for (int i = 0; i < alloc->float_caller_saved_count; i++) {
        if (alloc->float_caller_saved[i] == reg) return 1;
    }
    return 0;
}

int reg_alloc_phys_to_spill_slot(PhysicalRegister reg) {
    (void)reg;
    return -1;
}
