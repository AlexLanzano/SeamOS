#ifndef LS013B7H05_H
#define LS013B7H05_H

#include <stdint.h>
#include <mcu/gpio.h>
#include <mcu/spi.h>
#include <libraries/error.h>

typedef enum display_draw_attr{
    DISPLAY_DRAW_ATTR_NORMAL,
    DISPLAY_DRAW_ATTR_INVERT
} display_draw_attr_t;

typedef struct ls013b7h05_configuration {
    gpio_handle_t cs_pin_handle;
    spi_handle_t spi_handle;
} ls013b7h05_configuration_t;

error_t ls013b7h05_init(ls013b7h05_configuration_t config);
error_t ls013b7h05_render();
error_t ls013b7h05_draw_pixel(uint32_t x, uint32_t y, uint8_t value);
error_t ls013b7h05_draw_bitmap(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint8_t *bitmap);
void ls013b7h05_clear();
void ls013b7h05_set_draw_attr(display_draw_attr_t attr);

#endif
