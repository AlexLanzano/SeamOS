#ifndef ST7789_H
#define ST7789_H

#include <stdint.h>

typedef uint32_t st7789_handle_t;
typedef uint16_t color16_t;

typedef enum st7789_color_format {
    ST7789_COLOR_FORMAT_12_BIT = 0x53,
    ST7789_COLOR_FORMAT_16_BIT = 0x55,
    ST7789_COLOR_FORMAT_18_BIT = 0x66,
} st7789_color_format_t;

typedef struct st7789_configuration {
    spi_device_handle_t spi_handle;
    gpio_handle_t dc_pin_handle;
    uint32_t framebuffer_size;
    st7789_color_format_t color_format;
    uint16_t width;
    uint16_t height;
} st7789_configuration_t;

error_t st7789_init(st7789_configuration_t config, st7789_handle_t *handle);
error_t st7789_deinit(st7789_handle_t handle);
error_t st7789_render(st7789_handle_t handle);
error_t st7789_draw_pixel(st7789_handle_t handle, uint32_t x, uint32_t y, color16_t color);
error_t st7789_draw_filled_rect(st7789_handle_t handle, uint32_t x, uint32_t y, uint32_t width, uint32_t height, color16_t color);
error_t st7789_clear(st7789_handle_t handle);

#endif
