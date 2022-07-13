#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <libraries/error.h>
#include <libraries/device.h>
#include <libraries/string.h>

typedef struct display {
    device_handle_t handle;
    uint32_t width;
    uint32_t height;
    void *framebuffer;
} display_t;

error_t display_init(device_handle_t handle, display_t *display);
error_t display_draw_filled_rect(display_t *display, uint32_t x, uint32_t y, uint32_t width, uint32_t height,
                                 uint16_t color);
error_t display_update(display_t *display, uint32_t x, uint32_t y, uint32_t width, uint32_t height);

#endif
