/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  dhad_keyboard.h — Virtual Keyboard Device for Dhad Processor (v4.2)
 *
 *  Provides input to CPU via INPUT instruction.
 *  Frontends push key data into the keyboard buffer;
 *  CPU reads it when executing INPUT.
 *
 *  Keyboard registers:
 *    0x00: DATA    — Read: last key pressed (0 if none)
 *    0x01: STATUS  — Read: bit0=key ready
 *    0x02: CLEAR   — Write 1 to clear buffer
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef DHAD_KEYBOARD_H
#define DHAD_KEYBOARD_H

#include "dhad_device.h"
#include "dhad_memory_map.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DHAD_KEYBOARD_BUF_SIZE 256

/** Keyboard device instance */
typedef struct {
    DhadDevice base;                    /* Must be first */
    uint8_t    buffer[DHAD_KEYBOARD_BUF_SIZE];
    int        head;                    /* Write pointer */
    int        tail;                    /* Read pointer */
    int        count;                   /* Bytes available */
} DhadKeyboard;

/** Initialize a keyboard device */
void dhad_keyboard_init(DhadKeyboard *dev);

/** Push a key into the keyboard buffer (called by frontend) */
void dhad_keyboard_push(DhadKeyboard *dev, uint8_t key);

/** Check if a key is available */
bool dhad_keyboard_has_key(DhadKeyboard *dev);

/** Clear the keyboard buffer */
void dhad_keyboard_clear(DhadKeyboard *dev);

#ifdef __cplusplus
}
#endif

#endif /* DHAD_KEYBOARD_H */
