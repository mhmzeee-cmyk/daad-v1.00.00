/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  dhad_backend.h — Backend Abstraction for DHAD Runtime (v4.6)
 *
 *  Defines the interface that all backends (emulator, native host) must implement.
 *  The Runtime dispatches calls to the active backend.
 *
 *  Backend types:
 *    EMULATOR — runs DHAD binary through DhadCPU (cycle-accurate)
 *    HOST     — runs DHAD program natively on Intel/AMD via Linux syscalls
 *
 *  This interface is the bridge between:
 *    DHAD Runtime API  ←→  Backend Implementation
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef DHAD_BACKEND_H
#define DHAD_BACKEND_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ═══════════════════════════════════════
 *  Backend Type
 * ═══════════════════════════════════════ */

typedef enum {
    DHAD_BACKEND_EMULATOR = 0,
    DHAD_BACKEND_HOST     = 1,
} DhadBackendType;

/* ═══════════════════════════════════════
 *  Backend vtable
 *
 *  Each backend implements these functions.
 *  The Runtime never calls backend functions directly —
 *  it goes through this vtable.
 * ═══════════════════════════════════════ */

typedef struct DhadBackend DhadBackend;

typedef struct {
    const char *name;
    DhadBackendType type;

    /** Initialize the backend */
    int      (*init)(DhadBackend *backend);

    /** Shutdown and cleanup */
    void     (*shutdown)(DhadBackend *backend);

    /** Load a DHAD binary into the backend */
    int      (*load_program)(DhadBackend *backend, const uint8_t *data, uint16_t size);

    /** Execute until HALT or max_cycles. Returns exit status. */
    int      (*run)(DhadBackend *backend, uint32_t max_cycles);

    /** Execute one step. Returns bytes consumed (0 if halted). */
    int      (*step)(DhadBackend *backend);

    /** Write a character to the console/display */
    void     (*putchar)(DhadBackend *backend, char c);

    /** Read a character from input (returns -1 if none available) */
    int      (*getchar)(DhadBackend *backend);

    /** Write a buffer to the console/display */
    void     (*write)(DhadBackend *backend, const char *buf, int len);

    /** Read a buffer from input. Returns bytes read. */
    int      (*read)(DhadBackend *backend, char *buf, int max_len);

    /** Exit the program with a status code */
    void     (*exit_program)(DhadBackend *backend, int status);

    /** Check if the program is halted */
    bool     (*is_halted)(DhadBackend *backend);

    /** Get the cycle count */
    uint32_t (*get_cycles)(DhadBackend *backend);

    /** Get the ACC register value */
    uint8_t  (*get_acc)(DhadBackend *backend);

    /** Get exit status after run */
    int      (*get_exit_status)(DhadBackend *backend);
} DhadBackendVtable;

struct DhadBackend {
    DhadBackendVtable vtable;
    bool              initialized;
    void             *priv;
};

/* ═══════════════════════════════════════
 *  Backend Vtable Instances
 * ═══════════════════════════════════════ */

extern DhadBackendVtable dhad_backend_emulator_vtable;
extern DhadBackendVtable dhad_backend_host_vtable;

/* ═══════════════════════════════════════
 *  Runtime Internal (used by dhad_runtime.c)
 * ═══════════════════════════════════════ */

/** Set the active backend in the Runtime */
void dhad_runtime_set_backend(DhadBackend *backend);

#ifdef __cplusplus
}
#endif

#endif /* DHAD_BACKEND_H */
