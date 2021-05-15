#include <stdint.h>
#include <mcu/gpio.h>
#include <mcu/spi.h>
#include <libraries/string.h>
#include <kernel/debug/log.h>
#include <libraries/error.h>
#include <drivers/display/ls013b7h05.h>

#define DISPLAY_WIDTH 144
#define DISPLAY_HEIGHT 168

static uint8_t vcom = 0;
static uint32_t display_buffer_size = (DISPLAY_WIDTH * DISPLAY_HEIGHT) / 8;
static uint8_t display_buffer[(DISPLAY_WIDTH * DISPLAY_HEIGHT) / 8];
static display_draw_attr_t g_draw_attr = DISPLAY_DRAW_ATTR_NORMAL;

static gpio_handle_t g_cs_pin_handle;
static spi_handle_t g_spi_handle;

static uint8_t reverse_byte(uint8_t value)
{
    uint32_t v = value;
    __asm__("rbit %0, %0"
            : "=r" (v)
            :);
    return v >> 24;
}

error_t ls013b7h05_init(ls013b7h05_configuration_t config)
{
    g_cs_pin_handle = config.cs_pin_handle;
    g_spi_handle = config.spi_handle;

    // Zero out display buffer
    memset(display_buffer, 0, display_buffer_size);

    return SUCCESS;
}

error_t ls013b7h05_render()
{
    error_t error;

    // set Chip Select to high
    error = gpio_write(g_cs_pin_handle, 1);
    if (error) {
        log_error(error, "Failed to enable device");
        return error;
    }

    // Toggle vcom
    vcom = (vcom) ? 0 : 1;

    uint8_t update_data_msg = (vcom<<6) | 0x80;
    error = spi_write(g_spi_handle, &update_data_msg, 1);
    if (error) {
        log_error(error, "Failed to write update data to display");
        goto exit;
    }

    for (uint32_t line = 0; line < DISPLAY_HEIGHT; line++) {
        uint8_t msg[(DISPLAY_WIDTH/8)+2];

        // Write the line address
        msg[0] = reverse_byte(line + 1);

        // Copy over buffer data
        memcpy(&msg[1], &display_buffer[(line)*(DISPLAY_WIDTH/8)], DISPLAY_WIDTH/8);

        // Write end byte
        msg[(DISPLAY_WIDTH/8)+1] = 0;

        // Send it
        error = spi_write(g_spi_handle, msg, (DISPLAY_WIDTH/8)+2);
        if (error) {
            log_error(error, "Failed to write pixel data to display");
            goto exit;
        }
    }

 exit:
    // set Chip select to low
    error = gpio_write(g_cs_pin_handle, 0);
    if (error) {
        log_error(error, "Failed to disable device");
    }

    return error;
}

error_t ls013b7h05_draw_pixel(uint32_t x, uint32_t y, uint8_t value)
{
    if (x > DISPLAY_WIDTH || y > DISPLAY_HEIGHT) {
        log_error(ERROR_INVALID, "x:%u or y:%u exceeds display width:%u or height:%u",
                  x, y, DISPLAY_WIDTH, DISPLAY_HEIGHT);
        return ERROR_INVALID;
    }

    uint32_t byte = ((y * DISPLAY_WIDTH) + (x)) / 8;
    uint8_t bit = 7 - (x % 8);

    if (g_draw_attr == DISPLAY_DRAW_ATTR_NORMAL) {
        if (value) {
            display_buffer[byte] |= 1 << bit;
        } else {
            display_buffer[byte] &= ~(1 << bit);
        }
    }

    if (g_draw_attr == DISPLAY_DRAW_ATTR_INVERT) {
        if (value) {
            display_buffer[byte] &= ~(1 << bit);
        } else {
            display_buffer[byte] |= 1 << bit;
        }
    }

    return SUCCESS;
}

error_t ls013b7h05_draw_bitmap(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint8_t *bitmap)
{
    if (x > DISPLAY_WIDTH || y > DISPLAY_HEIGHT) {
        log_error(ERROR_INVALID, "x:%u or y:%u exceeds display width:%u or height:%u\r\n",
                  x, y, DISPLAY_WIDTH, DISPLAY_HEIGHT);
        return ERROR_INVALID;
    }

    if (x + width > DISPLAY_WIDTH || y + height > DISPLAY_HEIGHT) {
        log_error(ERROR_INVALID, "bitmap exceeds screen bounds");
        return ERROR_INVALID;
    }

    // TODO: Optimize this
    for (uint32_t y_1 = 0; y_1 < height; y_1++) {
        for (uint32_t x_1 = 0; x_1 < width; x_1++) {
            error_t error = ls013b7h05_draw_pixel(x + x_1, y + y_1, (bitmap[y_1] & (1 << (8-x_1))));
            if (error) {
                log_error(error, "Unable to draw pixel to display");
                return error;
            }
        }
    }

    return SUCCESS;
}

void ls013b7h05_clear()
{
    memset(display_buffer, 0, display_buffer_size);
}

void ls013b7h05_set_draw_attr(display_draw_attr_t attr)
{
    g_draw_attr = attr;
}
