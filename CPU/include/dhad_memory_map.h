/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  dhad_memory_map.h — Unified Memory Map for Dhad Processor (v4.5)
 *
 *  Centralizes ALL address map definitions in one place.
 *  Every component (CPU, devices, system) includes this header
 *  to guarantee consistent address decoding.
 *
 *  Memory Map:
 *    0x0000-0xEFFF  Program/RAM (61,440 bytes)
 *    0xF000-0xF0FF  MMIO I/O registers (256 bytes)
 *    0xF100-0xF1FF  Interrupt Vector Table (256 bytes)
 *    0xF200-0xFFFF  Reserved (3,328 bytes)
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef DHAD_MEMORY_MAP_H
#define DHAD_MEMORY_MAP_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ═══════════════════════════════════════
 *  Memory Architecture
 * ═══════════════════════════════════════ */

#define DHAD_MEM_SIZE       65536   /* Total address space: 64 KB */
#define DHAD_STACK_SIZE     64      /* Stack entries (separate array) */
#define DHAD_REG_COUNT      8       /* General registers S0-S7 */

/* ═══════════════════════════════════════
 *  Program Region
 * ═══════════════════════════════════════ */

#define DHAD_PROG_START     0x0000  /* Programs load here */
#define DHAD_PROG_MAX       0xF000  /* Maximum program size (61,440 bytes) */

/* ═══════════════════════════════════════
 *  Memory Regions
 * ═══════════════════════════════════════ */

#define DHAD_IO_BASE        0xF000  /* MMIO I/O registers */
#define DHAD_IO_END         0xF0FF  /* End of I/O region */
#define DHAD_IVT_BASE       0xF100  /* Interrupt Vector Table */
#define DHAD_IVT_END        0xF1FF  /* End of IVT */
#define DHAD_RESERVED_BASE  0xF200  /* Reserved for future use */

/* ═══════════════════════════════════════
 *  Region Size Helpers
 * ═══════════════════════════════════════ */

#define DHAD_IO_SIZE        (DHAD_IO_END - DHAD_IO_BASE + 1)       /* 256 */
#define DHAD_IVT_SIZE       (DHAD_IVT_END - DHAD_IVT_BASE + 1)     /* 256 */
#define DHAD_RAM_SIZE       (DHAD_IO_BASE)                          /* 61,440 */

/* ═══════════════════════════════════════
 *  MMIO Address Decoding
 *
 *  Format: 0xF0DD
 *    bits[7:4] = device index (0-15)
 *    bits[3:0] = register within device (0-15)
 * ═══════════════════════════════════════ */

/** Decode device index from MMIO address */
#define DHAD_MMIO_DEVICE(addr)   (((addr) >> 4) & 0x0F)

/** Decode register from MMIO address */
#define DHAD_MMIO_REG(addr)      ((addr) & 0x0F)

/** Build MMIO address from device index and register */
#define DHAD_MMIO_ADDR(dev, reg) (DHAD_IO_BASE | ((dev) << 4) | (reg))

/** Check if address is in MMIO range */
#define DHAD_IS_MMIO(addr)       ((addr) >= DHAD_IO_BASE && (addr) <= DHAD_IO_END)

/** Check if address is in IVT range */
#define DHAD_IS_IVT(addr)        ((addr) >= DHAD_IVT_BASE && (addr) <= DHAD_IVT_END)

/* ═══════════════════════════════════════
 *  MMIO Device Index Assignments
 * ═══════════════════════════════════════ */

#define DHAD_MMIO_DEV_DISPLAY   0   /* 0xF00x */
#define DHAD_MMIO_DEV_KEYBOARD  1   /* 0xF01x */
#define DHAD_MMIO_DEV_TIMER     2   /* 0xF02x */
#define DHAD_MMIO_DEV_GPIO      3   /* 0xF03x (future) */
#define DHAD_MMIO_DEV_SERIAL    4   /* 0xF04x (future) */
#define DHAD_MMIO_DEV_INTCTL    5   /* 0xF05x (future, replaces INTC) */

/* ═══════════════════════════════════════
 *  MMIO Register Offsets (by device)
 * ═══════════════════════════════════════ */

/* Timer registers */
#define DHAD_TMR_REG_COUNTER_LO  0x00
#define DHAD_TMR_REG_COUNTER_HI  0x01
#define DHAD_TMR_REG_CONTROL     0x02
#define DHAD_TMR_REG_PRESCALER   0x03
#define DHAD_TMR_REG_STATUS      0x04  /* bit0=irq_pending (read: check, write 1: clear) */

/* Timer control bits */
#define DHAD_TMR_CTRL_ENABLE     (1 << 0)
#define DHAD_TMR_CTRL_IRQ_ENABLE (1 << 1)

/* Keyboard registers */
#define DHAD_KB_REG_DATA   0x00
#define DHAD_KB_REG_STATUS 0x01
#define DHAD_KB_REG_CLEAR  0x02

/* Display registers */
#define DHAD_DISPLAY_REG_DATA     0x00
#define DHAD_DISPLAY_REG_STATUS   0x01
#define DHAD_DISPLAY_REG_CURSOR_X 0x02
#define DHAD_DISPLAY_REG_CURSOR_Y 0x03

/* ═══════════════════════════════════════
 *  Flags
 * ═══════════════════════════════════════ */

#define DHAD_FLAG_Z  (1 << 0)   /* Zero */
#define DHAD_FLAG_N  (1 << 1)   /* Negative (bit 7) */
#define DHAD_FLAG_C  (1 << 2)   /* Carry/Borrow */

#ifdef __cplusplus
}
#endif

#endif /* DHAD_MEMORY_MAP_H */
