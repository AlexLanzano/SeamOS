#ifndef DRIVER_ST7789_H
#define DRIVER_ST7789_H

#include <stdint.h>

typedef enum st7789_color_format {
    ST7789_COLOR_FORMAT_12_BIT = 0x53,
    ST7789_COLOR_FORMAT_16_BIT = 0x55,
    ST7789_COLOR_FORMAT_18_BIT = 0x66,
} st7789_color_format_t;

typedef struct st7789_configuration {
    uint32_t interface_handle;
    uint32_t cs_pin;
    uint32_t dc_pin;
    uint32_t width;
    uint32_t height;
    void *framebuffer;
    st7789_color_format_t color_format;
} st7789_configuration_t;

#endif
