/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  dhad_system.h — Dhad Virtual System (v4.5)
 *
 *  Central abstraction wrapping CPU + DeviceBus + all devices.
 *  This is the SINGLE entry point for creating and running a Dhad system.
 *  All frontends (CLI, Live, GUI) should use this instead of wiring devices
 *  manually.
 *
 *  Architecture:
 *    DhadSystem
 *     ├── DhadCPU        (core processor)
 *     ├── DhadDeviceBus   (device bus)
 *     ├── DhadDisplay     (display device, bus index 0)
 *     ├── DhadKeyboard    (keyboard device, bus index 1)
 *     └── DhadTimer       (timer device, bus index 2)
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef DHAD_SYSTEM_H
#define DHAD_SYSTEM_H

#include "dhad_cpu.h"
#include "dhad_device.h"
#include "dhad_display.h"
#include "dhad_keyboard.h"
#include "dhad_timer.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ═══════════════════════════════════════
 *  System Configuration
 * ═══════════════════════════════════════ */

/** Bus device indices (fixed assignment) */
#define DHAD_SYS_DEV_DISPLAY   0
#define DHAD_SYS_DEV_KEYBOARD  1
#define DHAD_SYS_DEV_TIMER     2

/** Default system settings */
#define DHAD_SYS_DEFAULT_IVT   DHAD_IVT_BASE

/* ═══════════════════════════════════════
 *  System Instance
 * ═══════════════════════════════════════ */

typedef struct DhadSystem {
    /* Core components */
    DhadCPU        cpu;
    DhadDeviceBus  bus;
    DhadDisplay    display;
    DhadKeyboard   keyboard;
    DhadTimer      timer;

    /* Output routing callback (optional) */
    void         (*output_func)(int value, bool is_char, void *userdata);
    void          *output_userdata;

    /* Display update callback (optional, for GUI frontends) */
    void         (*on_display)(const char *text, int len, void *userdata);
    void          *display_userdata;

    /* State */
    bool           initialized;
} DhadSystem;

/* ═══════════════════════════════════════
 *  System Lifecycle
 * ═══════════════════════════════════════ */

/**
 * Initialize a DhadSystem to clean state.
 * Creates CPU, bus, and all standard devices.
 * Call this before any other system function.
 */
void dhad_system_init(DhadSystem *sys);

/**
 * Reset the system: CPU state + all devices.
 * Program in memory is preserved.
 */
void dhad_system_reset(DhadSystem *sys);

/**
 * Load a program binary into CPU memory.
 * Returns size loaded, or -1 on error.
 */
int dhad_system_load_program(DhadSystem *sys, const uint8_t *program, uint16_t size);

/**
 * Execute one CPU instruction.
 * Also ticks all devices and checks for interrupts.
 * Returns number of bytes consumed (0 if halted).
 */
int dhad_system_step(DhadSystem *sys);

/**
 * Run until halted or max_cycles reached.
 * Calls step() in a loop.
 */
void dhad_system_run(DhadSystem *sys, uint32_t max_cycles);

/**
 * Shutdown the system (cleanup).
 * Currently a no-op but预留 for future resource management.
 */
void dhad_system_shutdown(DhadSystem *sys);

/* ═══════════════════════════════════════
 *  Convenience Accessors
 * ═══════════════════════════════════════ */

/** Get the CPU core (for direct register/state access) */
static inline DhadCPU *dhad_system_cpu(DhadSystem *sys) {
    return &sys->cpu;
}

/** Get the device bus */
static inline DhadDeviceBus *dhad_system_bus(DhadSystem *sys) {
    return &sys->bus;
}

/** Get the display device */
static inline DhadDisplay *dhad_system_display(DhadSystem *sys) {
    return &sys->display;
}

/** Get the keyboard device */
static inline DhadKeyboard *dhad_system_keyboard(DhadSystem *sys) {
    return &sys->keyboard;
}

/** Get the timer device */
static inline DhadTimer *dhad_system_timer(DhadSystem *sys) {
    return &sys->timer;
}

/* ═══════════════════════════════════════
 *  Output Routing
 * ═══════════════════════════════════════ */

/**
 * Set the output callback for PRINT/PRINT_CH instructions.
 * If set, output goes here AND to Display MMIO (if bus attached).
 * If not set, output goes to Display MMIO only (and internal buffer).
 */
void dhad_system_set_output(DhadSystem *sys,
    void (*output_func)(int value, bool is_char, void *userdata),
    void *userdata);

/**
 * Set the display update callback (for GUI frontends).
 * Called when display buffer receives new data.
 */
void dhad_system_set_display_callback(DhadSystem *sys,
    void (*on_display)(const char *text, int len, void *userdata),
    void *userdata);

/* ═══════════════════════════════════════
 *  Keyboard Input
 * ═══════════════════════════════════════ */

/** Push a key into the keyboard buffer (called by frontends) */
void dhad_system_push_key(DhadSystem *sys, uint8_t key);

/** Check if keyboard has data */
bool dhad_system_has_key(DhadSystem *sys);

#ifdef __cplusplus
}
#endif

#endif /* DHAD_SYSTEM_H */
