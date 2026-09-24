/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  dhad_timer.h — Virtual Timer Device for Dhad Processor (v4.2)
 *
 *  Provides deterministic cycle-based timing.
 *  Timer increments a counter each tick (CPU cycle).
 *  NOT wall-clock dependent — fully deterministic.
 *
 *  Timer registers:
 *    0x00: COUNTER_LO  — Read: counter low byte
 *    0x01: COUNTER_HI  — Read: counter high byte
 *    0x02: CONTROL     — bit0=enable, bit1=irq_enable
 *    0x03: PRESCALER   — Divide tick rate (0=every tick, 1=every 2, etc.)
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef DHAD_TIMER_H
#define DHAD_TIMER_H

#include "dhad_device.h"
#include "dhad_memory_map.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Timer device instance */
typedef struct {
    DhadDevice base;                    /* Must be first */
    uint16_t   counter;                 /* 16-bit counter */
    uint8_t    prescaler;               /* Tick divisor */
    uint16_t   prescaler_count;         /* Current prescaler count (uint16_t to prevent overflow at 255) */
    uint8_t    control;                 /* Control register */
    bool       irq_pending;             /* IRQ flag (set when counter overflows) */
} DhadTimer;

/** Initialize a timer device */
void dhad_timer_init(DhadTimer *dev);

/** Read the 16-bit counter value */
uint16_t dhad_timer_get_counter(DhadTimer *dev);

/** Check if timer interrupt is pending */
bool dhad_timer_irq_pending(DhadTimer *dev);

/** Clear the timer IRQ */
void dhad_timer_clear_irq(DhadTimer *dev);

#ifdef __cplusplus
}
#endif

#endif /* DHAD_TIMER_H */
