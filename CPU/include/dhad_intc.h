/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  dhad_intc.h — Interrupt Controller for Dhad Processor (v4.2)
 *
 *  Wraps the interrupt logic currently inline in the CPU.
 *  Provides a clean abstraction for:
 *    - Requesting interrupts
 *    - Masking individual IRQ lines
 *    - Checking pending interrupts
 *    - Delivering interrupts (vector lookup)
 *
 *  The CPU calls dhad_intc_check() at the start of each step.
 *  Devices call dhad_intc_request() to raise an IRQ.
 *
 *  Interrupt Architecture:
 *    8 interrupt lines (IRQ 0-7)
 *    IRQ 0 = highest priority
 *    IRQ 7 = lowest priority
 *    IVT base address + (IRQ * 4) = ISR address
 *    On delivery: push FLAGS, push PC, disable interrupts, jump to ISR
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef DHAD_INTC_H
#define DHAD_INTC_H

#include "dhad_cpu.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Interrupt numbers */
#define DHAD_IRQ_TIMER    0
#define DHAD_IRQ_KEYBOARD 1
#define DHAD_IRQ_SERIAL   2
#define DHAD_IRQ_GPIO     3
#define DHAD_IRQ_USER4    4
#define DHAD_IRQ_USER5    5
#define DHAD_IRQ_USER6    6
#define DHAD_IRQ_USER7    7

#define DHAD_IRQ_COUNT    8

/** Interrupt controller state */
typedef struct {
    bool     enabled;               /* Global interrupt enable */
    uint8_t  mask;                  /* Per-IRQ mask (1=masked) */
    uint8_t  pending;               /* Pending interrupt flags */
    uint16_t ivt_addr;             /* IVT base address */
} DhadInterruptController;

/** Initialize the interrupt controller */
void dhad_intc_init(DhadInterruptController *intc, uint16_t ivt_addr);

/** Request an interrupt (device calls this) */
void dhad_intc_request(DhadInterruptController *intc, uint8_t irq);

/** Clear a pending interrupt (called after delivery) */
void dhad_intc_clear(DhadInterruptController *intc, uint8_t irq);

/** Check if an interrupt should be delivered. Returns IRQ number (0-7) or -1. */
int dhad_intc_check(DhadInterruptController *intc);

/** Get the ISR address for an IRQ */
uint16_t dhad_intc_get_vector(DhadInterruptController *intc, uint8_t irq);

/** Enable/disable global interrupts */
void dhad_intc_enable(DhadInterruptController *intc, bool enable);

/** Set/clear mask for a specific IRQ */
void dhad_intc_set_mask(DhadInterruptController *intc, uint8_t irq, bool masked);

/** Get pending flags */
uint8_t dhad_intc_get_pending(DhadInterruptController *intc);

#ifdef __cplusplus
}
#endif

#endif /* DHAD_INTC_H */
