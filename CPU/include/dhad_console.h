/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  dhad_console.h — Dhad Console Abstraction (v4.5)
 *
 *  High-level console I/O over Display + Keyboard MMIO devices.
 *  Frontends use this instead of directly manipulating MMIO registers.
 *
 *  Provides:
 *    - Character/string output (to Display)
 *    - Character input (from Keyboard)
 *    - Display clear
 *    - Buffer access (for rendering)
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef DHAD_CONSOLE_H
#define DHAD_CONSOLE_H

#include "dhad_system.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ═══════════════════════════════════════
 *  Console Instance
 * ═══════════════════════════════════════ */

typedef struct DhadConsole {
    DhadSystem *sys;       /* Back-pointer to system */
} DhadConsole;

/* ═══════════════════════════════════════
 *  Console Lifecycle
 * ═══════════════════════════════════════ */

/** Initialize console attached to a system */
void dhad_console_init(DhadConsole *con, DhadSystem *sys);

/* ═══════════════════════════════════════
 *  Output (to Display)
 * ═══════════════════════════════════════ */

/** Write a single character to the display */
void dhad_console_putc(DhadConsole *con, char c);

/** Write a null-terminated string to the display */
void dhad_console_puts(DhadConsole *con, const char *str);

/** Write a buffer of specified length to the display */
void dhad_console_write(DhadConsole *con, const char *buf, int len);

/** Clear the display */
void dhad_console_clear(DhadConsole *con);

/* ═══════════════════════════════════════
 *  Input (from Keyboard)
 * ═══════════════════════════════════════ */

/** Check if a key is available */
bool dhad_console_kbhit(DhadConsole *con);

/** Read a character from keyboard (returns 0 if none available) */
int dhad_console_getc(DhadConsole *con);

/** Push a key into the keyboard buffer (for frontends) */
void dhad_console_push_key(DhadConsole *con, uint8_t key);

/* ═══════════════════════════════════════
 *  Display Buffer Access
 * ═══════════════════════════════════════ */

/** Get the display buffer contents */
const char *dhad_console_get_buffer(DhadConsole *con);

/** Get display buffer length */
int dhad_console_get_length(DhadConsole *con);

#ifdef __cplusplus
}
#endif

#endif /* DHAD_CONSOLE_H */
