/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  dhad_display.h — Virtual Display Device for Dhad Processor (v4.2)
 *
 *  Receives bytes from CPU via PRINT/PRINT_CH instructions.
 *  Provides a buffer of characters that frontends can render.
 *
 *  Display registers:
 *    0x00: DATA    — Write: append byte to display buffer
 *    0x01: STATUS  — Read: 0=ready (always)
 *    0x02: CURSOR_X — Read/Write: cursor X position
 *    0x03: CURSOR_Y — Read/Write: cursor Y position
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef DHAD_DISPLAY_H
#define DHAD_DISPLAY_H

#include "dhad_device.h"
#include "dhad_memory_map.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DHAD_DISPLAY_BUF_SIZE 4096

/** Display device instance */
typedef struct {
    DhadDevice base;                    /* Must be first */
    char       buffer[DHAD_DISPLAY_BUF_SIZE];
    int        buf_len;
    uint8_t    cursor_x;
    uint8_t    cursor_y;
    void     (*on_display)(const char *text, int len, void *userdata);
    void      *display_userdata;
} DhadDisplay;

/** Initialize a display device */
void dhad_display_init(DhadDisplay *dev);

/** Get the display buffer contents */
const char *dhad_display_get_buffer(DhadDisplay *dev);

/** Get buffer length */
int dhad_display_get_length(DhadDisplay *dev);

/** Clear the display buffer */
void dhad_display_clear(DhadDisplay *dev);

/** Set callback for when display updates */
void dhad_display_set_callback(DhadDisplay *dev,
    void (*on_display)(const char *text, int len, void *userdata),
    void *userdata);

#ifdef __cplusplus
}
#endif

#endif /* DHAD_DISPLAY_H */
