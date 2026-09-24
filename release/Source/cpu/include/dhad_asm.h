#ifndef DHAD_ASM_H
#define DHAD_ASM_H

#include <stdint.h>

#define ASM_PROG_MAX 0xF000
#define ASM_MAX_LABELS 256
#define ASM_MAX_EQUATES 256
#define ASM_MAX_MACROS 64
#define ASM_MAX_MACRO_LINES 32
#define ASM_MAX_IF_NESTING 32

typedef struct {
    char name[64];
    uint16_t addr;
} AsmLabel;

typedef struct {
    char name[64];
    int value;
} AsmEquate;

typedef struct {
    char name[64];
    char lines[ASM_MAX_MACRO_LINES][256];
    int line_count;
} AsmMacro;

typedef struct {
    AsmLabel  labels[ASM_MAX_LABELS];
    int       label_count;
    AsmEquate equates[ASM_MAX_EQUATES];
    int       equate_count;
    AsmMacro  macros[ASM_MAX_MACROS];
    int       macro_count;
    int       in_macro_def;
    char      current_macro[64];
    int       if_nesting;
    int       if_skip_stack[ASM_MAX_IF_NESTING];
    int       error_count;
    uint8_t   program[ASM_PROG_MAX];
    int       prog_size;
    int       pass;
    int       line;
    char      last_error[512];
} DhadAsm;

#ifdef __cplusplus
extern "C" {
#endif

int dhad_asm(DhadAsm *a, const char *filename);

#ifdef __cplusplus
}
#endif

#endif
