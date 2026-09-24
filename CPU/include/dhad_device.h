/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  dhad_device.h — Device Abstraction Layer for Dhad Processor (v4.2)
 *
 *  Provides a generic device interface that the CPU can use to communicate
 *  with virtual hardware. The CPU Core does NOT know device specifics —
 *  it only sees a uniform read/write interface.
 *
 *  Device Bus Architecture:
 *    CPU
 *     |
 *     +-- Memory (64KB address space)
 *     |
 *     +-- Device Bus
 *           |
 *           +-- Display   (0xF0-0xF7 mapped or callback)
 *           +-- Keyboard  (0xF8-0xFF mapped or callback)
 *           +-- Timer     (callback-based)
 *           +-- GPIO      (future)
 *           +-- Serial    (future)
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef DHAD_DEVICE_H
#define DHAD_DEVICE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ═══════════════════════════════════════
 *  Device Type IDs
 * ═══════════════════════════════════════ */

#define DHAD_DEV_TYPE_NONE      0
#define DHAD_DEV_TYPE_DISPLAY   1
#define DHAD_DEV_TYPE_KEYBOARD  2
#define DHAD_DEV_TYPE_TIMER     3
#define DHAD_DEV_TYPE_GPIO      4
#define DHAD_DEV_TYPE_SERIAL    5

/* ═══════════════════════════════════════
 *  Device Status Codes
 * ═══════════════════════════════════════ */

#define DHAD_DEV_OK             0
#define DHAD_DEV_ERR_NOT_READY  (-1)
#define DHAD_DEV_ERR_NO_DATA    (-2)
#define DHAD_DEV_ERR_BUSY       (-3)

/* ═══════════════════════════════════════
 *  Device Interface (Vtable)
 *
 *  Each device implements these functions.
 *  The CPU never calls device functions directly —
 *  it goes through the DhadDeviceBus.
 * ═══════════════════════════════════════ */

/** Forward declaration */
typedef struct DhadDevice DhadDevice;

/** Device vtable — each device type implements these */
typedef struct {
    const char *name;               /* Human-readable name */
    uint8_t     type;               /* DHAD_DEV_TYPE_* */

    /** Initialize device (called once at startup) */
    void     (*init)(DhadDevice *dev);

    /** Reset device to initial state */
    void     (*reset)(DhadDevice *dev);

    /** Read a byte from device register. Returns 0-255 or DHAD_DEV_ERR_* */
    int      (*read)(DhadDevice *dev, uint8_t reg);

    /** Write a byte to device register */
    void     (*write)(DhadDevice *dev, uint8_t reg, uint8_t val);

    /** Tick the device (called once per CPU cycle) */
    void     (*tick)(DhadDevice *dev);

    /** Cleanup (called once at shutdown) */
    void     (*destroy)(DhadDevice *dev);

    /** Private data pointer (device-specific) */
    void    *priv;
} DhadDeviceVtable;

/** Device instance */
struct DhadDevice {
    DhadDeviceVtable vtable;
    bool             enabled;
};

/* ═══════════════════════════════════════
 *  Device Bus
 *
 *  Connects up to 16 devices to the CPU.
 *  The CPU reads/writes through the bus.
 * ═══════════════════════════════════════ */

#define DHAD_BUS_MAX_DEVICES 16

typedef struct DhadDeviceBus {
    DhadDevice *devices[DHAD_BUS_MAX_DEVICES];
    int         count;
} DhadDeviceBus;

/* ═══════════════════════════════════════
 *  Bus API
 * ═══════════════════════════════════════ */

/** Initialize an empty device bus */
void dhad_bus_init(DhadDeviceBus *bus);

/** Register a device on the bus. Returns device index or -1 on error. */
int dhad_bus_add(DhadDeviceBus *bus, DhadDevice *dev);

/** Remove a device from the bus by index */
void dhad_bus_remove(DhadDeviceBus *bus, int index);

/** Find a device by type. Returns index or -1 if not found. */
int dhad_bus_find(DhadDeviceBus *bus, uint8_t type);

/** Get device pointer by index. Returns NULL if invalid or NULL device. */
DhadDevice *dhad_bus_get_device(DhadDeviceBus *bus, int index);

/** Read from device at index. Returns 0-255 or DHAD_DEV_ERR_* */
int dhad_bus_read(DhadDeviceBus *bus, int index, uint8_t reg);

/** Write to device at index */
void dhad_bus_write(DhadDeviceBus *bus, int index, uint8_t reg, uint8_t val);

/* ═══════════════════════════════════════
 *  Memory-Mapped I/O Dispatch
 * ═══════════════════════════════════════ */

/**
 * Dispatch a read from an I/O address (0xF000-0xF0FF).
 * Address decoding: bits[7:4] = device, bits[3:0] = register.
 * Device map: 0=Display, 1=Keyboard, 2=Timer, 3=INTC.
 * Returns 0 for unmapped addresses.
 */
int dhad_bus_read_io(DhadDeviceBus *bus, uint16_t addr);

/**
 * Dispatch a write to an I/O address (0xF000-0xF0FF).
 * Address decoding: bits[7:4] = device, bits[3:0] = register.
 * Silently ignores unmapped addresses.
 */
void dhad_bus_write_io(DhadDeviceBus *bus, uint16_t addr, uint8_t val);

/** Tick all enabled devices on the bus */
void dhad_bus_tick_all(DhadDeviceBus *bus);

/** Reset all devices on the bus */
void dhad_bus_reset_all(DhadDeviceBus *bus);

#ifdef __cplusplus
}
#endif

#endif /* DHAD_DEVICE_H */
