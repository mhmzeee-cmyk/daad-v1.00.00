/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  dhad_runtime.h — DHAD Runtime API (v4.6)
 *
 *  The Runtime is the ONLY interface between DHAD programs and the outside world.
 *  Programs never call Linux syscalls, malloc, stdout, or x86 instructions directly.
 *  They call Runtime APIs, which dispatch to the active backend.
 *
 *  Architecture:
 *    DHAD Program
 *         ↓
 *    DHAD Runtime API  (this file)
 *         ↓
 *    Backend vtable
 *         ↓
 *    ┌─────────────────────────┐
 *    │  Emulator Backend       │  → DhadCPU → cycle execution
 *    │  Host Backend           │  → Linux syscalls → Intel/AMD
 *    └─────────────────────────┘
 *
 *  Usage:
 *    dhad_runtime_init(DHAD_BACKEND_EMULATOR);
 *    dhad_runtime_load_program(data, size);
 *    dhad_runtime_run(100000);
 *    dhad_runtime_shutdown();
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef DHAD_RUNTIME_H
#define DHAD_RUNTIME_H

#include "dhad_backend.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ═══════════════════════════════════════
 *  Runtime Lifecycle
 * ═══════════════════════════════════════ */

/**
 * Initialize the DHAD Runtime with the specified backend.
 * Returns 0 on success, -1 on error.
 */
int dhad_runtime_init(DhadBackendType backend_type);

/** Shutdown the runtime and cleanup */
void dhad_runtime_shutdown(void);

/* ═══════════════════════════════════════
 *  Program Loading
 * ═══════════════════════════════════════ */

/**
 * Load a DHAD binary into the backend.
 * Returns size loaded, or -1 on error.
 */
int dhad_runtime_load_program(const uint8_t *data, uint16_t size);

/* ═══════════════════════════════════════
 *  Execution
 * ═══════════════════════════════════════ */

/** Execute until HALT or max_cycles. Returns exit status. */
int dhad_runtime_run(uint32_t max_cycles);

/** Execute one step. Returns bytes consumed (0 if halted). */
int dhad_runtime_step(void);

/* ═══════════════════════════════════════
 *  Console I/O (programs call these)
 * ═══════════════════════════════════════ */

/** Write a character to the console/display */
void dhad_runtime_putchar(char c);

/** Read a character from input (returns -1 if none available) */
int dhad_runtime_getchar(void);

/** Write a string to the console/display */
void dhad_runtime_puts(const char *str);

/** Write a buffer of specified length */
void dhad_runtime_write(const char *buf, int len);

/** Read a buffer from input. Returns bytes read. */
int dhad_runtime_read(char *buf, int max_len);

/* ═══════════════════════════════════════
 *  Program Control
 * ═══════════════════════════════════════ */

/** Exit the program with a status code */
void dhad_runtime_exit(int status);

/** Check if the program is halted */
bool dhad_runtime_is_halted(void);

/** Get the cycle count */
uint32_t dhad_runtime_get_cycles(void);

/** Get the ACC register value */
uint8_t dhad_runtime_get_acc(void);

/** Get exit status after run */
int dhad_runtime_get_exit_status(void);

/* ═══════════════════════════════════════
 *  Backend Access
 * ═══════════════════════════════════════ */

/** Get the active backend (may be NULL) */
DhadBackend *dhad_runtime_get_backend(void);

/** Get the backend type */
DhadBackendType dhad_runtime_get_backend_type(void);

#ifdef __cplusplus
}
#endif

#endif /* DHAD_RUNTIME_H */
