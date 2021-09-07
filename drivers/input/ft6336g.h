#ifndef FT6336G_H
#define FT6336G_H

#include <libraries/error.h>
#include <mcu/i2c.h>

typedef uint32_t ft6336g_handle_t;

typedef enum ft6336g_interrupt_mode {
    FT6336G_INTERRUPT_MODE_POLLING,
    FT6336G_INTERRUPT_MODE_TRIGGER
} ft6336g_interrupt_mode_t;

typedef enum ft6336g_event_flag {
    FT6336G_EVENT_FLAG_PRESS_DOWN,
    FT6336G_EVENT_FLAG_LIFT_UP,
    FT6336G_EVENT_FLAG_CONTACT,
    FT6336G_EVENT_FLAG_NO_EVENT
} ft6336g_event_flag_t;

typedef struct ft6336g_configuration {
    i2c_handle_t i2c_handle;
    uint8_t threshold;
    ft6336g_interrupt_mode_t interrupt_mode;
} ft6336g_configuration_t;

error_t ft6336g_init(ft6336g_configuration_t config, ft6336g_handle_t *handle);
error_t ft6336g_deinit(ft6336g_handle_t handle);
error_t ft6336g_read(ft6336g_handle_t handle, uint16_t *x, uint16_t *y, ft6336g_event_flag_t *event_flag);

#endif
