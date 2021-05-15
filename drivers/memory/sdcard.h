#ifndef SDCARD_H
#define SDCARD_H

#include <stdint.h>
#include <mcu/gpio.h>
#include <mcu/spi.h>
#include <libraries/error.h>

typedef struct sdcard_configuration {
    gpio_handle_t cs_pin_handle;
    spi_handle_t spi_handle;
} sdcard_configuration_t;

error_t sdcard_read_block(uint32_t addr, void *buffer);
error_t sdcard_write_block(uint32_t addr, void *buffer);
error_t sdcard_init(sdcard_configuration_t config);

#endif
