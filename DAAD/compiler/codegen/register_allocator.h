/**
 * @file register_allocator.h
 * @brief مخصص السجلات - Register Allocator (Linear Scan)
 */

#ifndef DAAD_REGISTER_ALLOCATOR_H
#define DAAD_REGISTER_ALLOCATOR_H

#include "../ir/ir_function.h"
#include "../ir/liveness.h"

typedef enum {
    REG_NONE = 0,
    REG_RAX, REG_RBX, REG_RCX, REG_RDX,
    REG_RSI, REG_RDI, REG_RBP, REG_RSP,
    REG_R8, REG_R9, REG_R10, REG_R11,
    REG_R12, REG_R13, REG_R14, REG_R15,
    REG_XMM0, REG_XMM1, REG_XMM2, REG_XMM3,
    REG_XMM4, REG_XMM5, REG_XMM6, REG_XMM7
} PhysicalRegister;

typedef enum {
    SPILL_NONE,
    SPILL_STACK,
    SPILL_MEM
} SpillType;

typedef struct {
    int ir_reg;
    int start;
    int end;
    PhysicalRegister phys_reg;
    SpillType spill_type;
    int spill_offset;
    int is_active;
    int is_spilled;
} LiveInterval;

typedef struct {
    LiveInterval* intervals;
    int interval_count;
    int interval_capacity;

    PhysicalRegister* int_callee_saved;
    int int_callee_saved_count;
    PhysicalRegister* int_caller_saved;
    int int_caller_saved_count;
    PhysicalRegister* float_caller_saved;
    int float_caller_saved_count;

    int stack_offset;
    int spill_slots;
    int frame_size;
} RegisterAllocator;

RegisterAllocator* reg_alloc_create(void);
void reg_alloc_destroy(RegisterAllocator* alloc);

void reg_alloc_add_interval(RegisterAllocator* alloc, int ir_reg, int start, int end);
void reg_alloc_linear_scan(RegisterAllocator* alloc);
PhysicalRegister reg_alloc_get_physical(RegisterAllocator* alloc, int ir_reg);
/* وضع المكدس الآمن: يعطّل تخصيص السجلات الفيزيائية عبر الكتل (افتراضيًا مفعّل) */
void reg_alloc_set_stack_mode(int enabled);
int  reg_alloc_get_stack_mode(void);
int reg_alloc_is_spilled(RegisterAllocator* alloc, int ir_reg);
int reg_alloc_get_spill_offset(RegisterAllocator* alloc, int ir_reg);
int reg_alloc_get_frame_size(RegisterAllocator* alloc);

const char* reg_alloc_phys_name(PhysicalRegister reg);
int reg_alloc_is_callee_saved(RegisterAllocator* alloc, PhysicalRegister reg);
int reg_alloc_is_caller_saved(RegisterAllocator* alloc, PhysicalRegister reg);
int reg_alloc_phys_to_spill_slot(PhysicalRegister reg);

#endif
